// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_THEME_INSTALL_BUBBLE_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_THEME_INSTALL_BUBBLE_VIEW_H_
#pragma once

#include "base/string16.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"
#include "ui/gfx/canvas.h"
#include "views/controls/label.h"

class TabContents;

namespace views {
class Widget;
}

// ThemeInstallBubbleView is a view that provides a "Loading..." bubble in the
// center of a browser window for use when an extension or theme is loaded.
// (The Browser class only calls it to install itself into the currently active
// browser window.)  If an extension is being applied, the bubble goes away
// immediately.  If a theme is being applied, it disappears when the theme has
// been loaded.  The purpose of this bubble is to warn the user that the browser
// may be unresponsive while the theme is being installed.
//
// Edge case: note that if one installs a theme in one window and then switches
// rapidly to another window to install a theme there as well (in the short time
// between install begin and theme caching seizing the UI thread), the loading
// bubble will only appear over the first window, as there is only ever one
// instance of the bubble.
class ThemeInstallBubbleView : public content::NotificationObserver,
                               public views::Label {
 public:
  virtual ~ThemeInstallBubbleView();

  // content::NotificationObserver
  virtual void Observe(int type,
                       const content::NotificationSource& source,
                       const content::NotificationDetails& details);

  // Show the loading bubble.
  static void Show(TabContents* tab_contents);

 private:
  explicit ThemeInstallBubbleView(TabContents* tab_contents);

  // Put the popup in the correct place on the tab.
  void Reposition();

  // Inherited from views.
  virtual gfx::Size GetPreferredSize();

  // Shut down the popup and remove our notifications.
  void Close();

  virtual void OnPaint(gfx::Canvas* canvas);

  // The content area at the start of the animation.
  gfx::Rect tab_contents_bounds_;

  // Widget containing us.
  views::Widget* popup_;

  // Text to show warning that theme is being installed.
  string16 text_;

  // A scoped container for notification registries.
  content::NotificationRegistrar registrar_;

  DISALLOW_COPY_AND_ASSIGN(ThemeInstallBubbleView);
};

#endif  // CHROME_BROWSER_UI_VIEWS_THEME_INSTALL_BUBBLE_VIEW_H_
