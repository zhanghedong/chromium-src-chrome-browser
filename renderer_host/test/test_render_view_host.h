// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_RENDERER_HOST_TEST_TEST_RENDER_VIEW_HOST_H_
#define CHROME_BROWSER_RENDERER_HOST_TEST_TEST_RENDER_VIEW_HOST_H_

#include "base/basictypes.h"
#include "base/message_loop.h"
#include "build/build_config.h"
#include "chrome/browser/renderer_host/mock_render_process_host.h"
#include "chrome/browser/renderer_host/render_widget_host_view.h"
#include "chrome/browser/renderer_host/render_view_host.h"
#include "chrome/browser/renderer_host/render_view_host_factory.h"
#include "chrome/browser/renderer_host/site_instance.h"
#include "chrome/browser/tab_contents/navigation_controller.h"
#include "chrome/browser/tab_contents/test_tab_contents.h"
#include "chrome/browser/user_data_manager.h"
#include "chrome/test/testing_profile.h"
#include "testing/gtest/include/gtest/gtest.h"

// This file provides a testing framework for mocking out the RenderProcessHost
// layer. It allows you to test RenderViewHost, TabContents,
// NavigationController, and other layers above that without running an actual
// renderer process.
//
// To use, derive your test base class from RenderViewHostTestHarness.

// TestRenderViewHostView ------------------------------------------------------

// Subclass the RenderViewHost's view so that we can call Show(), etc.,
// without having side-effects.
class TestRenderWidgetHostView : public RenderWidgetHostView {
 public:
  explicit TestRenderWidgetHostView(RenderWidgetHost* rwh);

  virtual void InitAsPopup(RenderWidgetHostView* parent_host_view,
                           const gfx::Rect& pos) {}
  virtual RenderWidgetHost* GetRenderWidgetHost() const { return NULL; }
  virtual void DidBecomeSelected() {}
  virtual void WasHidden() {}
  virtual void SetSize(const gfx::Size& size) {}
  virtual gfx::NativeView GetNativeView() { return NULL; }
  virtual void MovePluginWindows(
      const std::vector<webkit_glue::WebPluginGeometry>& moves) {}
#if defined(OS_WIN)
  virtual void ForwardMouseEventToRenderer(UINT message,
                                           WPARAM wparam,
                                           LPARAM lparam) {}
#endif
  virtual void Focus() {}
  virtual void Blur() {}
  virtual bool HasFocus() { return true; }
  virtual void AdvanceFocus(bool reverse) {}
  virtual void Show() { is_showing_ = true; }
  virtual void Hide() { is_showing_ = false; }
  virtual bool IsShowing() { return is_showing_; }
  virtual gfx::Rect GetViewBounds() const { return gfx::Rect(); }
  virtual void SetIsLoading(bool is_loading) {}
  virtual void UpdateCursor(const WebCursor& cursor) {}
  virtual void UpdateCursorIfOverSelf() {}
  virtual void IMEUpdateStatus(int control, const gfx::Rect& caret_rect) {}
  virtual void DidPaintBackingStoreRects(
      const std::vector<gfx::Rect>& rects) {}
  virtual void DidScrollBackingStoreRect(
      const gfx::Rect& rect, int dx, int dy) {}
  virtual void RenderViewGone() { delete this; }
  virtual void WillDestroyRenderWidget(RenderWidgetHost* rwh) { }
  virtual void Destroy() {}
  virtual void PrepareToDestroy() {}
  virtual void SetTooltipText(const std::wstring& tooltip_text) {}
  virtual BackingStore* AllocBackingStore(const gfx::Size& size);
  virtual VideoLayer* AllocVideoLayer(const gfx::Size& size);
#if defined(OS_MACOSX)
  virtual void ShowPopupWithItems(gfx::Rect bounds,
                                  int item_height,
                                  double item_font_size,
                                  int selected_item,
                                  const std::vector<WebMenuItem>& items,
                                  bool right_aligned) {}
  virtual gfx::Rect GetWindowRect();
  virtual gfx::Rect GetRootWindowRect();
  virtual void SetActive(bool active);
  virtual void SetWindowVisibility(bool visible) {}
  virtual void WindowFrameChanged() {}
  virtual gfx::PluginWindowHandle AllocateFakePluginWindowHandle(
      bool opaque);
  virtual void DestroyFakePluginWindowHandle(gfx::PluginWindowHandle window);
  virtual void AcceleratedSurfaceSetIOSurface(gfx::PluginWindowHandle window,
                                              int32 width,
                                              int32 height,
                                              uint64 io_surface_identifier);
  virtual void AcceleratedSurfaceSetTransportDIB(
      gfx::PluginWindowHandle window,
      int32 width,
      int32 height,
      TransportDIB::Handle transport_dib);
  virtual void AcceleratedSurfaceBuffersSwapped(gfx::PluginWindowHandle window);
  virtual void DrawAcceleratedSurfaceInstances(CGLContextObj context);
#endif
  virtual void SetVisuallyDeemphasized(bool deemphasized) { }

#if defined(OS_LINUX)
  virtual void CreatePluginContainer(gfx::PluginWindowHandle id) { }
  virtual void DestroyPluginContainer(gfx::PluginWindowHandle id) { }
#endif

  virtual bool ContainsNativeView(gfx::NativeView native_view) const {
    return false;
  }

  bool is_showing() const { return is_showing_; }

 private:
  RenderWidgetHost* rwh_;
  bool is_showing_;
};

// TestRenderViewHost ----------------------------------------------------------

// TODO(brettw) this should use a TestTabContents which should be generalized
// from the TabContents test. We will probably also need that class' version of
// CreateRenderViewForRenderManager when more complicate tests start using this.
class TestRenderViewHost : public RenderViewHost {
 public:
  TestRenderViewHost(SiteInstance* instance,
                     RenderViewHostDelegate* delegate,
                     int routing_id);
  virtual ~TestRenderViewHost();

  // Testing functions ---------------------------------------------------------

  // Calls the RenderViewHosts' private OnMessageReceived function with the
  // given message.
  void TestOnMessageReceived(const IPC::Message& msg);

  // Calls OnMsgNavigate on the RenderViewHost with the given information,
  // setting the rest of the parameters in the message to the "typical" values.
  // This is a helper function for simulating the most common types of loads.
  void SendNavigate(int page_id, const GURL& url);

  // Calls OnMsgNavigate on the RenderViewHost with the given information,
  // including a custom PageTransition::Type.  Sets the rest of the parameters
  // in the message to the "typical" values.
  // This is a helper function for simulating the most common types of loads.
  void SendNavigateWithTransition(int page_id, const GURL& url,
                                  PageTransition::Type transition);

  // If set, *delete_counter is incremented when this object destructs.
  void set_delete_counter(int* delete_counter) {
    delete_counter_ = delete_counter;
  }

  // Sets whether the RenderView currently exists or not. This controls the
  // return value from IsRenderViewLive, which the rest of the system uses to
  // check whether the RenderView has crashed or not.
  void set_render_view_created(bool created) {
    render_view_created_ = created;
  }

  // Returns whether the RenderViewHost is currently waiting to hear the result
  // of a before unload handler from the renderer.
  bool is_waiting_for_beforeunload_ack() const {
    return is_waiting_for_beforeunload_ack_;
  }

  // RenderViewHost overrides --------------------------------------------------

  virtual bool CreateRenderView(URLRequestContextGetter* request_context);
  virtual bool IsRenderViewLive() const;

 private:
  FRIEND_TEST(RenderViewHostTest, FilterNavigate);

  // Tracks if the caller thinks if it created the RenderView. This is so we can
  // respond to IsRenderViewLive appropriately.
  bool render_view_created_;

  // See set_delete_counter() above. May be NULL.
  int* delete_counter_;

  DISALLOW_COPY_AND_ASSIGN(TestRenderViewHost);
};

// TestRenderViewHostFactory ---------------------------------------------------

// Manages creation of the RenderViewHosts using our special subclass. This
// automatically registers itself when it goes in scope, and unregisters itself
// when it goes out of scope. Since you can't have more than one factory
// registered at a time, you can only have one of these objects at a time.
class TestRenderViewHostFactory : public RenderViewHostFactory {
 public:
  explicit TestRenderViewHostFactory(RenderProcessHostFactory* rph_factory)
      : render_process_host_factory_(rph_factory) {
    RenderViewHostFactory::RegisterFactory(this);
  }
  virtual ~TestRenderViewHostFactory() {
    RenderViewHostFactory::UnregisterFactory();
  }

  virtual void set_render_process_host_factory(
      RenderProcessHostFactory* rph_factory) {
    render_process_host_factory_ = rph_factory;
  }

  virtual RenderViewHost* CreateRenderViewHost(
      SiteInstance* instance,
      RenderViewHostDelegate* delegate,
      int routing_id,
      int64 session_storage_namespace_id) {
    // See declaration of render_process_host_factory_ below.
    instance->set_render_process_host_factory(render_process_host_factory_);
    return new TestRenderViewHost(instance, delegate, routing_id);
  }

 private:
  // This is a bit of a hack. With the current design of the site instances /
  // browsing instances, it's difficult to pass a RenderProcessHostFactory
  // around properly.
  //
  // Instead, we set it right before we create a new RenderViewHost, which
  // happens before the RenderProcessHost is created. This way, the instance
  // has the correct factory and creates our special RenderProcessHosts.
  RenderProcessHostFactory* render_process_host_factory_;

  DISALLOW_COPY_AND_ASSIGN(TestRenderViewHostFactory);
};

// RenderViewHostTestHarness ---------------------------------------------------

class RenderViewHostTestHarness : public testing::Test {
 public:
  RenderViewHostTestHarness()
      : rph_factory_(),
        rvh_factory_(&rph_factory_),
        contents_(NULL) {}
  virtual ~RenderViewHostTestHarness() {}

  NavigationController& controller() {
    return contents_->controller();
  }

  TestTabContents* contents() {
    return contents_.get();
  }

  TestRenderViewHost* rvh() {
    return static_cast<TestRenderViewHost*>(contents_->render_view_host());
  }

  TestRenderViewHost* pending_rvh() {
    return static_cast<TestRenderViewHost*>(
        contents_->render_manager()->pending_render_view_host());
  }

  TestRenderViewHost* active_rvh() {
    return pending_rvh() ? pending_rvh() : rvh();
  }

  TestingProfile* profile() {
    return profile_.get();
  }

  MockRenderProcessHost* process() {
    return static_cast<MockRenderProcessHost*>(rvh()->process());
  }

  // Frees the current tab contents for tests that want to test destruction.
  void DeleteContents() {
    contents_.reset();
  }

  // Creates a pending navigation to the given URL with the default parameters
  // and then commits the load with a page ID one larger than any seen. This
  // emulates what happens on a new navigation.
  void NavigateAndCommit(const GURL& url);

  // Simulates a reload of the current page.
  void Reload();

 protected:
  // testing::Test
  virtual void SetUp();
  virtual void TearDown();

  // This profile will be created in SetUp if it has not already been created.
  // This allows tests to override the profile if they so choose in their own
  // SetUp function before calling the base class's (us) SetUp().
  scoped_ptr<TestingProfile> profile_;

  MessageLoopForUI message_loop_;

  MockRenderProcessHostFactory rph_factory_;
  TestRenderViewHostFactory rvh_factory_;

  scoped_ptr<TestTabContents> contents_;

  scoped_ptr<UserDataManager> user_data_manager_;

  DISALLOW_COPY_AND_ASSIGN(RenderViewHostTestHarness);
};

#endif  // CHROME_BROWSER_RENDERER_HOST_TEST_TEST_RENDER_VIEW_HOST_H_
