// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/chromeos/login/screen_locker.h"

#include <string>
#include <vector>

#include "ash/desktop_background/desktop_background_controller.h"
#include "ash/shell.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/lazy_instance.h"
#include "base/memory/weak_ptr.h"
#include "base/message_loop.h"
#include "base/metrics/histogram.h"
#include "base/string_util.h"
#include "base/timer.h"
#include "base/utf_string_conversions.h"
#include "chrome/browser/chromeos/login/authenticator.h"
#include "chrome/browser/chromeos/login/login_performer.h"
#include "chrome/browser/chromeos/login/login_utils.h"
#include "chrome/browser/chromeos/login/user_manager.h"
#include "chrome/browser/chromeos/login/webui_screen_locker.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/browser/signin/signin_manager.h"
#include "chrome/browser/signin/signin_manager_factory.h"
#include "chrome/browser/sync/profile_sync_service.h"
#include "chrome/browser/sync/profile_sync_service_factory.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/browser_list.h"
#include "chrome/browser/ui/browser_window.h"
#include "chrome/common/chrome_notification_types.h"
#include "chrome/common/chrome_switches.h"
#include "chromeos/dbus/dbus_thread_manager.h"
#include "chromeos/dbus/power_manager_client.h"
#include "chromeos/dbus/session_manager_client.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/user_metrics.h"
#include "googleurl/src/gurl.h"
#include "grit/generated_resources.h"
#include "third_party/cros_system_api/window_manager/chromeos_wm_ipc_enums.h"
#include "ui/base/l10n/l10n_util.h"

using content::BrowserThread;
using content::UserMetricsAction;

namespace {

// Observer to start ScreenLocker when the screen lock
class ScreenLockObserver : public chromeos::SessionManagerClient::Observer,
                           public content::NotificationObserver {
 public:
  ScreenLockObserver() : session_started_(false) {
    registrar_.Add(this,
                   chrome::NOTIFICATION_LOGIN_USER_CHANGED,
                   content::NotificationService::AllSources());
    registrar_.Add(this,
                   chrome::NOTIFICATION_SESSION_STARTED,
                   content::NotificationService::AllSources());
  }

  // NotificationObserver overrides:
  virtual void Observe(int type,
                       const content::NotificationSource& source,
                       const content::NotificationDetails& details) OVERRIDE {
    switch (type) {
      case chrome::NOTIFICATION_LOGIN_USER_CHANGED: {
        // Register Screen Lock only after a user has logged in.
        chromeos::SessionManagerClient* session_manager =
            chromeos::DBusThreadManager::Get()->GetSessionManagerClient();
        if (!session_manager->HasObserver(this))
          session_manager->AddObserver(this);
        break;
      }

      case chrome::NOTIFICATION_SESSION_STARTED: {
        session_started_ = true;
        break;
      }

      default:
        NOTREACHED();
    }
  }

  virtual void LockScreen() OVERRIDE {
    VLOG(1) << "In: ScreenLockObserver::LockScreen";
    if (session_started_) {
      chromeos::ScreenLocker::Show();
      chromeos::SessionManagerClient* session_manager =
          chromeos::DBusThreadManager::Get()->GetSessionManagerClient();
      session_manager->NotifyLockScreenShown();
    } else {
      // If the user has not completed the sign in we will log them out. This
      // avoids complications with displaying the lock screen over the login
      // screen while remaining secure in the case that they walk away during
      // the signin steps. See crbug.com/112225 and crbug.com/110933.
      chromeos::DBusThreadManager::Get()->
          GetSessionManagerClient()->StopSession();
    }
  }

  virtual void UnlockScreen() OVERRIDE {
    chromeos::ScreenLocker::Hide();
    chromeos::SessionManagerClient* session_manager =
        chromeos::DBusThreadManager::Get()->GetSessionManagerClient();
    session_manager->NotifyLockScreenDismissed();
  }

 private:
  bool session_started_;
  content::NotificationRegistrar registrar_;
  std::string saved_previous_input_method_id_;
  std::string saved_current_input_method_id_;
  std::vector<std::string> saved_active_input_method_list_;

  DISALLOW_COPY_AND_ASSIGN(ScreenLockObserver);
};

static base::LazyInstance<ScreenLockObserver> g_screen_lock_observer =
    LAZY_INSTANCE_INITIALIZER;

}  // namespace

namespace chromeos {

// static
ScreenLocker* ScreenLocker::screen_locker_ = NULL;

//////////////////////////////////////////////////////////////////////////////
// ScreenLocker, public:

ScreenLocker::ScreenLocker(const User& user)
    : user_(user),
      // TODO(oshima): support auto login mode (this is not implemented yet)
      // http://crosbug.com/1881
      unlock_on_input_(user_.email().empty()),
      locked_(false),
      start_time_(base::Time::Now()),
      login_status_consumer_(NULL),
      incorrect_passwords_count_(0) {
  DCHECK(!screen_locker_);
  screen_locker_ = this;
}

void ScreenLocker::Init() {
  authenticator_ = LoginUtils::Get()->CreateAuthenticator(this);
  delegate_.reset(new WebUIScreenLocker(this));
  delegate_->LockScreen(unlock_on_input_);
}

void ScreenLocker::OnLoginFailure(const LoginFailure& error) {
  DVLOG(1) << "OnLoginFailure";
  content::RecordAction(UserMetricsAction("ScreenLocker_OnLoginFailure"));
  if (authentication_start_time_.is_null()) {
    LOG(ERROR) << "authentication_start_time_ is not set";
  } else {
    base::TimeDelta delta = base::Time::Now() - authentication_start_time_;
    VLOG(1) << "Authentication failure time: " << delta.InSecondsF();
    UMA_HISTOGRAM_TIMES("ScreenLocker.AuthenticationFailureTime", delta);
  }

  EnableInput();
  // Don't enable signout button here as we're showing
  // MessageBubble.

  delegate_->ShowErrorMessage(incorrect_passwords_count_++ ?
                                  IDS_LOGIN_ERROR_AUTHENTICATING_2ND_TIME :
                                  IDS_LOGIN_ERROR_AUTHENTICATING,
                              HelpAppLauncher::HELP_CANT_ACCESS_ACCOUNT);

  if (login_status_consumer_)
    login_status_consumer_->OnLoginFailure(error);
}

void ScreenLocker::OnLoginSuccess(
    const std::string& username,
    const std::string& password,
    bool pending_requests,
    bool using_oauth) {
  VLOG(1) << "OnLoginSuccess: Sending Unlock request.";
  incorrect_passwords_count_ = 0;
  if (authentication_start_time_.is_null()) {
    if (!username.empty())
      LOG(WARNING) << "authentication_start_time_ is not set";
  } else {
    base::TimeDelta delta = base::Time::Now() - authentication_start_time_;
    VLOG(1) << "Authentication success time: " << delta.InSecondsF();
    UMA_HISTOGRAM_TIMES("ScreenLocker.AuthenticationSuccessTime", delta);
  }

  Profile* profile = ProfileManager::GetDefaultProfile();
  if (profile && !password.empty()) {
    // We have a non-empty password, so notify listeners (such as the sync
    // engine).
    SigninManager* signin = SigninManagerFactory::GetForProfile(profile);
    DCHECK(signin);
    GoogleServiceSigninSuccessDetails details(
        signin->GetAuthenticatedUsername(),
        password);
    content::NotificationService::current()->Notify(
        chrome::NOTIFICATION_GOOGLE_SIGNIN_SUCCESSFUL,
        content::Source<Profile>(profile),
        content::Details<const GoogleServiceSigninSuccessDetails>(&details));
  }
  DBusThreadManager::Get()->GetSessionManagerClient()->RequestUnlockScreen();

  if (login_status_consumer_)
    login_status_consumer_->OnLoginSuccess(username, password, pending_requests,
                                           using_oauth);
}

void ScreenLocker::Authenticate(const string16& password) {
  authentication_start_time_ = base::Time::Now();
  delegate_->SetInputEnabled(false);
  delegate_->OnAuthenticate();

  // If LoginPerformer instance exists,
  // initial online login phase is still active.
  if (LoginPerformer::default_performer()) {
    DVLOG(1) << "Delegating authentication to LoginPerformer.";
    LoginPerformer::default_performer()->PerformLogin(
        user_.email(), UTF16ToUTF8(password),
        LoginPerformer::AUTH_MODE_INTERNAL);
  } else {
    BrowserThread::PostTask(
        BrowserThread::UI, FROM_HERE,
        base::Bind(&Authenticator::AuthenticateToUnlock, authenticator_.get(),
                   user_.email(), UTF16ToUTF8(password)));
  }
}

void ScreenLocker::ClearErrors() {
  delegate_->ClearErrors();
}

void ScreenLocker::EnableInput() {
  delegate_->SetInputEnabled(true);
}

void ScreenLocker::Signout() {
  delegate_->ClearErrors();
  content::RecordAction(UserMetricsAction("ScreenLocker_Signout"));
  DBusThreadManager::Get()->GetSessionManagerClient()->StopSession();

  // Don't hide yet the locker because the chrome screen may become visible
  // briefly.
}

void ScreenLocker::ShowErrorMessage(int error_msg_id,
                                    HelpAppLauncher::HelpTopic help_topic_id,
                                    bool sign_out_only) {
  delegate_->SetInputEnabled(!sign_out_only);
  delegate_->ShowErrorMessage(error_msg_id, help_topic_id);
}

void ScreenLocker::SetLoginStatusConsumer(
    chromeos::LoginStatusConsumer* consumer) {
  login_status_consumer_ = consumer;
}

// static
void ScreenLocker::Show() {
  DVLOG(1) << "In ScreenLocker::Show";
  content::RecordAction(UserMetricsAction("ScreenLocker_Show"));
  DCHECK(MessageLoop::current()->type() == MessageLoop::TYPE_UI);

  // Check whether the currently logged in user is a guest account and if so,
  // refuse to lock the screen (crosbug.com/23764).
  // For a demo user, we should never show the lock screen (crosbug.com/27647).
  // TODO(flackr): We can allow lock screen for guest accounts when
  // unlock_on_input is supported by the WebUI screen locker.
  if (UserManager::Get()->IsLoggedInAsGuest() ||
      UserManager::Get()->IsLoggedInAsDemoUser()) {
    DVLOG(1) << "Show: Refusing to lock screen for guest/demo account.";
    return;
  }

  // Exit fullscreen.
  Browser* browser = BrowserList::GetLastActive();
  // browser can be NULL if we receive a lock request before the first browser
  // window is shown.
  if (browser && browser->window()->IsFullscreen()) {
    chrome::ToggleFullscreenMode(browser);
  }

  if (!screen_locker_) {
    DVLOG(1) << "Show: Locking screen";
    ScreenLocker* locker =
        new ScreenLocker(UserManager::Get()->GetLoggedInUser());
    locker->Init();
  } else {
    DVLOG(1) << "Show: locker already exists. Just sending completion event.";
    DBusThreadManager::Get()->GetPowerManagerClient()->
        NotifyScreenLockCompleted();
  }
}

// static
void ScreenLocker::Hide() {
  DCHECK(MessageLoop::current()->type() == MessageLoop::TYPE_UI);
  // For a guest/demo user, screen_locker_ would have never been initialized.
  if (UserManager::Get()->IsLoggedInAsGuest() ||
      UserManager::Get()->IsLoggedInAsDemoUser()) {
    DVLOG(1) << "Hide: Nothing to do for guest/demo account.";
    return;
  }

  DCHECK(screen_locker_);
  VLOG(1) << "Hide: Deleting ScreenLocker: " << screen_locker_;
  MessageLoopForUI::current()->DeleteSoon(FROM_HERE, screen_locker_);
}

// static
void ScreenLocker::InitClass() {
  g_screen_lock_observer.Get();
}

////////////////////////////////////////////////////////////////////////////////
// ScreenLocker, private:

ScreenLocker::~ScreenLocker() {
  DCHECK(MessageLoop::current()->type() == MessageLoop::TYPE_UI);
  ClearErrors();
  ash::Shell::GetInstance()->
      desktop_background_controller()->MoveDesktopToUnlockedContainer();

  screen_locker_ = NULL;
  bool state = false;
  content::NotificationService::current()->Notify(
      chrome::NOTIFICATION_SCREEN_LOCK_STATE_CHANGED,
      content::Source<ScreenLocker>(this),
      content::Details<bool>(&state));
  DBusThreadManager::Get()->GetPowerManagerClient()->
      NotifyScreenUnlockCompleted();
}

void ScreenLocker::SetAuthenticator(Authenticator* authenticator) {
  authenticator_ = authenticator;
}

void ScreenLocker::ScreenLockReady() {
  VLOG(1) << "ScreenLockReady: sending completed signal to power manager.";
  locked_ = true;
  base::TimeDelta delta = base::Time::Now() - start_time_;
  VLOG(1) << "Screen lock time: " << delta.InSecondsF();
  UMA_HISTOGRAM_TIMES("ScreenLocker.ScreenLockTime", delta);

  ash::Shell::GetInstance()->
      desktop_background_controller()->MoveDesktopToLockedContainer();

  bool state = true;
  content::NotificationService::current()->Notify(
      chrome::NOTIFICATION_SCREEN_LOCK_STATE_CHANGED,
      content::Source<ScreenLocker>(this),
      content::Details<bool>(&state));
  DBusThreadManager::Get()->GetPowerManagerClient()->
      NotifyScreenLockCompleted();
}

}  // namespace chromeos
