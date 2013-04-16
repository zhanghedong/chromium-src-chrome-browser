// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/chromeos/drive/change_list_processor.h"

#include <utility>

#include "base/metrics/histogram.h"
#include "chrome/browser/chromeos/drive/drive.pb.h"
#include "chrome/browser/chromeos/drive/drive_file_system_util.h"
#include "chrome/browser/chromeos/drive/drive_resource_metadata.h"
#include "chrome/browser/chromeos/drive/resource_entry_conversion.h"
#include "chrome/browser/google_apis/gdata_wapi_parser.h"
#include "content/public/browser/browser_thread.h"

using content::BrowserThread;

namespace drive {

namespace {

// Callback for DriveResourceMetadata::SetLargestChangestamp.
// Runs |on_complete_callback|. |on_complete_callback| must not be null.
void RunOnCompleteCallback(const base::Closure& on_complete_callback,
                           DriveFileError error) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(!on_complete_callback.is_null());
  DCHECK_EQ(DRIVE_FILE_OK, error);

  on_complete_callback.Run();
}

}  // namespace

ChangeList::ChangeList(const google_apis::ResourceList& resource_list)
    : largest_changestamp_(resource_list.largest_changestamp()) {
  resource_list.GetNextFeedURL(&next_url_);

  entries_.resize(resource_list.entries().size());
  for (size_t i = 0; i < resource_list.entries().size(); ++i) {
    ConvertResourceEntryToDriveEntryProto(*resource_list.entries()[i]).Swap(
        &entries_[i]);
  }
}

ChangeList::~ChangeList() {}

class ChangeListProcessor::ChangeListToEntryProtoMapUMAStats {
 public:
  ChangeListToEntryProtoMapUMAStats()
    : num_regular_files_(0),
      num_hosted_documents_(0),
      num_shared_with_me_entries_(0) {
  }

  // Increments number of files.
  void IncrementNumFiles(bool is_hosted_document) {
    is_hosted_document ? num_hosted_documents_++ : num_regular_files_++;
  }

  // Increments number of shared-with-me entries.
  void IncrementNumSharedWithMeEntries() {
    num_shared_with_me_entries_++;
  }

  // Updates UMA histograms with file counts.
  void UpdateFileCountUmaHistograms() {
    const int num_total_files = num_hosted_documents_ + num_regular_files_;
    UMA_HISTOGRAM_COUNTS("Drive.NumberOfRegularFiles", num_regular_files_);
    UMA_HISTOGRAM_COUNTS("Drive.NumberOfHostedDocuments",
                         num_hosted_documents_);
    UMA_HISTOGRAM_COUNTS("Drive.NumberOfTotalFiles", num_total_files);
    UMA_HISTOGRAM_COUNTS("Drive.NumberOfSharedWithMeEntries",
                         num_shared_with_me_entries_);
  }

 private:
  int num_regular_files_;
  int num_hosted_documents_;
  int num_shared_with_me_entries_;
};

ChangeListProcessor::ChangeListProcessor(
    DriveResourceMetadata* resource_metadata)
  : resource_metadata_(resource_metadata),
    largest_changestamp_(0),
    ALLOW_THIS_IN_INITIALIZER_LIST(weak_ptr_factory_(this)) {
}

ChangeListProcessor::~ChangeListProcessor() {
}

void ChangeListProcessor::ApplyFeeds(
    scoped_ptr<google_apis::AboutResource> about_resource,
    ScopedVector<ChangeList> change_lists,
    bool is_delta_feed,
    const base::Closure& on_complete_callback) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(!on_complete_callback.is_null());
  DCHECK(is_delta_feed || about_resource.get());

  int64 delta_feed_changestamp = 0;
  ChangeListToEntryProtoMapUMAStats uma_stats;
  FeedToEntryProtoMap(change_lists.Pass(), &delta_feed_changestamp, &uma_stats);
  // Note FeedToEntryProtoMap calls Clear() which resets on_complete_callback_.
  on_complete_callback_ = on_complete_callback;
  largest_changestamp_ = 0;
  if (is_delta_feed) {
    largest_changestamp_ = delta_feed_changestamp;
  } else if (about_resource.get()) {
    largest_changestamp_ = about_resource->largest_change_id();

    DVLOG(1) << "Root folder ID is " << about_resource->root_folder_id();
    DCHECK(!about_resource->root_folder_id().empty());
  } else {
    // A full update without AboutResouce will have no effective changestamp.
    NOTREACHED();
  }

  ApplyEntryProtoMap(is_delta_feed, about_resource.Pass());

  // Shouldn't record histograms when processing delta feeds.
  if (!is_delta_feed)
    uma_stats.UpdateFileCountUmaHistograms();
}

void ChangeListProcessor::ApplyEntryProtoMap(
    bool is_delta_feed,
    scoped_ptr<google_apis::AboutResource> about_resource) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  if (!is_delta_feed) {  // Full update.
    DCHECK(about_resource);
    changed_dirs_.insert(util::GetDriveGrandRootPath());
    changed_dirs_.insert(util::GetDriveMyDriveRootPath());
    // After all nodes are cleared, create the MyDrive root directory at first.
    resource_metadata_->RemoveAll(
        base::Bind(
            &ChangeListProcessor::ApplyEntryProto,
            weak_ptr_factory_.GetWeakPtr(),
            util::CreateMyDriveRootEntry(about_resource->root_folder_id())));
  } else {
    // Go through all entries generated by the feed and apply them to the local
    // snapshot of the file system.
    ApplyNextEntryProtoAsync();
  }
}

void ChangeListProcessor::ApplyNextEntryProtoAsync() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  base::MessageLoopProxy::current()->PostTask(
      FROM_HERE,
      base::Bind(&ChangeListProcessor::ApplyNextEntryProto,
                 weak_ptr_factory_.GetWeakPtr()));
}

void ChangeListProcessor::ApplyNextEntryProto() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  if (!entry_proto_map_.empty()) {
    ApplyNextByIterator(entry_proto_map_.begin());  // Continue.
  } else {
    // Update the root entry and finish.
    UpdateRootEntry(base::Bind(&ChangeListProcessor::OnComplete,
                               weak_ptr_factory_.GetWeakPtr()));
  }
}

void ChangeListProcessor::ApplyNextByIterator(DriveEntryProtoMap::iterator it) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  DriveEntryProto entry_proto = it->second;
  DCHECK_EQ(it->first, entry_proto.resource_id());
  // Add the largest changestamp if this entry is a directory.
  if (entry_proto.file_info().is_directory()) {
    entry_proto.mutable_directory_specific_info()->set_changestamp(
        largest_changestamp_);
  }

  // The parent of this entry may not yet be processed. We need the parent
  // to be rooted in the metadata tree before we can add the child, so process
  // the parent first.
  DriveEntryProtoMap::iterator parent_it = entry_proto_map_.find(
      entry_proto.parent_resource_id());
  if (parent_it != entry_proto_map_.end()) {
    base::MessageLoopProxy::current()->PostTask(
        FROM_HERE,
        base::Bind(&ChangeListProcessor::ApplyNextByIterator,
                   weak_ptr_factory_.GetWeakPtr(),
                   parent_it));
  } else {
    // Erase the entry so the deleted entry won't be referenced.
    entry_proto_map_.erase(it);
    ApplyEntryProto(entry_proto);
  }
}

void ChangeListProcessor::ApplyEntryProto(const DriveEntryProto& entry_proto) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  // Lookup the entry.
  resource_metadata_->GetEntryInfoByResourceId(
      entry_proto.resource_id(),
      base::Bind(&ChangeListProcessor::ContinueApplyEntryProto,
                 weak_ptr_factory_.GetWeakPtr(),
                 entry_proto));
}

void ChangeListProcessor::ContinueApplyEntryProto(
    const DriveEntryProto& entry_proto,
    DriveFileError error,
    const base::FilePath& file_path,
    scoped_ptr<DriveEntryProto> old_entry_proto) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  if (error == DRIVE_FILE_OK) {
    if (entry_proto.deleted()) {
      // Deleted file/directory.
      RemoveEntryFromParent(entry_proto, file_path);
    } else {
      // Entry exists and needs to be refreshed.
      RefreshEntry(entry_proto, file_path);
    }
  } else if (error == DRIVE_FILE_ERROR_NOT_FOUND && !entry_proto.deleted()) {
    // Adding a new entry.
    AddEntry(entry_proto);
  } else {
    // Continue.
    ApplyNextEntryProtoAsync();
  }
}

void ChangeListProcessor::AddEntry(const DriveEntryProto& entry_proto) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  resource_metadata_->AddEntry(
      entry_proto,
      base::Bind(&ChangeListProcessor::NotifyForAddEntry,
                 weak_ptr_factory_.GetWeakPtr(),
                 entry_proto.file_info().is_directory()));
}

void ChangeListProcessor::NotifyForAddEntry(bool is_directory,
                                            DriveFileError error,
                                            const base::FilePath& file_path) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  DVLOG(1) << "NotifyForAddEntry " << file_path.value() << ", error = "
           << DriveFileErrorToString(error);
  if (error == DRIVE_FILE_OK) {
    // Notify if a directory has been created.
    if (is_directory)
      changed_dirs_.insert(file_path);

    // Notify parent.
    changed_dirs_.insert(file_path.DirName());
  }

  ApplyNextEntryProtoAsync();
}

void ChangeListProcessor::RemoveEntryFromParent(
    const DriveEntryProto& entry_proto,
    const base::FilePath& file_path) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(!file_path.empty());

  if (!entry_proto.file_info().is_directory()) {
    // No children if entry is a file.
    OnGetChildrenForRemove(entry_proto, file_path, std::set<base::FilePath>());
  } else {
    // If entry is a directory, notify its children.
    resource_metadata_->GetChildDirectories(
        entry_proto.resource_id(),
        base::Bind(&ChangeListProcessor::OnGetChildrenForRemove,
                   weak_ptr_factory_.GetWeakPtr(),
                   entry_proto,
                   file_path));
  }
}

void ChangeListProcessor::OnGetChildrenForRemove(
    const DriveEntryProto& entry_proto,
    const base::FilePath& file_path,
    const std::set<base::FilePath>& child_directories) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(!file_path.empty());

  resource_metadata_->RemoveEntry(
      entry_proto.resource_id(),
      base::Bind(&ChangeListProcessor::NotifyForRemoveEntryFromParent,
                 weak_ptr_factory_.GetWeakPtr(),
                 entry_proto.file_info().is_directory(),
                 file_path,
                 child_directories));
}

void ChangeListProcessor::NotifyForRemoveEntryFromParent(
    bool is_directory,
    const base::FilePath& file_path,
    const std::set<base::FilePath>& child_directories,
    DriveFileError error,
    const base::FilePath& parent_path) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  DVLOG(1) << "NotifyForRemoveEntryFromParent " << file_path.value();
  if (error == DRIVE_FILE_OK) {
    // Notify parent.
    changed_dirs_.insert(parent_path);

    // Notify children, if any.
    changed_dirs_.insert(child_directories.begin(),
                         child_directories.end());

    // If entry is a directory, notify self.
    if (is_directory)
      changed_dirs_.insert(file_path);
  }

  // Continue.
  ApplyNextEntryProtoAsync();
}

void ChangeListProcessor::RefreshEntry(const DriveEntryProto& entry_proto,
                                      const base::FilePath& file_path) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  resource_metadata_->RefreshEntry(
      entry_proto,
      base::Bind(&ChangeListProcessor::NotifyForRefreshEntry,
                 weak_ptr_factory_.GetWeakPtr(),
                 file_path));
}

void ChangeListProcessor::NotifyForRefreshEntry(
    const base::FilePath& old_file_path,
    DriveFileError error,
    const base::FilePath& file_path,
    scoped_ptr<DriveEntryProto> entry_proto) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  DVLOG(1) << "NotifyForRefreshEntry " << file_path.value();
  if (error == DRIVE_FILE_OK) {
    // Notify old parent.
    changed_dirs_.insert(old_file_path.DirName());

    // Notify new parent.
    changed_dirs_.insert(file_path.DirName());

    // Notify self if entry is a directory.
    if (entry_proto->file_info().is_directory()) {
      // Notify new self.
      changed_dirs_.insert(file_path);
      // Notify old self.
      changed_dirs_.insert(old_file_path);
    }
  }

  ApplyNextEntryProtoAsync();
}

void ChangeListProcessor::FeedToEntryProtoMap(
    ScopedVector<ChangeList> change_lists,
    int64* feed_changestamp,
    ChangeListToEntryProtoMapUMAStats* uma_stats) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  Clear();

  for (size_t i = 0; i < change_lists.size(); ++i) {
    ChangeList* change_list = change_lists[i];

    // Get upload url from the root feed. Links for all other collections will
    // be handled in ConvertResourceEntryToDriveEntryProto.
    if (i == 0) {
      // The changestamp appears in the first page of the change list.
      // The changestamp does not appear in the full resource list.
      if (feed_changestamp)
        *feed_changestamp = change_list->largest_changestamp();
      DCHECK_GE(change_list->largest_changestamp(), 0);
    }

    std::vector<DriveEntryProto>* entries = change_list->mutable_entries();
    for (size_t i = 0; i < entries->size(); ++i) {
      DriveEntryProto* entry_proto = &(*entries)[i];
      // Some document entries don't map into files (i.e. sites).
      if (entry_proto->resource_id().empty())
        continue;

      // Count the number of files.
      if (uma_stats) {
        if (!entry_proto->file_info().is_directory()) {
          uma_stats->IncrementNumFiles(
              entry_proto->file_specific_info().is_hosted_document());
        }
        if (entry_proto->shared_with_me())
          uma_stats->IncrementNumSharedWithMeEntries();
      }

      std::pair<DriveEntryProtoMap::iterator, bool> ret = entry_proto_map_.
          insert(std::make_pair(entry_proto->resource_id(), DriveEntryProto()));
      if (ret.second)
        ret.first->second.Swap(entry_proto);
      else
        LOG(DFATAL) << "Found duplicate file " << entry_proto->base_name();
    }
  }
}

void ChangeListProcessor::UpdateRootEntry(const base::Closure& closure) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(!closure.is_null());

  resource_metadata_->GetEntryInfoByPath(
      util::GetDriveMyDriveRootPath(),
      base::Bind(&ChangeListProcessor::UpdateRootEntryAfterGetEntry,
                 weak_ptr_factory_.GetWeakPtr(),
                 closure));
}

void ChangeListProcessor::UpdateRootEntryAfterGetEntry(
    const base::Closure& closure,
    DriveFileError error,
    scoped_ptr<DriveEntryProto> root_proto) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(!closure.is_null());

  if (error != DRIVE_FILE_OK) {
    // TODO(satorux): Need to trigger recovery if root is corrupt.
    LOG(WARNING) << "Failed to get the proto for root directory";
    closure.Run();
    return;
  }
  DCHECK(root_proto.get());

  // The changestamp should always be updated.
  root_proto->mutable_directory_specific_info()->set_changestamp(
      largest_changestamp_);

  resource_metadata_->RefreshEntry(
      *root_proto,
      base::Bind(&ChangeListProcessor::UpdateRootEntryAfterRefreshEntry,
                 weak_ptr_factory_.GetWeakPtr(),
                 closure));
}

void ChangeListProcessor::UpdateRootEntryAfterRefreshEntry(
    const base::Closure& closure,
    DriveFileError error,
    const base::FilePath& /* root_path */,
    scoped_ptr<DriveEntryProto> /* root_proto */) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(!closure.is_null());
  LOG_IF(WARNING, error != DRIVE_FILE_OK) << "Failed to refresh root directory";

  closure.Run();
}

void ChangeListProcessor::OnComplete() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  resource_metadata_->SetLargestChangestamp(
      largest_changestamp_,
      base::Bind(&RunOnCompleteCallback, on_complete_callback_));
}

void ChangeListProcessor::Clear() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  entry_proto_map_.clear();
  changed_dirs_.clear();
  largest_changestamp_ = 0;
  on_complete_callback_.Reset();
}

}  // namespace drive
