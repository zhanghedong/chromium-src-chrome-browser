// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CHROMEOS_DRIVE_JOB_LIST_INTERFACE_H_
#define CHROME_BROWSER_CHROMEOS_DRIVE_JOB_LIST_INTERFACE_H_

#include "base/basictypes.h"
#include "base/files/file_path.h"

namespace drive {

// Enum representing the type of job.
enum JobType {
  TYPE_GET_ABOUT_RESOURCE,
  TYPE_GET_ACCOUNT_METADATA,
  TYPE_GET_APP_LIST,
  TYPE_GET_ALL_RESOURCE_LIST,
  TYPE_GET_RESOURCE_LIST_IN_DIRECTORY,
  TYPE_SEARCH,
  TYPE_GET_CHANGE_LIST,
  TYPE_CONTINUE_GET_RESOURCE_LIST,
  TYPE_GET_RESOURCE_ENTRY,
  TYPE_DELETE_RESOURCE,
  TYPE_COPY_HOSTED_DOCUMENT,
  TYPE_RENAME_RESOURCE,
  TYPE_ADD_RESOURCE_TO_DIRECTORY,
  TYPE_REMOVE_RESOURCE_FROM_DIRECTORY,
  TYPE_ADD_NEW_DIRECTORY,
  TYPE_DOWNLOAD_FILE,
  TYPE_UPLOAD_NEW_FILE,
  TYPE_UPLOAD_EXISTING_FILE,
};

// Current state of the job.
enum JobState {
  // The job is queued, but not yet executed.
  STATE_NONE,

  // The job is in the process of being handled.
  STATE_RUNNING,

  // The job failed, but has been re-added to the queue.
  STATE_RETRY,
};

// Unique ID assigned to each job.
typedef int32 JobID;

// Information about a specific job that is visible to other systems.
struct JobInfo {
  explicit JobInfo(JobType in_job_type)
      : job_type(in_job_type),
        job_id(-1),
        state(STATE_NONE),
        num_completed_bytes(0),
        num_total_bytes(0) {
  }

  // Type of the job.
  JobType job_type;

  // Id of the job, which can be used to query or modify it.
  JobID job_id;

  // Current state of the operation.
  JobState state;

  // The fields below are available only for jobs with job_type:
  // TYPE_DOWNLOAD_FILE, TYPE_UPLOAD_NEW_FILE, or TYPE_UPLOAD_EXISTING_FILE.

  // Number of bytes completed.
  int64 num_completed_bytes;

  // Total bytes of this operation.
  int64 num_total_bytes;

  // Drive path of the file that this job acts on.
  base::FilePath file_path;
};

// The interface for observing JobListInterface.
// All events are notified in the UI thread.
class JobListObserver {
 public:
  // Called when a new job id added.
  virtual void OnJobAdded(const JobInfo& job_info) {}

  // Called when a job id finished.
  virtual void OnJobDone(const JobInfo& job_info) {}

  // Called when a job status is updated.
  virtual void OnJobUpdated(const JobInfo& job_info) {}

 protected:
  virtual ~JobListObserver() {}
};

// The interface to expose the list of issued Drive jobs.
class JobListInterface {
 public:
  virtual ~JobListInterface() {}

  // Returns the list of jobs currently managed by the scheduler.
  virtual std::vector<JobInfo> GetJobInfoList() = 0;

  // Adds an observer.
  virtual void AddObserver(JobListObserver* observer) = 0;

  // Removes an observer.
  virtual void RemoveObserver(JobListObserver* observer) = 0;
};

}  // namespace drive

#endif  // CHROME_BROWSER_CHROMEOS_DRIVE_JOB_LIST_INTERFACE_H_