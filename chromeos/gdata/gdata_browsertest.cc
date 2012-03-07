// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/bind.h"
#include "base/command_line.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/json/json_reader.h"
#include "chrome/browser/chromeos/gdata/gdata.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/ui_test_utils.h"
#include "net/test/test_server.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

class GDataTest : public InProcessBrowserTest {
 public:
  GDataTest()
      : InProcessBrowserTest(),
        gdata_test_server_(net::TestServer::TYPE_GDATA,
                           net::TestServer::kLocalhost,
                           FilePath(FILE_PATH_LITERAL("chrome/test/data"))) {
  }

  virtual void SetUpOnMainThread() OVERRIDE {
    ASSERT_TRUE(gdata_test_server_.Start());
    service_.Initialize(browser()->profile());
    service_.gdata_auth_service()->set_oauth2_auth_token_for_testing(
        net::TestServer::kGDataAuthToken);
  }

 protected:
  net::TestServer gdata_test_server_;
  gdata::DocumentsService service_;
};

// The test callback for DocumentsService::DownloadFile().
void TestDownloadCallback(gdata::GDataErrorCode* result,
                          std::string* contents,
                          gdata::GDataErrorCode error,
                          const GURL& content_url,
                          const FilePath& temp_file) {
  *result = error;
  file_util::ReadFileToString(temp_file, contents);
  file_util::Delete(temp_file, false);
  MessageLoop::current()->Quit();
}

// The test callback for DocumentsService::GetDocuments().
void TestGetDocumentsCallback(gdata::GDataErrorCode* result_code,
                              base::Value** result_data,
                              gdata::GDataErrorCode error,
                              scoped_ptr<base::Value> feed_data) {
  *result_code = error;
  *result_data = feed_data.release();
  MessageLoop::current()->Quit();
}

}  // namespace

IN_PROC_BROWSER_TEST_F(GDataTest, DISABLED_Download) {
  gdata::GDataErrorCode result = gdata::GDATA_OTHER_ERROR;
  std::string contents;
  service_.DownloadFile(
      gdata_test_server_.GetURL("files/chromeos/gdata/testfile.txt"),
      base::Bind(&TestDownloadCallback, &result, &contents));
  ui_test_utils::RunMessageLoop();

  EXPECT_EQ(gdata::HTTP_SUCCESS, result);
  FilePath expected_filepath = gdata_test_server_.document_root().Append(
      FilePath(FILE_PATH_LITERAL("chromeos/gdata/testfile.txt")));
  std::string expected_contents;
  file_util::ReadFileToString(expected_filepath, &expected_contents);
  EXPECT_EQ(expected_contents, contents);
}

IN_PROC_BROWSER_TEST_F(GDataTest, DISABLED_NonExistingDownload) {
  gdata::GDataErrorCode result = gdata::GDATA_OTHER_ERROR;
  std::string dummy_contents;
  service_.DownloadFile(
      gdata_test_server_.GetURL("files/chromeos/gdata/no-such-file.txt"),
      base::Bind(&TestDownloadCallback, &result, &dummy_contents));
  ui_test_utils::RunMessageLoop();

  EXPECT_EQ(gdata::HTTP_NOT_FOUND, result);
  // Do not verify the not found message.
}

IN_PROC_BROWSER_TEST_F(GDataTest, DISABLED_GetDocuments) {
  gdata::GDataErrorCode result = gdata::GDATA_OTHER_ERROR;
  base::Value* result_data = NULL;
  service_.GetDocuments(
      gdata_test_server_.GetURL("files/chromeos/gdata/root_feed.json"),
      base::Bind(&TestGetDocumentsCallback, &result, &result_data));
  ui_test_utils::RunMessageLoop();

  EXPECT_EQ(gdata::HTTP_SUCCESS, result);
  ASSERT_TRUE(result_data);
  FilePath expected_filepath = gdata_test_server_.document_root().Append(
      FilePath(FILE_PATH_LITERAL("chromeos/gdata/root_feed.json")));
  std::string expected_contents;
  file_util::ReadFileToString(expected_filepath, &expected_contents);
  scoped_ptr<base::Value> expected_data(
      base::JSONReader::Read(expected_contents, false));
  EXPECT_TRUE(base::Value::Equals(expected_data.get(), result_data));
  delete result_data;
}

IN_PROC_BROWSER_TEST_F(GDataTest, DISABLED_GetDocumentsFailure) {
  // testfile.txt exists but the response is not JSON, so it should
  // emit a parse error instead.
  gdata::GDataErrorCode result = gdata::GDATA_OTHER_ERROR;
  base::Value* result_data = NULL;
  service_.GetDocuments(
      gdata_test_server_.GetURL("files/chromeos/gdata/testfile.txt"),
      base::Bind(&TestGetDocumentsCallback, &result, &result_data));
  ui_test_utils::RunMessageLoop();

  EXPECT_EQ(gdata::GDATA_PARSE_ERROR, result);
  EXPECT_FALSE(result_data);
}
