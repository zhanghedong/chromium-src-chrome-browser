// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

var mainWindow = null;
chrome.app.runtime.onLaunched.addListener(function() {
  chrome.app.window.create('main.html', {
    id: 'apps_devtool',
    minHeight: 600,
    minWidth: 800,
    height: 600,
    width: 800,
    singleton: true
  });
});
