// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_WEBUI_NTP_CORE_APP_LAUNCHER_HANDLER_H_
#define CHROME_BROWSER_UI_WEBUI_NTP_CORE_APP_LAUNCHER_HANDLER_H_

#include <string>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/values.h"
#include "chrome/common/extensions/extension.h"
#include "chrome/common/extensions/extension_constants.h"
#include "content/public/browser/web_ui_message_handler.h"

class Profile;

class CoreAppLauncherHandler : public content::WebUIMessageHandler {
 public:
  CoreAppLauncherHandler();
  virtual ~CoreAppLauncherHandler();

  // Records the given type of app launch for UMA.
  static void RecordAppLaunchType(extension_misc::AppLaunchBucket bucket,
                                  extensions::Manifest::Type app_type);

 private:
  // Callback for the "recordAppLaunchByUrl" message. Takes an escaped URL and
  // a launch source(integer), and if the URL represents an app, records the
  // action for UMA.
  void HandleRecordAppLaunchByUrl(const base::ListValue* args);

  // Records an app launch in the corresponding |bucket| of the app launch
  // histogram if the |escaped_url| corresponds to an installed app.
  void RecordAppLaunchByUrl(Profile* profile,
                            std::string escaped_url,
                            extension_misc::AppLaunchBucket bucket);

  // WebUIMessageHandler implementation.
  virtual void RegisterMessages() OVERRIDE;

  DISALLOW_COPY_AND_ASSIGN(CoreAppLauncherHandler);
};

#endif  // CHROME_BROWSER_UI_WEBUI_NTP_CORE_APP_LAUNCHER_HANDLER_H_
