// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <utility>

#include "base/metrics/histogram.h"
#include "chrome/browser/chromeos/gdata/drive_resource_metadata.h"
#include "chrome/browser/chromeos/gdata/drive_files.h"
#include "chrome/browser/chromeos/gdata/gdata_wapi_feed_processor.h"
#include "content/public/browser/browser_thread.h"

using content::BrowserThread;

namespace gdata {

FeedToFileResourceMapUmaStats::FeedToFileResourceMapUmaStats()
    : num_regular_files(0),
      num_hosted_documents(0) {
}

FeedToFileResourceMapUmaStats::~FeedToFileResourceMapUmaStats() {
}

GDataWapiFeedProcessor::GDataWapiFeedProcessor(
    DriveResourceMetadata* resource_metadata)
  : resource_metadata_(resource_metadata) {
}

GDataWapiFeedProcessor::~GDataWapiFeedProcessor() {
}

DriveFileError GDataWapiFeedProcessor::ApplyFeeds(
    const ScopedVector<DocumentFeed>& feed_list,
    int64 start_changestamp,
    int64 root_feed_changestamp,
    std::set<FilePath>* changed_dirs) {
  bool is_delta_feed = start_changestamp != 0;

  resource_metadata_->set_origin(FROM_SERVER);

  int64 delta_feed_changestamp = 0;
  FeedToFileResourceMapUmaStats uma_stats;
  FileResourceIdMap file_map;
  DriveFileError error = FeedToFileResourceMap(feed_list,
                                               &file_map,
                                               &delta_feed_changestamp,
                                               &uma_stats);
  if (error != DRIVE_FILE_OK)
    return error;

  ApplyFeedFromFileUrlMap(
      is_delta_feed,
      is_delta_feed ? delta_feed_changestamp : root_feed_changestamp,
      &file_map,
      changed_dirs);

  // Shouldn't record histograms when processing delta feeds.
  if (!is_delta_feed)
    UpdateFileCountUmaHistograms(uma_stats);

  return DRIVE_FILE_OK;
}

void GDataWapiFeedProcessor::UpdateFileCountUmaHistograms(
    const FeedToFileResourceMapUmaStats& uma_stats) const {
  const int num_total_files =
      uma_stats.num_hosted_documents + uma_stats.num_regular_files;
  UMA_HISTOGRAM_COUNTS("Drive.NumberOfRegularFiles",
                       uma_stats.num_regular_files);
  UMA_HISTOGRAM_COUNTS("Drive.NumberOfHostedDocuments",
                       uma_stats.num_hosted_documents);
  UMA_HISTOGRAM_COUNTS("Drive.NumberOfTotalFiles", num_total_files);
  for (FeedToFileResourceMapUmaStats::EntryKindToCountMap::const_iterator iter =
           uma_stats.num_files_with_entry_kind.begin();
       iter != uma_stats.num_files_with_entry_kind.end();
       ++iter) {
    const DriveEntryKind kind = iter->first;
    const int count = iter->second;
    for (int i = 0; i < count; ++i) {
      UMA_HISTOGRAM_ENUMERATION(
          "Drive.EntryKind", kind, ENTRY_KIND_MAX_VALUE);
    }
  }
  for (FeedToFileResourceMapUmaStats::FileFormatToCountMap::const_iterator
           iter = uma_stats.num_files_with_file_format.begin();
       iter != uma_stats.num_files_with_file_format.end();
       ++iter) {
    const DriveFileFormat format = iter->first;
    const int count = iter->second;
    for (int i = 0; i < count; ++i) {
      UMA_HISTOGRAM_ENUMERATION(
          "Drive.FileFormat", format, FILE_FORMAT_MAX_VALUE);
    }
  }
}

void GDataWapiFeedProcessor::ApplyFeedFromFileUrlMap(
    bool is_delta_feed,
    int64 feed_changestamp,
    FileResourceIdMap* file_map,
  std::set<FilePath>* changed_dirs) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(changed_dirs);

  if (!is_delta_feed) {  // Full update.
    resource_metadata_->root()->RemoveChildren();
    changed_dirs->insert(resource_metadata_->root()->GetFilePath());
  }
  resource_metadata_->set_largest_changestamp(feed_changestamp);

  scoped_ptr<DriveResourceMetadata> orphaned_resources(
      new DriveResourceMetadata);
  // Go through all entries generated by the feed and apply them to the local
  // snapshot of the file system.
  for (FileResourceIdMap::iterator it = file_map->begin();
       it != file_map->end();) {
    // Ensure that the entry is deleted, unless the ownership is explicitly
    // transferred by entry.release().
    scoped_ptr<DriveEntry> entry(it->second);
    DCHECK_EQ(it->first, entry->resource_id());
    // Erase the entry so the deleted entry won't be referenced.
    file_map->erase(it++);

    DriveEntry* old_entry =
        resource_metadata_->GetEntryByResourceId(entry->resource_id());
    DriveDirectory* dest_dir = NULL;
    if (entry->is_deleted()) {  // Deleted file/directory.
      DVLOG(1) << "Removing file " << entry->base_name();
      if (!old_entry)
        continue;

      dest_dir = old_entry->parent();
      if (!dest_dir) {
        NOTREACHED();
        continue;
      }
      RemoveEntryFromDirectoryAndCollectChangedDirectories(
          dest_dir, old_entry, changed_dirs);
    } else if (old_entry) {  // Change or move of existing entry.
      // Please note that entry rename is just a special case of change here
      // since name is just one of the properties that can change.
      DVLOG(1) << "Changed file " << entry->base_name();
      dest_dir = old_entry->parent();
      if (!dest_dir) {
        NOTREACHED();
        continue;
      }
      // Move children files over if we are dealing with directories.
      if (old_entry->AsDriveDirectory() && entry->AsDriveDirectory()) {
        entry->AsDriveDirectory()->TakeOverEntries(
            old_entry->AsDriveDirectory());
      }
      // Remove the old instance of this entry.
      RemoveEntryFromDirectoryAndCollectChangedDirectories(
          dest_dir, old_entry, changed_dirs);
      // Did we actually move the new file to another directory?
      if (dest_dir->resource_id() != entry->parent_resource_id()) {
        changed_dirs->insert(dest_dir->GetFilePath());
        dest_dir = FindDirectoryForNewEntry(entry.get(),
                                            *file_map,
                                            orphaned_resources.get());
      }
      DCHECK(dest_dir);
      AddEntryToDirectoryAndCollectChangedDirectories(
          entry.release(),
          dest_dir,
          orphaned_resources.get(),
          changed_dirs);
    } else {  // Adding a new file.
      dest_dir = FindDirectoryForNewEntry(entry.get(),
                                          *file_map,
                                          orphaned_resources.get());
      DCHECK(dest_dir);
      AddEntryToDirectoryAndCollectChangedDirectories(
          entry.release(),
          dest_dir,
          orphaned_resources.get(),
          changed_dirs);
    }

    // Record changed directory if this was a delta feed and the parent
    // directory is already properly rooted within its parent.
    if (dest_dir && (dest_dir->parent() ||
        dest_dir == resource_metadata_->root()) &&
        dest_dir != orphaned_resources->root() && is_delta_feed) {
      changed_dirs->insert(dest_dir->GetFilePath());
    }
  }
  // All entry must be erased from the map.
  DCHECK(file_map->empty());
}

// static
void GDataWapiFeedProcessor::AddEntryToDirectoryAndCollectChangedDirectories(
    DriveEntry* entry,
    DriveDirectory* directory,
    DriveResourceMetadata* orphaned_resources,
    std::set<FilePath>* changed_dirs) {
  directory->AddEntry(entry);
  if (entry->AsDriveDirectory() && directory != orphaned_resources->root())
    changed_dirs->insert(entry->GetFilePath());
}

// static
void GDataWapiFeedProcessor::
RemoveEntryFromDirectoryAndCollectChangedDirectories(
    DriveDirectory* directory,
    DriveEntry* entry,
    std::set<FilePath>* changed_dirs) {
  // Get the list of all sub-directory paths, so we can notify their listeners
  // that they are smoked.
  DriveDirectory* dir = entry->AsDriveDirectory();
  if (dir)
    dir->GetChildDirectoryPaths(changed_dirs);
  directory->RemoveEntry(entry);
}

DriveDirectory* GDataWapiFeedProcessor::FindDirectoryForNewEntry(
    DriveEntry* new_entry,
    const FileResourceIdMap& file_map,
    DriveResourceMetadata* orphaned_resources) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DriveDirectory* dir = NULL;
  // Added file.
  const std::string& parent_id = new_entry->parent_resource_id();
  if (parent_id.empty()) {
    dir = resource_metadata_->root();
    DVLOG(1) << "Root parent for " << new_entry->base_name();
  } else {
    DriveEntry* entry = resource_metadata_->GetEntryByResourceId(parent_id);
    dir = entry ? entry->AsDriveDirectory() : NULL;
    if (!dir) {
      // The parent directory was also added with this set of feeds.
      FileResourceIdMap::const_iterator find_iter =
          file_map.find(parent_id);
      dir = (find_iter != file_map.end() &&
             find_iter->second) ?
                find_iter->second->AsDriveDirectory() : NULL;
      if (dir) {
        DVLOG(1) << "Found parent for " << new_entry->base_name()
                 << " in file_map " << parent_id;
      } else {
        DVLOG(1) << "Adding orphan " << new_entry->GetFilePath().value();
        dir = orphaned_resources->root();
      }
    }
  }
  return dir;
}

DriveFileError GDataWapiFeedProcessor::FeedToFileResourceMap(
    const ScopedVector<DocumentFeed>& feed_list,
    FileResourceIdMap* file_map,
    int64* feed_changestamp,
    FeedToFileResourceMapUmaStats* uma_stats) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(uma_stats);

  DriveFileError error = DRIVE_FILE_OK;
  uma_stats->num_regular_files = 0;
  uma_stats->num_hosted_documents = 0;
  uma_stats->num_files_with_entry_kind.clear();
  uma_stats->num_files_with_file_format.clear();
  for (size_t i = 0; i < feed_list.size(); ++i) {
    const DocumentFeed* feed = feed_list[i];

    // Get upload url from the root feed. Links for all other collections will
    // be handled in GDatadirectory::FromDocumentEntry();
    if (i == 0) {
      const Link* root_feed_upload_link =
          feed->GetLinkByType(Link::LINK_RESUMABLE_CREATE_MEDIA);
      if (root_feed_upload_link)
        resource_metadata_->root()->set_upload_url(
            root_feed_upload_link->href());
      *feed_changestamp = feed->largest_changestamp();
      DCHECK_GE(*feed_changestamp, 0);
    }

    for (ScopedVector<DocumentEntry>::const_iterator iter =
             feed->entries().begin();
         iter != feed->entries().end(); ++iter) {
      DocumentEntry* doc = *iter;
      scoped_ptr<DriveEntry> entry =
          resource_metadata_->FromDocumentEntry(*doc);
      // Some document entries don't map into files (i.e. sites).
      if (!entry.get())
        continue;
      // Count the number of files.
      DriveFile* as_file = entry->AsDriveFile();
      if (as_file) {
        if (as_file->is_hosted_document()) {
          ++uma_stats->num_hosted_documents;
        } else {
          ++uma_stats->num_regular_files;
          const FilePath::StringType extension =
              FilePath(as_file->base_name()).Extension();
          const DriveFileFormat format = GetDriveFileFormat(extension);
          ++uma_stats->num_files_with_file_format[format];
        }
        ++uma_stats->num_files_with_entry_kind[as_file->kind()];
      }

      FileResourceIdMap::iterator map_entry =
          file_map->find(entry->resource_id());

      // An entry with the same self link may already exist, so we need to
      // release the existing DriveEntry instance before overwriting the
      // entry with another DriveEntry instance.
      if (map_entry != file_map->end()) {
        LOG(WARNING) << "Found duplicate file "
                     << map_entry->second->base_name();

        delete map_entry->second;
        file_map->erase(map_entry);
      }
      // Must use this temporary because entry.release() may be evaluated
      // first in make_pair.
      const std::string& resource_id = entry->resource_id();
      file_map->insert(std::make_pair(resource_id, entry.release()));
    }
  }

  if (error != DRIVE_FILE_OK) {
    // If the code above fails to parse a feed, any DriveEntry instance
    // added to |file_by_url| is not managed by a DriveDirectory instance,
    // so we need to explicitly release them here.
    STLDeleteValues(file_map);
  }

  return error;
}

}  // namespace gdata
