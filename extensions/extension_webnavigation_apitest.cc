// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/command_line.h"
#include "chrome/browser/extensions/extension_apitest.h"
#include "chrome/browser/extensions/extension_webnavigation_api.h"
#include "chrome/common/chrome_switches.h"


IN_PROC_BROWSER_TEST_F(ExtensionApiTest, WebNavigation) {
  CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kEnableExperimentalExtensionApis);

  ExtensionWebNavigationEventRouter::GetInstance()->EnableExtensionScheme();

  ASSERT_TRUE(RunExtensionSubtest("webnavigation",
                                  "api/test.html")) << message_;
}

IN_PROC_BROWSER_TEST_F(ExtensionApiTest, WebNavigationClientRedirect) {
  CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kEnableExperimentalExtensionApis);

  ExtensionWebNavigationEventRouter::GetInstance()->EnableExtensionScheme();

  ASSERT_TRUE(RunExtensionSubtest("webnavigation",
                                  "clientRedirect/test.html")) << message_;
}

IN_PROC_BROWSER_TEST_F(ExtensionApiTest, WebNavigationForwardBack) {
  CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kEnableExperimentalExtensionApis);

  ExtensionWebNavigationEventRouter::GetInstance()->EnableExtensionScheme();

  ASSERT_TRUE(RunExtensionSubtest("webnavigation",
                                  "forwardBack/test.html")) << message_;
}

IN_PROC_BROWSER_TEST_F(ExtensionApiTest, WebNavigationIFrame) {
  CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kEnableExperimentalExtensionApis);

  ExtensionWebNavigationEventRouter::GetInstance()->EnableExtensionScheme();

  ASSERT_TRUE(RunExtensionSubtest("webnavigation",
                                  "iframe/test.html")) << message_;
}

IN_PROC_BROWSER_TEST_F(ExtensionApiTest, WebNavigationOpenTab) {
  CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kEnableExperimentalExtensionApis);

  ExtensionWebNavigationEventRouter::GetInstance()->EnableExtensionScheme();

  ASSERT_TRUE(RunExtensionSubtest("webnavigation",
                                  "openTab/test.html")) << message_;
}

IN_PROC_BROWSER_TEST_F(ExtensionApiTest, WebNavigationReferenceFragment) {
  CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kEnableExperimentalExtensionApis);

  ExtensionWebNavigationEventRouter::GetInstance()->EnableExtensionScheme();

  ASSERT_TRUE(RunExtensionSubtest("webnavigation",
                                  "referenceFragment/test.html")) << message_;
}

IN_PROC_BROWSER_TEST_F(ExtensionApiTest, WebNavigationSimpleLoad) {
  CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kEnableExperimentalExtensionApis);

  ExtensionWebNavigationEventRouter::GetInstance()->EnableExtensionScheme();

  ASSERT_TRUE(RunExtensionSubtest("webnavigation",
                                  "simpleLoad/test.html")) << message_;
}

IN_PROC_BROWSER_TEST_F(ExtensionApiTest, WebNavigationFailures) {
  CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kEnableExperimentalExtensionApis);

  ExtensionWebNavigationEventRouter::GetInstance()->EnableExtensionScheme();

  ASSERT_TRUE(RunExtensionSubtest("webnavigation",
                                  "failures/test.html")) << message_;
}
