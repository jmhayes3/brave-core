// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "base/macros.h"
#include "brave/components/brave_sync/brave_sync_prefs.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/autofill/payments/save_card_bubble_controller_impl.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/ui_test_utils.h"
#include "components/prefs/pref_service.h"
#include "components/sync/base/sync_prefs.h"
#include "testing/gtest/include/gtest/gtest.h"

class BraveSaveCardBubbleControllerImplTest : public InProcessBrowserTest {
 public:
  BraveSaveCardBubbleControllerImplTest() {}

  void ShowUi() {
    content::WebContents* web_contents =
        browser()->tab_strip_model()->GetActiveWebContents();

    // Do lazy initialization of SaveCardBubbleControllerImpl.
    autofill::SaveCardBubbleControllerImpl::CreateForWebContents(web_contents);
    controller_ =
        autofill::SaveCardBubbleControllerImpl::FromWebContents(web_contents);
    DCHECK(controller_);
    controller_->ShowBubbleForSignInPromo();
  }

  void EnableBraveSync(PrefService* pref) {
    pref->SetBoolean(brave_sync::prefs::kSyncEnabled, true);
    syncer::SyncPrefs sync_prefs(pref);
    sync_prefs.SetSyncRequested(true);
    pref->SetInteger(brave_sync::prefs::kSyncMigrateBookmarksVersion, 2);
  }

  void DisableBraveSync(PrefService* pref) {
    pref->SetBoolean(brave_sync::prefs::kSyncEnabled, false);
    syncer::SyncPrefs sync_prefs(pref);
    sync_prefs.SetSyncRequested(false);
  }

  autofill::SaveCardBubbleControllerImpl* controller() { return controller_; }

 private:
  autofill::SaveCardBubbleControllerImpl* controller_ = nullptr;

  DISALLOW_COPY_AND_ASSIGN(BraveSaveCardBubbleControllerImplTest);
};

// Tests that requesting to open signin promo bubble doesn't result in the
// bubble being shown.
IN_PROC_BROWSER_TEST_F(BraveSaveCardBubbleControllerImplTest,
                       NoSignInPromoBraveSyncDisabled) {
  ShowUi();
  EXPECT_EQ(nullptr, controller()->save_card_bubble_view());
}

IN_PROC_BROWSER_TEST_F(BraveSaveCardBubbleControllerImplTest,
                       NoSignInPromoBraveSyncEnabled) {
  EnableBraveSync(browser()->profile()->GetPrefs());
  ShowUi();
  EXPECT_EQ(nullptr, controller()->save_card_bubble_view());
  DisableBraveSync(browser()->profile()->GetPrefs());
  ShowUi();
  EXPECT_EQ(nullptr, controller()->save_card_bubble_view());
}
