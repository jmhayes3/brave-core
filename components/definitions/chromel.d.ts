/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

declare namespace chrome {
  function getVariableValue (variable: string): string
  function setVariableValue (variable: string, value: any): void
  function send (stat: string, args?: any[]): void
}

declare namespace chrome.dns {
  function resolve (hostname: string, callback: any): void
}

declare namespace chrome.settingsPrivate {
  // See chromium definition at
  // https://chromium.googlesource.com/chromium/src.git/+/master/chrome/common/extensions/api/settings_private.idl
  enum PrefType {
    BOOLEAN = 'BOOLEAN',
    NUMBER = 'NUMBER',
    STRING = 'STRING',
    URL = 'URL',
    LIST = 'LIST',
    DICTIONARY = 'DICTIONARY'
  }

  type PrefBooleanValue = {
    type: PrefType.BOOLEAN,
    value: boolean
  }
  type SettingsNumberValue = {
    type: PrefType.NUMBER,
    value: number
  }
  type SettingsStringValue = {
    type: PrefType.STRING,
    value: string
  }
  // TODO(petemill): implement other types as needed

  type PrefObject = {
    key: string
  } & (PrefBooleanValue)

  type GetPrefCallback = (pref: PrefObject) => void
  function getPref (key: string, callback: GetPrefCallback): void

  type SetPrefCallback = (success: boolean) => void
  function setPref (key: string, value: any, pageId?: string | null, callback?: SetPrefCallback): void

  type GetAllPrefsCallback = (prefs: PrefObject[]) => void
  function getAllPrefs (callback: GetAllPrefsCallback): void

  type GetDefaultZoomCallback = (zoom: number) => void
  function getDefaultZoom (callback: GetDefaultZoomCallback): void

  type SetDefaultZoomCallback = (success: boolean) => void
  function setDefaultZoom (zoom: number, callback?: SetDefaultZoomCallback): void
}

declare namespace chrome.braveRewards {
  const createWallet: () => {}
  const tipSite: (tabId: number, publisherKey: string) => {}
  const tipTwitterUser: (tabId: number, mediaMetaData: RewardsTip.MediaMetaData) => {}
  const tipRedditUser: (tabId: number, mediaMetaData: RewardsTip.MediaMetaData) => {}
  const tipGitHubUser: (tabId: number, githubMetaData: RewardsTip.MediaMetaData) => {}
  const getPublisherData: (windowId: number, url: string, faviconUrl: string, publisherBlob: string | undefined) => {}
  const getWalletProperties: () => {}
  const getCurrentReport: () => {}
  const onWalletInitialized: {
    addListener: (callback: (result: RewardsExtension.Result) => void) => void
  }
  const onPublisherData: {
    addListener: (callback: (windowId: number, publisher: RewardsExtension.Publisher) => void) => void
  }
  const onWalletProperties: {
    addListener: (callback: (properties: RewardsExtension.WalletProperties) => void) => void
  }
  const onCurrentReport: {
    addListener: (callback: (properties: RewardsExtension.Report) => void) => void
  }
  const onGrant: {
    addListener: (callback: (properties: RewardsExtension.GrantResponse) => void) => void
  }
  const onGrantFinish: {
    addListener: (callback: (properties: RewardsExtension.GrantFinish) => void) => void
  }
  const onGrantCaptcha: {
    addListener: (callback: (properties: RewardsExtension.Captcha) => void) => void
  }
  const includeInAutoContribution: (publisherKey: string, exclude: boolean) => {}
  const getGrants: () => {}
  const getGrantCaptcha: (promotionId: string, type: string) => {}
  const solveGrantCaptcha: (solution: string, promotionId: string) => {}
  const getPendingContributionsTotal: (callback: (amount: number) => void) => {}
  const getNonVerifiedSettings: (callback: (nonVerified: boolean) => void) => {}
  const onEnabledMain: {
    addListener: (callback: (enabledMain: boolean) => void) => void
  }
  const getRewardsMainEnabled: (callback: (enabled: boolean) => void) => {}
  const saveAdsSetting: (key: string, value: string) => {}
  const onPendingContributionSaved: {
    addListener: (callback: (result: number) => void) => void
  }
  const getACEnabled: (callback: (enabled: boolean) => void) => {}
  const onPublisherListNormalized: {
    addListener: (callback: (properties: RewardsExtension.PublisherNormalized[]) => void) => void
  }
  const onExcludedSitesChanged: {
    addListener: (callback: (properties: RewardsExtension.ExcludedSitesChanged) => void) => void
  }
  const saveSetting: (key: string, value: string) => {}
  const getRecurringTips: (callback: (tips: RewardsExtension.RecurringTips) => void) => {}
  const saveRecurringTip: (publisherKey: string, newAmount: string) => {}
  const removeRecurringTip: (publisherKey: string) => {}
  const getPublisherBanner: (publisherKey: string, callback: (banner: RewardsExtension.PublisherBanner) => void) => {}
  const onRecurringTipSaved: {
    addListener: (callback: (success: boolean) => void) => void
  }
  const onRecurringTipRemoved: {
    addListener: (callback: (success: boolean) => void) => void
  }
  const refreshPublisher: (publisherKey: string, callback: (status: number, publisherKey: string) => void) => {}
  const getAllNotifications: (callback: (list: RewardsExtension.Notification[]) => void) => {}
  const getInlineTipSetting: (key: string, callback: (enabled: boolean) => void) => {}
  const fetchBalance: (callback: (balance: RewardsExtension.Balance) => void) => {}
  const onReconcileComplete: {
    addListener: (callback: (result: number, category: number) => void) => void
  }

  const getExternalWallet: (type: string, callback: (result: number, wallet: RewardsExtension.ExternalWallet) => void) => {}

  const disconnectWallet: (type: string) => {}

  const onDisconnectWallet: {
    addListener: (callback: (properties: {result: number, walletType: string}) => void) => void
  }

  const onlyAnonWallet: (callback: (only: boolean) => void) => {}
}

declare namespace chrome.rewardsNotifications {
  const addNotification: (type: number, args: string[], id: string) => {}
  const deleteNotification: (id: string) => {}
  const deleteAllNotifications: () => {}
  const getNotification: (id: string) => {}

  const onNotificationAdded: {
    addListener: (callback: (id: string, type: number, timestamp: number, args: string[]) => void) => void
  }
  const onNotificationDeleted: {
    addListener: (callback: (id: string, type: number, timestamp: number) => void) => void
  }
  const onAllNotificationsDeleted: {
    addListener: (callback: () => void) => void
  }
  const onGetNotification: {
    addListener: (callback: (id: string, type: number, timestamp: number) => void) => void
  }
}

type BlockTypes = 'ads' | 'trackers' | 'httpUpgradableResources' | 'javascript' | 'fingerprinting'

interface BlockDetails {
  blockType: BlockTypes
  tabId: number
  subresource: string
}

interface BlockDetails {
  blockType: BlockTypes
  tabId: number
  subresource: string
}
declare namespace chrome.tabs {
  const setAsync: any
  const getAsync: any
}

declare namespace chrome.windows {
  const getAllAsync: any
}

declare namespace chrome.braveShields {
  const onBlocked: {
    addListener: (callback: (detail: BlockDetails) => void) => void
    emit: (detail: BlockDetails) => void
  }

  const allowScriptsOnce: any
  const setBraveShieldsEnabledAsync: any
  const getBraveShieldsEnabledAsync: any
  const setAdControlTypeAsync: any
  const getAdControlTypeAsync: any
  const setCookieControlTypeAsync: any
  const getCookieControlTypeAsync: any
  const setFingerprintingControlTypeAsync: any
  const getFingerprintingControlTypeAsync: any
  const setHTTPSEverywhereEnabledAsync: any
  const getHTTPSEverywhereEnabledAsync: any
  const setNoScriptControlTypeAsync: any
  const getNoScriptControlTypeAsync: any

  type BraveShieldsViewPreferences = {
    showAdvancedView: boolean
  }
  type BraveShieldsSetViewPreferencesData = {
    showAdvancedView?: boolean
  }
}

declare namespace chrome.braveWallet {
  const promptToEnableWallet: (tabId: number | undefined) => void
  const isEnabled: (callback: (enabled: boolean) => void) => void
}

declare namespace chrome.test {
  const sendMessage: (message: string) => {}
}

declare namespace chrome.braveTheme {
  type ThemeType = 'Light' | 'Dark' | 'System'
  type ThemeList = Array<{name: ThemeType, index: number}>
  type ThemeTypeCallback = (themeType: ThemeType) => void
  type ThemeListCallback = (themeList: ThemeList) => void
  const getBraveThemeType: (themeType: ThemeTypeCallback) => void
  const getBraveThemeList: (themeList: ThemeListCallback) => void
  const setBraveThemeType: (themeType: ThemeType) => void
  const onBraveThemeTypeChanged: {
    addListener: (callback: ThemeTypeCallback) => void
  }
}
