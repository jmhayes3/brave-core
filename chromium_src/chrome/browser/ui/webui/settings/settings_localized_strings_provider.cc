/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/webui/settings/settings_localized_strings_provider.h"
#include "base/stl_util.h"

namespace settings {
void BraveAddLocalizedStrings(content::WebUIDataSource*, Profile*);
}  // namespace settings

// Override some chromium strings
#include "brave/grit/brave_generated_resources.h"
#include "chrome/grit/chromium_strings.h"
#include "chrome/grit/generated_resources.h"
#include "extensions/common/extension_urls.h"

#undef IDS_SETTINGS_EDIT_PERSON
#define IDS_SETTINGS_EDIT_PERSON IDS_SETTINGS_BRAVE_EDIT_PROFILE
#undef IDS_SETTINGS_PROFILE_NAME_AND_PICTURE
#define IDS_SETTINGS_PROFILE_NAME_AND_PICTURE IDS_SETTINGS_BRAVE_EDIT_PROFILE

#include "../../../../../../chrome/browser/ui/webui/settings/settings_localized_strings_provider.cc"  // NOLINT

#include "brave/browser/ui/webui/brave_settings_ui.h"
namespace settings {

void BraveAddImportDataStrings(content::WebUIDataSource* html_source) {
  LocalizedString localized_strings[] = {
    {"importCookies", IDS_SETTINGS_IMPORT_COOKIES_CHECKBOX},
    {"importStats", IDS_SETTINGS_IMPORT_STATS_CHECKBOX},
    {"importLedger", IDS_SETTINGS_IMPORT_LEDGER_CHECKBOX},
    {"importWindows", IDS_SETTINGS_IMPORT_WINDOWS_CHECKBOX},
  };
  AddLocalizedStringsBulk(html_source, localized_strings,
                          base::size(localized_strings));
}

const char kWebRTCLearnMoreURL[] =
  "https://support.brave.com/hc/en-us/articles/"
  "360017989132-How-do-I-change-my-Privacy-Settings-#webrtc";

void BraveAddCommonStrings(content::WebUIDataSource* html_source,
                           Profile* profile) {
  LocalizedString localized_strings[] = {
    {"siteSettingsAutoplay",
      IDS_SETTINGS_SITE_SETTINGS_AUTOPLAY},
    {"siteSettingsCategoryAutoplay",
      IDS_SETTINGS_SITE_SETTINGS_AUTOPLAY},
    {"siteSettingsAutoplayAsk",
      IDS_SETTINGS_SITE_SETTINGS_AUTOPLAY_ASK},
    {"siteSettingsAutoplayAskRecommended",
      IDS_SETTINGS_SITE_SETTINGS_AUTOPLAY_ASK_RECOMMENDED},
    {"braveGetStartedTitle",
      IDS_SETTINGS_BRAVE_GET_STARTED_TITLE},
    {"braveAdditionalSettingsTitle",
      IDS_SETTINGS_BRAVE_ADDITIONAL_SETTINGS},
    {"appearanceSettingsAskWidevineInstallDesc",
      IDS_SETTINGS_ASK_WIDEVINE_INSTALL_DESC},
    {"appearanceSettingsBraveTheme",
      IDS_SETTINGS_APPEARANCE_SETTINGS_BRAVE_THEMES},
    {"appearanceSettingsLocationBarIsWide",
      IDS_SETTINGS_APPEARANCE_SETTINGS_LOCATION_BAR_IS_WIDE},
    {"appearanceSettingsHideBraveRewardsButtonLabel",
       IDS_SETTINGS_HIDE_BRAVE_REWARDS_BUTTON_LABEL},
    {"appearanceSettingsHideBraveRewardsButtonDesc",
       IDS_SETTINGS_HIDE_BRAVE_REWARDS_BUTTON_DESC},
    {"appearanceSettingsAlwaysShowBookmarkBarOnNTP",
       IDS_SETTINGS_ALWAYS_SHOW_BOOKMARK_BAR_ON_NTP},
    {"braveShieldsTitle",
      IDS_SETTINGS_BRAVE_SHIELDS_TITLE},
    {"braveShieldsDefaultsSectionTitle",
      IDS_SETTINGS_BRAVE_SHIELDS_DEFAULTS_TITLE},
    {"braveShieldsDefaultsSectionDescription1",
      IDS_SETTINGS_BRAVE_SHIELDS_DEFAULTS_DESCRIPTION_1},
    {"braveShieldsDefaultsSectionDescription2",
      IDS_SETTINGS_BRAVE_SHIELDS_DEFAULTS_DESCRIPTION_2},
    {"braveShieldsDefaultsSectionDescription3",
      IDS_SETTINGS_BRAVE_SHIELDS_DEFAULTS_DESCRIPTION_3},
    {"socialBlocking",
      IDS_SETTINGS_SOCIAL_BLOCKING_DEFAULTS_TITLE},
    {"defaultView",
      IDS_SETTINGS_BRAVE_SHIELDS_DEFAULTS_DEFAULT_VIEW_LABEL},
    {"simpleView",
      IDS_SETTINGS_BRAVE_SHIELDS_DEFAULTS_SIMPLE_VIEW_LABEL},
    {"advancedView",
      IDS_SETTINGS_BRAVE_SHIELDS_DEFAULTS_ADVANCED_VIEW_LABEL},
    {"adControlLabel",
      IDS_SETTINGS_BRAVE_SHIELDS_AD_CONTROL_LABEL},
    {"cookieControlLabel",
      IDS_SETTINGS_BRAVE_SHIELDS_COOKIE_CONTROL_LABEL},
    {"fingerprintingControlLabel",
      IDS_SETTINGS_BRAVE_SHIELDS_FINGERPRINTING_CONTROL_LABEL},
    {"httpsEverywhereControlLabel",
      IDS_SETTINGS_BRAVE_SHIELDS_HTTPS_EVERYWHERE_CONTROL_LABEL},
    {"noScriptControlLabel",
      IDS_SETTINGS_BRAVE_SHIELDS_NO_SCRIPT_CONTROL_LABEL},
    {"fbEmbedControlLabel",
      IDS_SETTINGS_BRAVE_SHIELDS_FACEBOOK_EMBEDDED_POSTS_LABEL},
    {"twitterEmbedControlLabel",
      IDS_SETTINGS_BRAVE_SHIELDS_TWITTER_EMBEDDED_TWEETS_LABEL},
    {"linkedInEmbedControlLabel",
      IDS_SETTINGS_BRAVE_SHIELDS_LINKEDIN_EMBEDDED_POSTS_LABEL},
    {"block3rdPartyCookies",
      IDS_SETTINGS_BLOCK_3RD_PARTY_COOKIES},
    {"allowAllCookies",
      IDS_SETTINGS_ALLOW_ALL_COOKIES},
    {"blockAllCookies",
      IDS_SETTINGS_BLOCK_ALL_COOKIES},
    {"block3rdPartyFingerprinting",
      IDS_SETTINGS_BLOCK_3RD_PARTY_FINGERPRINTING},
    {"allowAllFingerprinting",
      IDS_SETTINGS_ALLOW_FINGERPRINTING},
    {"blockAllFingerprinting",
      IDS_SETTINGS_BLOCK_FINGERPRINTING},
    {"webRTCPolicyLabel",
      IDS_SETTINGS_WEBRTC_POLICY_LABEL},
    {"webRTCPolicySubLabel",
      IDS_SETTINGS_WEBRTC_POLICY_SUB_LABEL},
    {"webRTCDefault",
      IDS_SETTINGS_WEBRTC_POLICY_DEFAULT},
    {"defaultPublicAndPrivateInterfaces",
      IDS_SETTINGS_WEBRTC_POLICY_DEFAULT_PUBLIC_AND_PRIVATE_INTERFACES},
    {"defaultPublicInterfaceOnly",
      IDS_SETTINGS_WEBRTC_POLICY_DEFAULT_PUBLIC_INTERFACE_ONLY},
    {"disableNonProxiedUdp",
      IDS_SETTINGS_WEBRTC_POLICY_DISABLE_NON_PROXIED_UDP},
    {"braveSync",
      IDS_SETTINGS_BRAVE_SYNC_TITLE},
    {"braveSyncLabel",
      IDS_SETTINGS_BRAVE_SYNC_LINK_LABEL},
    {"onExitPageTitle",
      IDS_SETTINGS_BRAVE_ON_EXIT},
    {"braveDefaultExtensions",
      IDS_SETTINGS_BRAVE_DEFAULT_EXTENSIONS_TITLE},
    {"webTorrentEnabledDesc",
      IDS_SETTINGS_WEBTORRENT_ENABLED_DESC},
    {"braveWalletEnabledDesc",
      IDS_SETTINGS_BRAVE_WALLET_ENABLED_DESC},
    {"hangoutsEnabledDesc",
      IDS_SETTINGS_HANGOUTS_ENABLED_DESC},
    {"ipfsCompanionEnabledDesc",
      IDS_SETTINGS_IPFS_COMPANION_ENABLED_DESC},
    {"mediaRouterEnabledDesc",
      IDS_SETTINGS_MEDIA_ROUTER_ENABLED_DESC},
    {"restartNotice",
      IDS_SETTINGS_RESTART_NOTICE},
    {"relaunchButtonLabel",
      IDS_SETTINGS_RELAUNCH_BUTTON_LABEL},
    {"manageExtensionsLabel",
      IDS_SETTINGS_MANAGE_EXTENSIONS_LABEL},
    {"getMoreExtensionsLabel",
      IDS_BRAVE_SETTINGS_GET_MORE_EXTENSIONS_LABEL},
    {"getMoreExtensionsSubLabel",
      IDS_EXTENSIONS_SIDEBAR_OPEN_CHROME_WEB_STORE}
  };
  AddLocalizedStringsBulk(html_source, localized_strings,
                          base::size(localized_strings));
  html_source->AddString("webRTCLearnMoreURL",
      base::ASCIIToUTF16(kWebRTCLearnMoreURL));
  html_source->AddString("getMoreExtensionsUrl",
    base::ASCIIToUTF16(
      google_util::AppendGoogleLocaleParam(
                GURL(extension_urls::GetWebstoreExtensionsCategoryURL()),
                g_browser_process->GetApplicationLocale())
                .spec()));
}

void BraveAddResources(content::WebUIDataSource* html_source,
                       Profile* profile) {
  BraveSettingsUI::AddResources(html_source, profile);
}

void BraveAddLocalizedStrings(content::WebUIDataSource* html_source,
                              Profile* profile) {
  BraveAddImportDataStrings(html_source);
  BraveAddCommonStrings(html_source, profile);
  BraveAddResources(html_source, profile);
}

}  // namespace settings
