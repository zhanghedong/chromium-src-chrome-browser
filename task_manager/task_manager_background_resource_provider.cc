// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/task_manager/task_manager_background_resource_provider.h"

#include <string>

#include "base/basictypes.h"
#include "base/i18n/rtl.h"
#include "base/string16.h"
#include "base/utf_string_conversions.h"
#include "chrome/browser/background/background_contents_service.h"
#include "chrome/browser/background/background_contents_service_factory.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/extensions/extension_service.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/browser/tab_contents/background_contents.h"
#include "chrome/common/chrome_notification_types.h"
#include "chrome/common/extensions/extension.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "grit/generated_resources.h"
#include "grit/theme_resources.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"

using content::RenderProcessHost;
using content::RenderViewHost;
using content::WebContents;
using extensions::Extension;

////////////////////////////////////////////////////////////////////////////////
// TaskManagerBackgroundContentsResource class
////////////////////////////////////////////////////////////////////////////////

gfx::ImageSkia* TaskManagerBackgroundContentsResource::default_icon_ = NULL;

// TODO(atwilson): http://crbug.com/116893
// HACK: if the process handle is invalid, we use the current process's handle.
// This preserves old behavior but is incorrect, and should be fixed.
TaskManagerBackgroundContentsResource::TaskManagerBackgroundContentsResource(
    BackgroundContents* background_contents,
    const string16& application_name)
    : TaskManagerRendererResource(
          background_contents->web_contents()->GetRenderProcessHost()->
              GetHandle() ?
              background_contents->web_contents()->GetRenderProcessHost()->
                  GetHandle() :
              base::Process::Current().handle(),
          background_contents->web_contents()->GetRenderViewHost()),
      background_contents_(background_contents),
      application_name_(application_name) {
  // Just use the same icon that other extension resources do.
  // TODO(atwilson): Use the favicon when that's available.
  if (!default_icon_) {
    ResourceBundle& rb = ResourceBundle::GetSharedInstance();
    default_icon_ = rb.GetImageSkiaNamed(IDR_PLUGINS_FAVICON);
  }
  // Ensure that the string has the appropriate direction markers (see comment
  // in TaskManagerTabContentsResource::GetTitle()).
  base::i18n::AdjustStringForLocaleDirection(&application_name_);
}

TaskManagerBackgroundContentsResource::~TaskManagerBackgroundContentsResource(
    ) {
}

string16 TaskManagerBackgroundContentsResource::GetTitle() const {
  string16 title = application_name_;

  if (title.empty()) {
    // No title (can't locate the parent app for some reason) so just display
    // the URL (properly forced to be LTR).
    title = base::i18n::GetDisplayStringInLTRDirectionality(
        UTF8ToUTF16(background_contents_->GetURL().spec()));
  }
  return l10n_util::GetStringFUTF16(IDS_TASK_MANAGER_BACKGROUND_PREFIX, title);
}

string16 TaskManagerBackgroundContentsResource::GetProfileName() const {
  return string16();
}

gfx::ImageSkia TaskManagerBackgroundContentsResource::GetIcon() const {
  return *default_icon_;
}

bool TaskManagerBackgroundContentsResource::IsBackground() const {
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// TaskManagerBackgroundContentsResourceProvider class
////////////////////////////////////////////////////////////////////////////////

TaskManagerBackgroundContentsResourceProvider::
    TaskManagerBackgroundContentsResourceProvider(TaskManager* task_manager)
    : updating_(false),
      task_manager_(task_manager) {
}

TaskManagerBackgroundContentsResourceProvider::
    ~TaskManagerBackgroundContentsResourceProvider() {
}

TaskManager::Resource*
TaskManagerBackgroundContentsResourceProvider::GetResource(
    int origin_pid,
    int render_process_host_id,
    int routing_id) {
  // If an origin PID was specified, the request is from a plugin, not the
  // render view host process
  if (origin_pid)
    return NULL;

  for (Resources::iterator i = resources_.begin(); i != resources_.end(); i++) {
    WebContents* tab = i->first->web_contents();
    if (tab->GetRenderProcessHost()->GetID() == render_process_host_id
        && tab->GetRenderViewHost()->GetRoutingID() == routing_id) {
      return i->second;
    }
  }

  // Can happen if the page went away while a network request was being
  // performed.
  return NULL;
}

void TaskManagerBackgroundContentsResourceProvider::StartUpdating() {
  DCHECK(!updating_);
  updating_ = true;

  // Add all the existing BackgroundContents from every profile, including
  // incognito profiles.
  ProfileManager* profile_manager = g_browser_process->profile_manager();
  std::vector<Profile*> profiles(profile_manager->GetLoadedProfiles());
  size_t num_default_profiles = profiles.size();
  for (size_t i = 0; i < num_default_profiles; ++i) {
    if (profiles[i]->HasOffTheRecordProfile()) {
      profiles.push_back(profiles[i]->GetOffTheRecordProfile());
    }
  }
  for (size_t i = 0; i < profiles.size(); ++i) {
    BackgroundContentsService* background_contents_service =
        BackgroundContentsServiceFactory::GetForProfile(profiles[i]);
    std::vector<BackgroundContents*> contents =
        background_contents_service->GetBackgroundContents();
    ExtensionService* extension_service = profiles[i]->GetExtensionService();
    for (std::vector<BackgroundContents*>::iterator iterator = contents.begin();
         iterator != contents.end(); ++iterator) {
      string16 application_name;
      // Lookup the name from the parent extension.
      if (extension_service) {
        const string16& application_id =
            background_contents_service->GetParentApplicationId(*iterator);
        const Extension* extension = extension_service->GetExtensionById(
            UTF16ToUTF8(application_id), false);
        if (extension)
          application_name = UTF8ToUTF16(extension->name());
      }
      Add(*iterator, application_name);
    }
  }

  // Then we register for notifications to get new BackgroundContents.
  registrar_.Add(this, chrome::NOTIFICATION_BACKGROUND_CONTENTS_OPENED,
                 content::NotificationService::AllBrowserContextsAndSources());
  registrar_.Add(this, chrome::NOTIFICATION_BACKGROUND_CONTENTS_NAVIGATED,
                 content::NotificationService::AllBrowserContextsAndSources());
  registrar_.Add(this, chrome::NOTIFICATION_BACKGROUND_CONTENTS_DELETED,
                 content::NotificationService::AllBrowserContextsAndSources());
}

void TaskManagerBackgroundContentsResourceProvider::StopUpdating() {
  DCHECK(updating_);
  updating_ = false;

  // Unregister for notifications
  registrar_.Remove(
      this, chrome::NOTIFICATION_BACKGROUND_CONTENTS_OPENED,
      content::NotificationService::AllBrowserContextsAndSources());
  registrar_.Remove(
      this, chrome::NOTIFICATION_BACKGROUND_CONTENTS_NAVIGATED,
      content::NotificationService::AllBrowserContextsAndSources());
  registrar_.Remove(
      this, chrome::NOTIFICATION_BACKGROUND_CONTENTS_DELETED,
      content::NotificationService::AllBrowserContextsAndSources());

  // Delete all the resources.
  STLDeleteContainerPairSecondPointers(resources_.begin(), resources_.end());

  resources_.clear();
}

void TaskManagerBackgroundContentsResourceProvider::AddToTaskManager(
    BackgroundContents* background_contents,
    const string16& application_name) {
  TaskManagerBackgroundContentsResource* resource =
      new TaskManagerBackgroundContentsResource(background_contents,
                                                application_name);
  resources_[background_contents] = resource;
  task_manager_->AddResource(resource);
}

void TaskManagerBackgroundContentsResourceProvider::Add(
    BackgroundContents* contents, const string16& application_name) {
  if (!updating_)
    return;

  // TODO(atwilson): http://crbug.com/116893
  // We should check that the process handle is valid here, but it won't
  // be in the case of NOTIFICATION_BACKGROUND_CONTENTS_OPENED.

  // Should never add the same BackgroundContents twice.
  DCHECK(resources_.find(contents) == resources_.end());
  AddToTaskManager(contents, application_name);
}

void TaskManagerBackgroundContentsResourceProvider::Remove(
    BackgroundContents* contents) {
  if (!updating_)
    return;
  Resources::iterator iter = resources_.find(contents);
  DCHECK(iter != resources_.end());

  // Remove the resource from the Task Manager.
  TaskManagerBackgroundContentsResource* resource = iter->second;
  task_manager_->RemoveResource(resource);
  // And from the provider.
  resources_.erase(iter);
  // Finally, delete the resource.
  delete resource;
}

void TaskManagerBackgroundContentsResourceProvider::Observe(
    int type,
    const content::NotificationSource& source,
    const content::NotificationDetails& details) {
  switch (type) {
    case chrome::NOTIFICATION_BACKGROUND_CONTENTS_OPENED: {
      // Get the name from the parent application. If no parent application is
      // found, just pass an empty string - BackgroundContentsResource::GetTitle
      // will display the URL instead in this case. This should never happen
      // except in rare cases when an extension is being unloaded or chrome is
      // exiting while the task manager is displayed.
      string16 application_name;
      ExtensionService* service =
          content::Source<Profile>(source)->GetExtensionService();
      if (service) {
        std::string application_id = UTF16ToUTF8(
            content::Details<BackgroundContentsOpenedDetails>(details)->
                application_id);
        const Extension* extension =
            service->GetExtensionById(application_id, false);
        // Extension can be NULL when running unit tests.
        if (extension)
          application_name = UTF8ToUTF16(extension->name());
      }
      Add(content::Details<BackgroundContentsOpenedDetails>(details)->contents,
          application_name);
      // Opening a new BackgroundContents needs to force the display to refresh
      // (applications may now be considered "background" that weren't before).
      task_manager_->ModelChanged();
      break;
    }
    case chrome::NOTIFICATION_BACKGROUND_CONTENTS_NAVIGATED: {
      BackgroundContents* contents =
          content::Details<BackgroundContents>(details).ptr();
      // Should never get a NAVIGATED before OPENED.
      DCHECK(resources_.find(contents) != resources_.end());
      // Preserve the application name.
      string16 application_name(
          resources_.find(contents)->second->application_name());
      Remove(contents);
      Add(contents, application_name);
      break;
    }
    case chrome::NOTIFICATION_BACKGROUND_CONTENTS_DELETED:
      Remove(content::Details<BackgroundContents>(details).ptr());
      // Closing a BackgroundContents needs to force the display to refresh
      // (applications may now be considered "foreground" that weren't before).
      task_manager_->ModelChanged();
      break;
    default:
      NOTREACHED() << "Unexpected notification.";
      return;
  }
}