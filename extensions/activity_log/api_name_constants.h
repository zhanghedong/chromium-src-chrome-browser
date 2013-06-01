// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Constants used in api_actions.cc.

#ifndef CHROME_BROWSER_EXTENSIONS_ACTIVITY_LOG_API_NAME_CONSTANTS_H_
#define CHROME_BROWSER_EXTENSIONS_ACTIVITY_LOG_API_NAME_CONSTANTS_H_

namespace activity_log_api_name_constants {

// All of the chrome.* API names and events. ADD TO THE END. Do not remove any!
const char* const kNames[] = {
    "alarms.clear", "alarms.clearAll", "alarms.create", "alarms.get",
    "alarms.getAll", "alarms.onAlarm", "app.runtime.onLaunched",
    "app.runtime.onRestarted", "app.window.create", "app.window.current",
    "app.window.onBoundsChanged", "app.window.onClosed",
    "app.window.onFullscreened", "app.window.onMaximized",
    "app.window.onMinimized", "app.window.onRestored", "bluetooth.connect",
    "bluetooth.disconnect", "bluetooth.getAdapterState",
    "bluetooth.getDevices", "bluetooth.getLocalOutOfBandPairingData",
    "bluetooth.getServices", "bluetooth.onAdapterStateChanged",
    "bluetooth.read", "bluetooth.setOutOfBandPairingData",
    "bluetooth.startDiscovery", "bluetooth.stopDiscovery", "bluetooth.write",
    "bookmarks.create", "bookmarks.get", "bookmarks.getChildren",
    "bookmarks.getRecent", "bookmarks.getSubTree", "bookmarks.getTree",
    "bookmarks.move", "bookmarks.onChanged", "bookmarks.onChildrenReordered",
    "bookmarks.onCreated", "bookmarks.onImportBegan",
    "bookmarks.onImportEnded", "bookmarks.onMoved", "bookmarks.onRemoved",
    "bookmarks.remove", "bookmarks.removeTree", "bookmarks.search",
    "bookmarks.update", "browserAction.disable", "browserAction.enable",
    "browserAction.getBadgeBackgroundColor", "browserAction.getBadgeText",
    "browserAction.getPopup", "browserAction.getTitle",
    "browserAction.onClicked", "browserAction.setBadgeBackgroundColor",
    "browserAction.setBadgeText", "browserAction.setIcon",
    "browserAction.setPopup", "browserAction.setTitle", "browsingData.remove",
    "browsingData.removeAppcache", "browsingData.removeCache",
    "browsingData.removeCookies", "browsingData.removeDownloads",
    "browsingData.removeFileSystems", "browsingData.removeFormData",
    "browsingData.removeHistory", "browsingData.removeIndexedDB",
    "browsingData.removeLocalStorage", "browsingData.removePasswords",
    "browsingData.removePluginData", "browsingData.removeWebSQL",
    "browsingData.settings", "commands.getAll", "commands.onCommand",
    "contextMenus.create", "contextMenus.onClicked", "contextMenus.remove",
    "contextMenus.removeAll", "contextMenus.update", "cookies.get",
    "cookies.getAll", "cookies.getAllCookieStores", "cookies.onChanged",
    "cookies.remove", "cookies.set", "debugger.attach", "debugger.detach",
    "debugger.getTargets", "debugger.onDetach", "debugger.onEvent",
    "debugger.sendCommand", "declarativeContent.onPageChanged",
    "declarativeWebRequest.onMessage", "declarativeWebRequest.onRequest",
    "devtools.inspectedWindow.eval", "devtools.inspectedWindow.getResources",
    "devtools.inspectedWindow.onResourceAdded",
    "devtools.inspectedWindow.onResourceContentCommitted",
    "devtools.inspectedWindow.reload", "devtools.network.getHAR",
    "devtools.network.onNavigated", "devtools.network.onRequestFinished",
    "devtools.panels.create", "devtools.panels.setOpenResourceHandler",
    "downloads.acceptDanger", "downloads.cancel", "downloads.download",
    "downloads.drag", "downloads.erase", "downloads.getFileIcon",
    "downloads.onChanged", "downloads.onCreated",
    "downloads.onDeterminingFilename", "downloads.onErased", "downloads.open",
    "downloads.pause", "downloads.resume", "downloads.search",
    "downloads.show", "experimental.devtools.audits.addCategory",
    "experimental.devtools.console.addMessage",
    "experimental.devtools.console.getMessages",
    "experimental.devtools.console.onMessageAdded",
    "experimental.discovery.clearAllSuggestions",
    "experimental.discovery.removeSuggestion",
    "experimental.discovery.suggest", "experimental.history.getMostVisited",
    "experimental.identity.getAuthToken",
    "experimental.identity.launchWebAuthFlow", "experimental.infobars.show",
    "experimental.mediaGalleries.assembleMediaFile",
    "experimental.mediaGalleries.extractEmbeddedThumbnails",
    "experimental.processes.getProcessIdForTab",
    "experimental.processes.getProcessInfo",
    "experimental.processes.onCreated", "experimental.processes.onExited",
    "experimental.processes.onUnresponsive",
    "experimental.processes.onUpdated",
    "experimental.processes.onUpdatedWithMemory",
    "experimental.processes.terminate",
    "experimental.record.captureURLs", "experimental.record.replayURLs",
    "experimental.speechInput.isRecording", "experimental.speechInput.onError",
    "experimental.speechInput.onResult", "experimental.speechInput.onSoundEnd",
    "experimental.speechInput.onSoundStart", "experimental.speechInput.start",
    "experimental.speechInput.stop", "experimental.systemInfo.cpu.get" ,
    "experimental.systemInfo.cpu.onUpdated",
    "experimental.systemInfo.display.get",
    "experimental.systemInfo.memory.get", "experimental.systemInfo.storage.get",
     "experimental.systemInfo.storage.onAttached",
    "experimental.systemInfo.storage.onAvailableCapacityChanged",
    "experimental.systemInfo.storage.onDetached",
    "extension.getBackgroundPage", "extension.getURL", "extension.getViews",
    "extension.isAllowedFileSchemeAccess",
    "extension.isAllowedIncognitoAccess", "extension.setUpdateUrlData",
    "fileBrowserHandler.onExecute", "fileBrowserHandler.selectFile",
    "fileSystem.chooseEntry", "fileSystem.getDisplayPath",
    "fileSystem.getEntryById", "fileSystem.getEntryId",
    "fileSystem.getWritableEntry", "fileSystem.isWritableEntry",
    "fontSettings.clearDefaultFixedFontSize",
    "fontSettings.clearDefaultFontSize", "fontSettings.clearFont",
    "fontSettings.clearMinimumFontSize",
    "fontSettings.getDefaultFixedFontSize", "fontSettings.getDefaultFontSize",
    "fontSettings.getFont", "fontSettings.getFontList",
    "fontSettings.getMinimumFontSize",
    "fontSettings.onDefaultFixedFontSizeChanged",
    "fontSettings.onDefaultFontSizeChanged", "fontSettings.onFontChanged",
    "fontSettings.onMinimumFontSizeChanged",
    "fontSettings.setDefaultFixedFontSize", "fontSettings.setDefaultFontSize",
    "fontSettings.setFont", "fontSettings.setMinimumFontSize",
    "history.addUrl", "history.deleteAll", "history.deleteRange",
    "history.deleteUrl", "history.getVisits", "history.onVisitRemoved",
    "history.onVisited", "history.search", "i18n.getAcceptLanguages",
    "i18n.getMessage", "idle.onStateChanged", "idle.queryState",
    "idle.setDetectionInterval", "input.ime.clearComposition",
    "input.ime.commitText", "input.ime.deleteSurroundingText",
    "input.ime.keyEventHandled", "input.ime.onActivate", "input.ime.onBlur",
    "input.ime.onCandidateClicked", "input.ime.onDeactivated",
    "input.ime.onFocus", "input.ime.onInputContextUpdate",
    "input.ime.onKeyEvent", "input.ime.onMenuItemActivated",
    "input.ime.onSurroundingTextChanged",
    "input.ime.setCandidateWindowProperties", "input.ime.setCandidates",
    "input.ime.setComposition", "input.ime.setCursorPosition",
    "input.ime.setMenuItems", "input.ime.updateMenuItems", "management.get",
    "management.getAll", "management.getPermissionWarningsById",
    "management.getPermissionWarningsByManifest", "management.launchApp",
    "management.onDisabled", "management.onEnabled", "management.onInstalled",
    "management.onUninstalled", "management.setEnabled",
    "management.uninstall", "management.uninstallSelf",
    "mediaGalleries.getMediaFileSystemMetadata",
    "mediaGalleries.getMediaFileSystems", "notifications.clear",
    "notifications.create", "notifications.onButtonClicked",
    "notifications.onClicked", "notifications.onClosed",
    "notifications.onDisplayed", "notifications.update",
    "omnibox.onInputCancelled", "omnibox.onInputChanged",
    "omnibox.onInputEntered", "omnibox.onInputStarted",
    "omnibox.setDefaultSuggestion", "pageAction.getPopup",
    "pageAction.getTitle", "pageAction.hide", "pageAction.onClicked",
    "pageAction.setIcon", "pageAction.setPopup", "pageAction.setTitle",
    "pageAction.show", "pageCapture.saveAsMHTML", "permissions.contains",
    "permissions.getAll", "permissions.onAdded", "permissions.onRemoved",
    "permissions.remove", "permissions.request", "power.releaseKeepAwake",
    "power.requestKeepAwake", "proxy.onProxyError",
    "pushMessaging.getChannelId", "pushMessaging.onMessage", "runtime.connect",
    "runtime.getBackgroundPage", "runtime.getManifest", "runtime.getURL",
    "runtime.onBrowserUpdateAvailable", "runtime.onConnect",
    "runtime.onConnectExternal", "runtime.onInstalled", "runtime.onMessage",
    "runtime.onMessageExternal", "runtime.onStartup", "runtime.onSuspend",
    "runtime.onSuspendCanceled", "runtime.onUpdateAvailable", "runtime.reload",
    "runtime.requestUpdateCheck", "runtime.sendMessage",
    "scriptBadge.getAttention", "scriptBadge.getPopup",
    "scriptBadge.onClicked", "scriptBadge.setPopup", "serial.close",
    "serial.flush", "serial.getControlSignals", "serial.getPorts",
    "serial.open", "serial.read", "serial.setControlSignals", "serial.write",
    "sessionRestore.getRecentlyClosed", "sessionRestore.restore",
    "socket.accept", "socket.bind", "socket.connect", "socket.create",
    "socket.destroy", "socket.disconnect", "socket.getInfo",
    "socket.getNetworkList", "socket.listen", "socket.read", "socket.recvFrom",
    "socket.sendTo", "socket.setKeepAlive", "socket.setNoDelay",
    "socket.write", "storage.onChanged",
    "syncFileSystem.getConflictResolutionPolicy",
    "syncFileSystem.getFileStatus", "syncFileSystem.getFileStatuses",
    "syncFileSystem.getUsageAndQuota", "syncFileSystem.onFileStatusChanged",
    "syncFileSystem.onServiceStatusChanged",
    "syncFileSystem.requestFileSystem",
    "syncFileSystem.setConflictResolutionPolicy", "tabCapture.capture",
    "tabCapture.getCapturedTabs", "tabCapture.onStatusChanged",
    "tabs.captureVisibleTab", "tabs.connect", "tabs.create",
    "tabs.detectLanguage", "tabs.duplicate", "tabs.executeScript", "tabs.get",
    "tabs.getCurrent", "tabs.highlight", "tabs.insertCSS", "tabs.move",
    "tabs.onActivated", "tabs.onAttached", "tabs.onCreated", "tabs.onDetached",
    "tabs.onHighlighted", "tabs.onMoved", "tabs.onRemoved", "tabs.onReplaced",
    "tabs.onUpdated", "tabs.query", "tabs.reload", "tabs.remove",
    "tabs.sendMessage", "tabs.update", "topSites.get", "tts.getVoices",
    "tts.isSpeaking", "tts.speak", "tts.stop", "ttsEngine.onSpeak",
    "ttsEngine.onStop", "usb.bulkTransfer", "usb.claimInterface",
    "usb.closeDevice", "usb.controlTransfer", "usb.findDevices",
    "usb.interruptTransfer", "usb.isochronousTransfer", "usb.releaseInterface",
    "usb.setInterfaceAlternateSetting", "webNavigation.getAllFrames",
    "webNavigation.getFrame", "webNavigation.onBeforeNavigate",
    "webNavigation.onCommitted", "webNavigation.onCompleted",
    "webNavigation.onCreatedNavigationTarget",
    "webNavigation.onDOMContentLoaded", "webNavigation.onErrorOccurred",
    "webNavigation.onHistoryStateUpdated",
    "webNavigation.onReferenceFragmentUpdated", "webNavigation.onTabReplaced",
    "webRequest.handlerBehaviorChanged", "webRequest.onAuthRequired",
    "webRequest.onBeforeRedirect", "webRequest.onBeforeRequest",
    "webRequest.onBeforeSendHeaders", "webRequest.onCompleted",
    "webRequest.onErrorOccurred", "webRequest.onHeadersReceived",
    "webRequest.onResponseStarted", "webRequest.onSendHeaders",
    "webstore.install", "windows.create", "windows.get", "windows.getAll",
    "windows.getCurrent", "windows.getLastFocused", "windows.onCreated",
    "windows.onFocusChanged", "windows.onRemoved", "windows.remove",
    "windows.update",
    "tabs.getSelected", "tabs.sendRequest",
    "systemInfo.cpu.get"
};

}  // namespace activity_log_api_name_constants

#endif  // CHROME_BROWSER_EXTENSIONS_ACTIVITY_LOG_API_NAME_CONSTANTS_H_
