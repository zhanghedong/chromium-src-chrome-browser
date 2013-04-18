// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/chromeos/drive/drive_file_stream_reader.h"

#include <string>

#include "base/file_util.h"
#include "base/files/file_path.h"
#include "base/message_loop.h"
#include "chrome/browser/google_apis/test_util.h"
#include "content/public/test/test_browser_thread.h"
#include "net/base/file_stream.h"
#include "net/base/io_buffer.h"
#include "net/base/net_errors.h"
#include "net/base/test_completion_callback.h"
#include "testing/gtest/include/gtest/gtest.h"

using content::BrowserThread;

namespace drive {
namespace internal {

TEST(LocalReaderProxyTest, Read) {
  // Prepare the test content.
  const base::FilePath kTestFile(
      google_apis::test_util::GetTestFilePath("chromeos/drive/applist.json"));
  std::string expected_content;
  ASSERT_TRUE(file_util::ReadFileToString(kTestFile, &expected_content));

  // The LocaReaderProxy should live on IO thread.
  MessageLoopForIO io_loop;
  content::TestBrowserThread io_thread(BrowserThread::IO, &io_loop);

  // Open the file first.
  scoped_ptr<net::FileStream> file_stream(new net::FileStream(NULL));
  net::TestCompletionCallback callback;
  int result = file_stream->Open(
      google_apis::test_util::GetTestFilePath("chromeos/drive/applist.json"),
      base::PLATFORM_FILE_OPEN | base::PLATFORM_FILE_READ |
      base::PLATFORM_FILE_ASYNC,
      callback.callback());
  ASSERT_EQ(net::OK, callback.GetResult(result));

  // Test instance.
  LocalReaderProxy proxy(file_stream.Pass());

  // Prepare the buffer, whose size is smaller than the whole data size.
  const int kBufferSize = 10;
  ASSERT_LE(static_cast<size_t>(kBufferSize), expected_content.size());
  scoped_refptr<net::IOBuffer> buffer(new net::IOBuffer(kBufferSize));

  // Read repeatedly, until it is finished.
  std::string concatenated_content;
  while (concatenated_content.size() < expected_content.size()) {
    result = proxy.Read(buffer.get(), kBufferSize, callback.callback());
    result = callback.GetResult(result);

    // The read content size should be smaller than the buffer size.
    ASSERT_GT(result, 0);
    ASSERT_LE(result, kBufferSize);
    concatenated_content.append(buffer->data(), result);
  }

  // Make sure the read contant is as same as the file.
  EXPECT_EQ(expected_content, concatenated_content);
}

}  // namespace internal
}  // namespace drive