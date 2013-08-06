// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_EXTENSIONS_EXTENSION_FUNCTION_HISTOGRAM_VALUE_H_
#define CHROME_BROWSER_EXTENSIONS_EXTENSION_FUNCTION_HISTOGRAM_VALUE_H_


namespace extensions {
namespace functions {

// Short version:
//  *Never* reorder or delete entries in the |HistogramValue| enumeration.
//  When creating a new extension function, add a new entry at the end of the
//  enum, just prior to ENUM_BOUNDARY.
//
// Long version:
//  This enumeration is used to associate a unique integer value to each
//  extension function so that their usage can be recorded in histogram charts.
//  Given we want the values recorded in the these charts to remain stable over
//  time for comparison purposes, once an entry has been added to the
//  enumeration, it should never be removed or moved to another spot in the
//  enum.
//
//  Here are instructions how to manage entries depending on what you are trying
//  to achieve.
//
//  1) Creating a new extension function:
//
//      Add a new entry at the end of the |HistogramValue| enum. The name of the
//      entry should follow this algorithm:
//      a) Take the string value passed as first argument to
//         DECLARE_EXTENSION_FUNCTION.
//      b) Replace '.' with '_'.
//      c) Make all letters uppercase.
//
//      Example: "tabs.create" -> TABS_CREATE
//
//  2) Deleting an existing function:
//
//      Given an existing entry should *never* be removed from this enumeration,
//      it is recommended to add a "DELETED_" prefix to the existing entry.
//
//  3) Renaming an existing function:
//
//      There are 2 options, depending if you want to keep accruing data in the
//      *existing* histogram stream or in a *new* one.
//
//      a) If you want keep recording usages of the extension function in the
//         *existing* histogram stream, simply rename the enum entry to match
//         the new extension function name, following the same naming rule as
//         mentioned in 1). The enum entry will keep the same underlying integer
//         value, so the same histogram stream will be used for recording
//         usages.
//
//      b) If you want start recording usages of the extension function to in a
//         *new* histogram stream, follow the instructions in step 1) and 2)
//         above. This will effectively deprecate the old histogram stream and
//         creates a new one for the new function name.
//
enum HistogramValue {
  UNKNOWN = 0,
  WEBNAVIGATION_GETALLFRAMES,
  BROWSINGDATA_REMOVEWEBSQL,
  ALARMS_CREATE,
  FILEBROWSERPRIVATE_REMOVEFILEWATCH,
  COOKIES_GET,
  FONTSETTINGS_GETMINIMUMFONTSIZE,
  CHROMEOSINFOPRIVATE_GET,
  BOOKMARKMANAGERPRIVATE_CUT,
  TABS_CAPTUREVISIBLETAB,
  MANAGEMENT_SETENABLED,
  HISTORY_DELETEALL,
  STORAGE_GET,
  SOCKET_SETKEEPALIVE,
  DOWNLOADS_CANCEL,
  BOOKMARKS_CREATE,
  BOOKMARKS_UPDATE,
  FILEBROWSERPRIVATE_GETDRIVEFILES,
  TERMINALPRIVATE_ONTERMINALRESIZE,
  DELETED_FILEBROWSERPRIVATE_REQUESTDIRECTORYREFRESH,
  BLUETOOTH_GETADAPTERSTATE,
  FILEBROWSERPRIVATE_CANCELFILETRANSFERS,
  FILEBROWSERPRIVATE_PINDRIVEFILE,
  SOCKET_WRITE,
  OMNIBOX_SETDEFAULTSUGGESTION,
  TTS_SPEAK,
  WALLPAPERPRIVATE_RESTOREMINIMIZEDWINDOWS,
  BROWSINGDATA_REMOVEHISTORY,
  DELETED_FILEBROWSERPRIVATE_ISFULLSCREEN,
  AUTOTESTPRIVATE_LOGOUT,
  EXPERIMENTAL_HISTORY_GETMOSTVISITED,
  BLUETOOTH_DISCONNECT,
  BLUETOOTH_SETOUTOFBANDPAIRINGDATA,
  BOOKMARKMANAGERPRIVATE_CANPASTE,
  AUTOTESTPRIVATE_RESTART,
  USB_CLAIMINTERFACE,
  MEDIAPLAYERPRIVATE_SETWINDOWHEIGHT,
  EXPERIMENTAL_PROCESSES_GETPROCESSINFO,
  HISTORY_GETVISITS,
  SOCKET_BIND,
  TABS_MOVE,
  SOCKET_DISCONNECT,
  FILESYSTEM_GETWRITABLEENTRY,
  SYNCFILESYSTEM_REQUESTFILESYSTEM,
  COMMANDS_GETALL,
  EXPERIMENTAL_DISCOVERY_REMOVESUGGESTION,
  EXPERIMENTAL_INPUT_VIRTUALKEYBOARD_SENDKEYBOARDEVENT,
  BOOKMARKMANAGERPRIVATE_GETSUBTREE,
  EXPERIMENTAL_RLZ_RECORDPRODUCTEVENT,
  BOOKMARKS_GETRECENT,
  APP_CURRENTWINDOWINTERNAL_SETBOUNDS,
  CLOUDPRINTPRIVATE_SETUPCONNECTOR,
  SERIAL_SETCONTROLSIGNALS,
  FILEBROWSERPRIVATE_SETLASTMODIFIED,
  IDLE_SETDETECTIONINTERVAL,
  FILEBROWSERPRIVATE_GETFILETASKS,
  WEBSTOREPRIVATE_GETSTORELOGIN,
  SYSTEMPRIVATE_GETINCOGNITOMODEAVAILABILITY,
  EXPERIMENTAL_IDLTEST_SENDARRAYBUFFERVIEW,
  SOCKET_SETNODELAY,
  APP_CURRENTWINDOWINTERNAL_SHOW,
  WEBSTOREPRIVATE_GETBROWSERLOGIN,
  EXPERIMENTAL_IDENTITY_GETAUTHTOKEN,
  DELETED_SYSTEMINFO_DISPLAY_GETDISPLAYINFO,
  BROWSINGDATA_REMOVEPLUGINDATA,
  SOCKET_LISTEN,
  MEDIAGALLERIES_GETMEDIAFILESYSTEMS,
  DOWNLOADS_OPEN,
  TABS_EXECUTESCRIPT,
  SYNCFILESYSTEM_GETUSAGEANDQUOTA,
  INPUTMETHODPRIVATE_GET,
  USB_CLOSEDEVICE,
  TTS_STOP,
  SERIAL_GETPORTS,
  FILEBROWSERPRIVATE_CLEARDRIVECACHE,
  SERIAL_GETCONTROLSIGNALS,
  DEVELOPERPRIVATE_ENABLE,
  FILEBROWSERPRIVATE_GETDRIVEFILEPROPERTIES,
  USB_FINDDEVICES,
  BOOKMARKMANAGERPRIVATE_DROP,
  DELETED_FILEBROWSERPRIVATE_GETFILETRANSFERS,
  INPUT_IME_SETMENUITEMS,
  BOOKMARKS_EXPORT,
  HISTORY_SEARCH,
  TTSENGINE_SENDTTSEVENT,
  EXPERIMENTAL_ACCESSIBILITY_GETALERTSFORTAB,
  BOOKMARKS_IMPORT,
  SYNCFILESYSTEM_DELETEFILESYSTEM,
  DEBUGGER_SENDCOMMAND,
  DEBUGGER_DETACH,
  METRICSPRIVATE_RECORDSMALLCOUNT,
  APP_CURRENTWINDOWINTERNAL_MINIMIZE,
  DEVELOPERPRIVATE_AUTOUPDATE,
  EXPERIMENTAL_DNS_RESOLVE,
  DELETED_EXPERIMENTAL_SYSTEMINFO_MEMORY_GET,
  HISTORY_ADDURL,
  TABS_GET,
  BROWSERACTION_SETBADGETEXT,
  TABS_RELOAD,
  WINDOWS_CREATE,
  DEVELOPERPRIVATE_LOADUNPACKED,
  DELETED_DOWNLOADS_SETDESTINATION,
  EXPERIMENTAL_PROCESSES_GETPROCESSIDFORTAB,
  BOOKMARKS_GETCHILDREN,
  BROWSERACTION_GETTITLE,
  TERMINALPRIVATE_OPENTERMINALPROCESS,
  SERIAL_CLOSE,
  CONTEXTMENUS_REMOVE,
  FILEBROWSERPRIVATE_REQUESTFILESYSTEM,
  ECHOPRIVATE_GETREGISTRATIONCODE,
  TABS_GETCURRENT,
  FONTSETTINGS_CLEARDEFAULTFIXEDFONTSIZE,
  MEDIAPLAYERPRIVATE_CLOSEWINDOW,
  WEBREQUESTINTERNAL_ADDEVENTLISTENER,
  CLOUDPRINTPRIVATE_GETPRINTERS,
  STORAGE_SET,
  FONTSETTINGS_GETDEFAULTFONTSIZE,
  EXTENSION_SETUPDATEURLDATA,
  SERIAL_WRITE,
  IDLE_QUERYSTATE,
  EXPERIMENTAL_RLZ_GETACCESSPOINTRLZ,
  WEBSTOREPRIVATE_SETSTORELOGIN,
  PAGEACTIONS_ENABLEFORTAB,
  COOKIES_SET,
  CONTENTSETTINGS_SET,
  CONTEXTMENUS_REMOVEALL,
  TABS_INSERTCSS,
  WEBREQUEST_HANDLERBEHAVIORCHANGED,
  INPUT_IME_SETCURSORPOSITION,
  OMNIBOX_SENDSUGGESTIONS,
  SYSTEMINDICATOR_ENABLE,
  EVENTS_GETRULES,
  BOOKMARKMANAGERPRIVATE_COPY,
  SOCKET_RECVFROM,
  TABS_GETALLINWINDOW,
  CONTEXTMENUS_UPDATE,
  BOOKMARKS_SEARCH,
  EXPERIMENTAL_APP_CLEARALLNOTIFICATIONS,
  BLUETOOTH_GETLOCALOUTOFBANDPAIRINGDATA,
  SYSTEMPRIVATE_GETUPDATESTATUS,
  FONTSETTINGS_CLEARMINIMUMFONTSIZE,
  DELETED_FILEBROWSERPRIVATE_GETFILELOCATIONS,
  EXPERIMENTAL_DISCOVERY_SUGGEST,
  FILEBROWSERPRIVATE_SETDEFAULTTASK,
  BROWSERACTION_GETBADGETEXT,
  APP_CURRENTWINDOWINTERNAL_HIDE,
  SOCKET_CONNECT,
  BOOKMARKS_GETSUBTREE,
  HISTORY_DELETEURL,
  DELETED_EXPERIMENTAL_MEDIAGALLERIES_ASSEMBLEMEDIAFILE,
  BOOKMARKMANAGERPRIVATE_STARTDRAG,
  BROWSINGDATA_REMOVEPASSWORDS,
  DOWNLOADS_DRAG,
  INPUT_IME_SETCOMPOSITION,
  METRICSPRIVATE_RECORDUSERACTION,
  USB_RELEASEINTERFACE,
  PAGEACTION_GETPOPUP,
  SCRIPTBADGE_GETATTENTION,
  FONTSETTINGS_GETFONTLIST,
  PERMISSIONS_CONTAINS,
  SCRIPTBADGE_GETPOPUP,
  EXPERIMENTAL_ACCESSIBILITY_GETFOCUSEDCONTROL,
  DEVELOPERPRIVATE_GETSTRINGS,
  METRICSPRIVATE_RECORDMEDIUMCOUNT,
  MANAGEMENT_GET,
  PERMISSIONS_GETALL,
  DOWNLOADS_SHOW,
  EXPERIMENTAL_RLZ_CLEARPRODUCTSTATE,
  TABS_REMOVE,
  MANAGEMENT_GETPERMISSIONWARNINGSBYID,
  WINDOWS_GET,
  FILEBROWSERPRIVATE_EXECUTETASK,
  TTS_GETVOICES,
  MANAGEMENT_GETALL,
  MANAGEMENT_GETPERMISSIONWARNINGSBYMANIFEST,
  APP_CURRENTWINDOWINTERNAL_CLEARATTENTION,
  AUTOTESTPRIVATE_SHUTDOWN,
  FONTSETTINGS_CLEARDEFAULTFONTSIZE,
  BOOKMARKS_GETTREE,
  FILEBROWSERPRIVATE_SELECTFILES,
  RUNTIME_GETBACKGROUNDPAGE,
  DELETED_EXPERIMENTAL_RECORD_REPLAYURLS,
  WEBSTOREPRIVATE_COMPLETEINSTALL,
  DELETED_EXPERIMENTAL_SPEECHINPUT_START,
  COOKIES_GETALL,
  DOWNLOADS_GETFILEICON,
  PAGEACTION_GETTITLE,
  BROWSINGDATA_REMOVE,
  SERIAL_OPEN,
  FILESYSTEM_GETDISPLAYPATH,
  FILEBROWSERPRIVATE_FORMATDEVICE,
  BOOKMARKS_GET,
  DELETED_MANAGEDMODEPRIVATE_GET,
  ALARMS_CLEAR,
  SYNCFILESYSTEM_GETFILESYNCSTATUS,
  SOCKET_GETINFO,
  WEBSTOREPRIVATE_INSTALLBUNDLE,
  BROWSERACTION_ENABLE,
  METRICSPRIVATE_RECORDMEDIUMTIME,
  PAGEACTION_SETTITLE,
  CLOUDPRINTPRIVATE_GETHOSTNAME,
  CONTENTSETTINGS_GETRESOURCEIDENTIFIERS,
  SOCKET_CREATE,
  DEVELOPERPRIVATE_RELOAD,
  FILEBROWSERPRIVATE_GETMOUNTPOINTS,
  APP_RUNTIME_POSTINTENTRESPONSE,
  DELETED_MANAGEDMODEPRIVATE_SETPOLICY,
  WEBSTOREPRIVATE_BEGININSTALLWITHMANIFEST3,
  WALLPAPERPRIVATE_SETWALLPAPER,
  USB_CONTROLTRANSFER,
  DELETED_EXPERIMENTAL_SPEECHINPUT_STOP,
  USB_BULKTRANSFER,
  FILEBROWSERPRIVATE_GETVOLUMEMETADATA,
  PAGECAPTURE_SAVEASMHTML,
  EXTENSION_ISALLOWEDINCOGNITOACCESS,
  BROWSINGDATA_REMOVEAPPCACHE,
  APP_CURRENTWINDOWINTERNAL_DRAWATTENTION,
  METRICSPRIVATE_RECORDCOUNT,
  USB_INTERRUPTTRANSFER,
  TYPES_CHROMESETTING_CLEAR,
  INPUT_IME_COMMITTEXT,
  EXPERIMENTAL_IDLTEST_SENDARRAYBUFFER,
  WALLPAPERPRIVATE_SETWALLPAPERIFEXISTS,
  SOCKET_ACCEPT,
  WEBNAVIGATION_GETFRAME,
  EXPERIMENTAL_POWER_RELEASEKEEPAWAKE,
  APP_CURRENTWINDOWINTERNAL_SETICON,
  PUSHMESSAGING_GETCHANNELID,
  EXPERIMENTAL_INFOBARS_SHOW,
  INPUT_IME_SETCANDIDATEWINDOWPROPERTIES,
  METRICSPRIVATE_RECORDPERCENTAGE,
  TYPES_CHROMESETTING_GET,
  WINDOWS_GETLASTFOCUSED,
  DELETED_MANAGEDMODEPRIVATE_GETPOLICY,
  STORAGE_CLEAR,
  STORAGE_GETBYTESINUSE,
  TABS_QUERY,
  PAGEACTION_SETPOPUP,
  DEVELOPERPRIVATE_INSPECT,
  DOWNLOADS_SEARCH,
  FONTSETTINGS_CLEARFONT,
  WINDOWS_UPDATE,
  BOOKMARKMANAGERPRIVATE_CANOPENNEWWINDOWS,
  SERIAL_FLUSH,
  BROWSERACTION_SETTITLE,
  BOOKMARKMANAGERPRIVATE_CANEDIT,
  WALLPAPERPRIVATE_SETCUSTOMWALLPAPER,
  BOOKMARKS_REMOVE,
  INPUT_IME_SETCANDIDATES,
  TERMINALPRIVATE_CLOSETERMINALPROCESS,
  HISTORY_DELETERANGE,
  EXPERIMENTAL_IDLTEST_GETARRAYBUFFER,
  TERMINALPRIVATE_SENDINPUT,
  TABS_HIGHLIGHT,
  BLUETOOTH_STARTDISCOVERY,
  FILEBROWSERPRIVATE_SELECTFILE,
  WINDOWS_GETCURRENT,
  DEBUGGER_ATTACH,
  WALLPAPERPRIVATE_SAVETHUMBNAIL,
  INPUT_IME_KEYEVENTHANDLED,
  FONTSETTINGS_SETDEFAULTFONTSIZE,
  RUNTIME_REQUESTUPDATECHECK,
  PAGEACTION_SETICON,
  BROWSERACTION_SETBADGEBACKGROUNDCOLOR,
  DEVELOPERPRIVATE_GETITEMSINFO,
  BLUETOOTH_STOPDISCOVERY,
  COOKIES_REMOVE,
  EXPERIMENTAL_RLZ_SENDFINANCIALPING,
  TABCAPTURE_GETCAPTUREDTABS,
  WINDOWS_REMOVE,
  WALLPAPERPRIVATE_GETOFFLINEWALLPAPERLIST,
  BROWSERACTION_GETBADGEBACKGROUNDCOLOR,
  PAGEACTIONS_DISABLEFORTAB,
  DEVELOPERPRIVATE_ALLOWFILEACCESS,
  FILEBROWSERPRIVATE_REMOVEMOUNT,
  BLUETOOTH_CONNECT,
  TABCAPTURE_CAPTURE,
  NOTIFICATIONS_CREATE,
  TABS_DUPLICATE,
  BLUETOOTH_WRITE,
  PAGEACTION_SHOW,
  WALLPAPERPRIVATE_GETTHUMBNAIL,
  DOWNLOADS_PAUSE,
  PERMISSIONS_REQUEST,
  TOPSITES_GET,
  BROWSINGDATA_REMOVEDOWNLOADS,
  BROWSINGDATA_REMOVELOCALSTORAGE,
  FILEBROWSERHANDLERINTERNAL_SELECTFILE,
  INPUT_IME_UPDATEMENUITEMS,
  FILEBROWSERPRIVATE_GETSTRINGS,
  CONTENTSETTINGS_GET,
  FONTSETTINGS_SETDEFAULTFIXEDFONTSIZE,
  EXPERIMENTAL_APP_NOTIFY,
  METRICSPRIVATE_RECORDLONGTIME,
  SOCKET_READ,
  EXPERIMENTAL_PROCESSES_TERMINATE,
  METRICSPRIVATE_RECORDTIME,
  BOOKMARKMANAGERPRIVATE_GETSTRINGS,
  USB_ISOCHRONOUSTRANSFER,
  PERMISSIONS_REMOVE,
  MANAGEMENT_UNINSTALL,
  I18N_GETACCEPTLANGUAGES,
  MANAGEMENT_LAUNCHAPP,
  INPUT_IME_CLEARCOMPOSITION,
  ALARMS_GETALL,
  DIAL_DISCOVERNOW,
  TYPES_CHROMESETTING_SET,
  BROWSERACTION_SETICON,
  EXPERIMENTAL_ACCESSIBILITY_SETACCESSIBILITYENABLED,
  FILEBROWSERPRIVATE_VIEWFILES,
  BLUETOOTH_GETSERVICES,
  TABS_UPDATE,
  BROWSINGDATA_REMOVEFORMDATA,
  DELETED_FILEBROWSERPRIVATE_RELOADDRIVE,
  ALARMS_GET,
  BROWSINGDATA_REMOVEINDEXEDDB,
  FILEBROWSERPRIVATE_ADDFILEWATCH,
  CONTENTSETTINGS_CLEAR,
  FILEBROWSERPRIVATE_GETPREFERENCES,
  BOOKMARKMANAGERPRIVATE_PASTE,
  FILESYSTEM_ISWRITABLEENTRY,
  USB_SETINTERFACEALTERNATESETTING,
  FONTSETTINGS_SETMINIMUMFONTSIZE,
  BROWSERACTION_GETPOPUP,
  SOCKET_DESTROY,
  BLUETOOTH_GETDEVICES,
  ALARMS_CLEARALL,
  FONTSETTINGS_GETDEFAULTFIXEDFONTSIZE,
  FILEBROWSERPRIVATE_ZIPSELECTION,
  SYSTEMINDICATOR_DISABLE,
  SCRIPTBADGE_SETPOPUP,
  EXTENSION_ISALLOWEDFILESCHEMEACCESS,
  EXPERIMENTAL_IDENTITY_LAUNCHWEBAUTHFLOW,
  FILEBROWSERPRIVATE_GETDRIVECONNECTIONSTATE,
  TABS_DETECTLANGUAGE,
  METRICSPRIVATE_RECORDVALUE,
  BOOKMARKMANAGERPRIVATE_SORTCHILDREN,
  SERIAL_READ,
  APP_CURRENTWINDOWINTERNAL_MAXIMIZE,
  EXPERIMENTAL_DISCOVERY_CLEARALLSUGGESTIONS,
  DELETED_MANAGEDMODEPRIVATE_ENTER,
  FILEBROWSERPRIVATE_TRANSFERFILE,
  BROWSERACTION_SETPOPUP,
  TABS_GETSELECTED,
  FONTSETTINGS_GETFONT,
  BLUETOOTH_READ,
  WEBREQUESTINTERNAL_EVENTHANDLED,
  EVENTS_ADDRULES,
  CONTEXTMENUS_CREATE,
  MEDIAPLAYERPRIVATE_GETPLAYLIST,
  DOWNLOADS_ERASE,
  DELETED_EXPERIMENTAL_RECORD_CAPTUREURLS,
  TTS_ISSPEAKING,
  BOOKMARKS_REMOVETREE,
  FILEBROWSERPRIVATE_SEARCHDRIVE,
  DELETED_EXPERIMENTAL_SYSTEMINFO_CPU_GET,
  FILEBROWSERPRIVATE_SETPREFERENCES,
  FONTSETTINGS_SETFONT,
  SOCKET_GETNETWORKLIST,
  BOOKMARKS_MOVE,
  WALLPAPERPRIVATE_MINIMIZEINACTIVEWINDOWS,
  STORAGE_REMOVE,
  AUTOTESTPRIVATE_LOGINSTATUS,
  TABS_CREATE,
  FILEBROWSERPRIVATE_CANCELDIALOG,
  BROWSINGDATA_REMOVECOOKIES,
  FILESYSTEM_CHOOSEENTRY,
  MEDIAPLAYERPRIVATE_PLAY,
  WEBSTOREPRIVATE_GETWEBGLSTATUS,
  SOCKET_SENDTO,
  BROWSINGDATA_REMOVEFILESYSTEMS,
  WALLPAPERPRIVATE_GETSTRINGS,
  BROWSINGDATA_REMOVECACHE,
  BOOKMARKMANAGERPRIVATE_RECORDLAUNCH,
  BROWSERACTION_DISABLE,
  DELETED_EXPERIMENTAL_SPEECHINPUT_ISRECORDING,
  APP_WINDOW_CREATE,
  RUNTIME_RELOAD,
  EXPERIMENTAL_POWER_REQUESTKEEPAWAKE,
  SYSTEMINDICATOR_SETICON,
  FILEBROWSERPRIVATE_ADDMOUNT,
  APP_CURRENTWINDOWINTERNAL_FOCUS,
  EVENTS_REMOVERULES,
  DOWNLOADS_DOWNLOAD,
  WINDOWS_GETALL,
  DELETED_FILEBROWSERPRIVATE_TOGGLEFULLSCREEN,
  APP_CURRENTWINDOWINTERNAL_RESTORE,
  DELETED_WEBSOCKETPROXYPRIVATE_GETPASSPORTFORTCP,
  PAGEACTION_HIDE,
  DELETED_EXPERIMENTAL_SYSTEMINFO_STORAGE_GET,
  DOWNLOADS_ACCEPTDANGER,
  DELETED_WEBSOCKETPROXYPRIVATE_GETURLFORTCP,
  FILEBROWSERPRIVATE_GETSIZESTATS,
  DOWNLOADS_RESUME,
  COOKIES_GETALLCOOKIESTORES,
  MEDIAGALLERIESPRIVATE_ADDGALLERYWATCH,
  MEDIAGALLERIESPRIVATE_REMOVEGALLERYWATCH,
  WEBVIEW_EXECUTESCRIPT,
  NOTIFICATIONS_UPDATE,
  NOTIFICATIONS_CLEAR,
  SESSIONRESTORE_GETRECENTLYCLOSED,
  SESSIONRESTORE_RESTORE,
  MANAGEMENT_UNINSTALLSELF,
  ECHOPRIVATE_GETOOBETIMESTAMP,
  FILEBROWSERPRIVATE_VALIDATEPATHNAMELENGTH,
  BROWSINGDATA_SETTINGS,
  WEBSTOREPRIVATE_GETISLAUNCHERENABLED,
  NETWORKINGPRIVATE_GETPROPERTIES,
  NETWORKINGPRIVATE_GETVISIBLENETWORKS,
  NETWORKINGPRIVATE_STARTCONNECT,
  NETWORKINGPRIVATE_STARTDISCONNECT,
  MEDIAGALLERIESPRIVATE_GETALLGALLERYWATCH,
  MEDIAGALLERIESPRIVATE_REMOVEALLGALLERYWATCH,
  FILEBROWSERPRIVATE_SEARCHDRIVEMETADATA,
  ECHOPRIVATE_CHECKALLOWREDEEMOFFERS,
  MEDIAGALLERIESPRIVATE_EJECTDEVICE,
  FILEBROWSERPRIVATE_LOGOUTUSER,
  DEVELOPERPRIVATE_CHOOSEPATH,
  DEVELOPERPRIVATE_PACKDIRECTORY,
  NETWORKINGPRIVATE_VERIFYDESTINATION,
  NETWORKINGPRIVATE_VERIFYANDENCRYPTCREDENTIALS,
  NETWORKINGPRIVATE_VERIFYANDENCRYPTDATA,
  DEVELOPERPRIVATE_RESTART,
  DEVELOPERPRIVATE_ALLOWINCOGNITO,
  INPUT_IME_DELETESURROUNDINGTEXT,
  DELETED_FILEBROWSERPRIVATE_OPENNEWWINDOW,
  CLOUDPRINTPRIVATE_GETCLIENTID,
  ECHOPRIVATE_GETUSERCONSENT,
  SYNCFILESYSTEM_SETCONFLICTRESOLUTIONPOLICY,
  SYNCFILESYSTEM_GETCONFLICTRESOLUTIONPOLICY,
  NETWORKINGPRIVATE_SETPROPERTIES,
  NETWORKINGPRIVATE_GETSTATE,
  POWER_REQUESTKEEPAWAKE,
  POWER_RELEASEKEEPAWAKE,
  WALLPAPERPRIVATE_SETCUSTOMWALLPAPERLAYOUT,
  DOWNLOADSINTERNAL_DETERMINEFILENAME,
  SYNCFILESYSTEM_GETFILESYNCSTATUSES,
  MEDIAGALLERIESPRIVATE_GETHANDLERS,
  WALLPAPERPRIVATE_RESETWALLPAPER,
  DEVELOPERPRIVATE_PERMISSIONS,
  WEBSTOREPRIVATE_ENABLEAPPLAUNCHER,
  APP_CURRENTWINDOWINTERNAL_FULLSCREEN,
  DEVELOPERPRIVATE_LOADUNPACKEDCROS,
  NETWORKINGPRIVATE_REQUESTNETWORKSCAN,
  ENTERPRISE_PLATFORMKEYSPRIVATE_CHALLENGEMACHINEKEY,
  ENTERPRISE_PLATFORMKEYSPRIVATE_CHALLENGEUSERKEY,
  DEVELOPERPRIVATE_LOADPROJECTTOSYNCFS,
  DEVELOPERPRIVATE_GETPROJECTSINFO,
  DEVELOPERPRIVATE_LOADPROJECT,
  COMMANDLINEPRIVATE_HASSWITCH,
  NETWORKINGPRIVATE_GETMANAGEDPROPERTIES,
  LOCATION_WATCHLOCATION,
  LOCATION_CLEARWATCH,
  BLUETOOTH_ADDPROFILE,
  BLUETOOTH_REMOVEPROFILE,
  BLUETOOTH_GETPROFILES,
  EXPERIMENTAL_IDENTITY_REMOVECACHEDAUTHTOKEN,
  AUDIO_GETINFO,
  AUDIO_SETACTIVEDEVICES,
  AUDIO_SETPROPERTIES,
  USB_RESETDEVICE,
  SOCKET_MULTICAST_JOIN_GROUP,
  SOCKET_MULTICAST_LEAVE_GROUP,
  SOCKET_MULTICAST_SET_TIME_TO_LIVE,
  SOCKET_MULTICAST_SET_LOOPBACK_MODE,
  SOCKET_MULTICAST_GET_JOINED_GROUPS,
  EXPERIMENTAL_ACCESSIBILITY_SETNATIVEACCESSIBILITYENABLED,
  RUNTIME_GETPLATFORMINFO,
  WEBVIEW_INSERTCSS,
  METRICSPRIVATE_GETISCRASHRECORDINGENABLED,
  IDENTITYPRIVATE_GETRESOURCES,
  NOTIFICATIONS_GET_ALL,
  USB_LISTINTERFACES,
  FILESYSTEM_RETAINENTRY,
  FILESYSTEM_ISRESTORABLE,
  FILESYSTEM_RESTOREENTRY,
  RUNTIME_SETUNINSTALLURL,
  INPUTMETHODPRIVATE_STARTIME,
  MUSICMANAGERPRIVATE_GETDEVICEID,
  TTS_PAUSE,
  TTS_RESUME,
  DELETED_SYSTEMINFO_CPU_GET,
  DELETED_EXPERIMENTAL_SYSTEMINFO_STORAGE_ADDWATCH,
  DELETED_EXPERIMENTAL_SYSTEMINFO_STORAGE_REMOVEWATCH,
  DELETED_EXPERIMENTAL_SYSTEMINFO_STORAGE_GETALLWATCH,
  DELETED_EXPERIMENTAL_SYSTEMINFO_STORAGE_REMOVEALLWATCH,
  DELETED_SYSTEMINFO_MEMORY_GET,
  ACTIVITYLOGPRIVATE_GETEXTENSIONACTIVITIES,
  RUNTIME_GETPACKAGEDIRECTORYENTRY,
  DELETED_SYSTEMINFO_DISPLAY_SETDISPLAYPROPERTIES,
  FEEDBACKPRIVATE_GETUSEREMAIL,
  FEEDBACKPRIVATE_GETSYSTEMINFORMATION,
  FEEDBACKPRIVATE_SENDFEEDBACK,
  EXPERIMENTAL_INPUT_VIRTUALKEYBOARD_INSERTTEXT,
  DIAGNOSTICS_SENDPACKET,
  METRICSPRIVATE_GETFIELDTRIAL,
  FILEBROWSERPRIVATE_ZOOM,
  WEBVIEW_GO,
  WEBSTOREPRIVATE_ISININCOGNITOMODEFUNCTION,
  FILEBROWSERPRIVATE_REQUESTACCESSTOKEN,
  WEBVIEW_STOP,
  WEBVIEW_RELOAD,
  WEBVIEW_TERMINATE,
  TYPES_PRIVATE_CHROMEDIRECTSETTING_GET,
  TYPES_PRIVATE_CHROMEDIRECTSETTING_SET,
  TYPES_PRIVATE_CHROMEDIRECTSETTING_CLEAR,
  DELETED_EXPERIMENTAL_SYSTEMINFO_STORAGE_EJECTDEVICE,
  SYSTEM_CPU_GETINFO,
  BOOKMARKMANAGERPRIVATE_REMOVETREES,
  SYSTEM_DISPLAY_GETINFO,
  SYSTEM_DISPLAY_SETDISPLAYPROPERTIES,
  SYSTEM_MEMORY_GETINFO,
  FILEBROWSERPRIVATE_GETSHAREURL,
  SYSTEM_STORAGE_GETINFO,
  SYSTEM_STORAGE_EJECTDEVICE,
  SYSTEM_STORAGE_ADDAVAILABLECAPACITYWATCH,
  SYSTEM_STORAGE_REMOVEAVAILABLECAPACITYWATCH,
  SYSTEM_STORAGE_GETALLAVAILABLECAPACITYWATCHES,
  SYSTEM_STORAGE_REMOVEALLAVAILABLECAPACITYWATCHES,
  DOWNLOADS_REMOVEFILE,
  DOWNLOADS_SHOWDEFAULTFOLDER,
  INFOBARS_SHOW,
  DOWNLOADS_SETSHELFENABLED,
  RECOVERYPRIVATE_WRITEFROMURL,
  RECOVERYPRIVATE_WRITEFROMFILE,
  RECOVERYPRIVATE_CANCELWRITE,
  RECOVERYPRIVATE_DESTROYPARTITIONS,
  FEEDBACKPRIVATE_GETSTRINGS,
  ENUM_BOUNDARY // Last entry: Add new entries above.
};

}  // namespace functions
}  // namespace extensions

#endif  // CHROME_BROWSER_EXTENSIONS_EXTENSION_FUNCTION_HISTOGRAM_VALUE_H_
