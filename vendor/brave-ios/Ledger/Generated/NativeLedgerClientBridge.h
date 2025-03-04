/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#import <Foundation/Foundation.h>
#import "bat/ledger/ledger_client.h"

@protocol NativeLedgerClientBridge
@required

- (void)confirmationsTransactionHistoryDidChange;
- (void)fetchFavIcon:(const std::string &)url faviconKey:(const std::string &)favicon_key callback:(ledger::FetchIconCallback)callback;
- (std::string)generateGUID;
- (void)getActivityInfoList:(uint32_t)start limit:(uint32_t)limit filter:(ledger::ActivityInfoFilterPtr)filter callback:(ledger::PublisherInfoListCallback)callback;
- (void)getOneTimeTips:(ledger::PublisherInfoListCallback)callback;
- (void)getPendingContributions:(ledger::PendingContributionInfoListCallback)callback;
- (void)getPendingContributionsTotal:(ledger::PendingContributionsTotalCallback)callback;
- (void)saveRecurringTip:(ledger::ContributionInfoPtr)info callback:(ledger::SaveRecurringTipCallback)callback;
- (void)getRecurringTips:(ledger::PublisherInfoListCallback)callback;
- (void)killTimer:(const uint32_t)timer_id;
- (void)loadActivityInfo:(ledger::ActivityInfoFilterPtr)filter callback:(ledger::PublisherInfoCallback)callback;
- (void)loadLedgerState:(ledger::OnLoadCallback)callback;
- (void)loadMediaPublisherInfo:(const std::string &)media_key callback:(ledger::PublisherInfoCallback)callback;
- (void)loadNicewareList:(ledger::GetNicewareListCallback)callback;
- (void)loadPanelPublisherInfo:(ledger::ActivityInfoFilterPtr)filter callback:(ledger::PublisherInfoCallback)callback;
- (void)loadPublisherInfo:(const std::string &)publisher_key callback:(ledger::PublisherInfoCallback)callback;
- (void)loadPublisherState:(ledger::OnLoadCallback)callback;
- (void)loadState:(const std::string &)name callback:(ledger::OnLoadCallback)callback;
- (void)loadURL:(const std::string &)url headers:(const std::vector<std::string> &)headers content:(const std::string &)content contentType:(const std::string &)contentType method:(const ledger::URL_METHOD)method callback:(ledger::LoadURLCallback)callback;
- (std::unique_ptr<ledger::LogStream>)log:(const char *)file line:(int)line logLevel:(const ledger::LogLevel)log_level;
- (void)onGrantFinish:(ledger::Result)result grant:(ledger::GrantPtr)grant;
- (void)onPanelPublisherInfo:(ledger::Result)result publisherInfo:(ledger::PublisherInfoPtr)publisher_info windowId:(uint64_t)windowId;
- (void)onReconcileComplete:(ledger::Result)result viewingId:(const std::string &)viewing_id category:(ledger::REWARDS_CATEGORY)category probi:(const std::string &)probi;
- (void)removeRecurringTip:(const std::string &)publisher_key callback:(ledger::RemoveRecurringTipCallback)callback;
- (void)restorePublishers:(ledger::RestorePublishersCallback)callback;
- (void)onWalletProperties:(ledger::Result)result arg1:(ledger::WalletPropertiesPtr)arg1;
- (void)removeAllPendingContributions:(ledger::RemovePendingContributionCallback)callback;
- (void)removePendingContribution:(const std::string &)publisher_key viewingId:(const std::string &)viewing_id addedDate:(uint64_t)added_date callback:(ledger::RemovePendingContributionCallback )callback;
- (void)resetState:(const std::string &)name callback:(ledger::OnResetCallback)callback;
- (void)saveActivityInfo:(ledger::PublisherInfoPtr)publisher_info callback:(ledger::PublisherInfoCallback)callback;
- (void)saveContributionInfo:(const std::string &)probi month:(const int)month year:(const int)year date:(const uint32_t)date publisherKey:(const std::string &)publisher_key category:(const ledger::REWARDS_CATEGORY)category;
- (void)saveLedgerState:(const std::string &)ledger_state handler:(ledger::LedgerCallbackHandler *)handler;
- (void)saveMediaPublisherInfo:(const std::string &)media_key publisherId:(const std::string &)publisher_id;
- (void)saveNormalizedPublisherList:(ledger::PublisherInfoList)normalized_list;
- (void)savePendingContribution:(ledger::PendingContributionList)list callback:(ledger::SavePendingContributionCallback)callback;
- (void)savePublisherInfo:(ledger::PublisherInfoPtr)publisher_info callback:(ledger::PublisherInfoCallback)callback;
- (void)savePublisherState:(const std::string &)publisher_state handler:(ledger::LedgerCallbackHandler *)handler;
- (void)saveState:(const std::string &)name value:(const std::string &)value callback:(ledger::OnSaveCallback)callback;
- (void)setConfirmationsIsReady:(const bool)is_ready;
- (void)setTimer:(uint64_t)time_offset timerId:(uint32_t *)timer_id;
- (std::string)URIEncode:(const std::string &)value;
- (std::unique_ptr<ledger::LogStream>)verboseLog:(const char *)file line:(int)line vlogLevel:(int)vlog_level;
- (void)onContributeUnverifiedPublishers:(ledger::Result)result publisherKey:(const std::string&)publisher_key publisherName:(const std::string&)publisher_name;
- (void)setBooleanState:(const std::string&)name value:(bool)value;
- (bool)getBooleanState:(const std::string&)name;
- (void)setIntegerState:(const std::string&)name value:(int)value;
- (int)getIntegerState:(const std::string&)name;
- (void)setDoubleState:(const std::string&)name value:(double)value;
- (double)getDoubleState:(const std::string&)name;
- (void)setStringState:(const std::string&)name value:(const std::string&)value;
- (std::string)getStringState:(const std::string&)name;
- (void)setInt64State:(const std::string&)name value:(int64_t)value;
- (int64_t)getInt64State:(const std::string&)name;
- (void)setUint64State:(const std::string&)name value:(uint64_t)value;
- (uint64_t)getUint64State:(const std::string&)name;
- (void)clearState:(const std::string&)name;
- (void)getExternalWallets:(ledger::GetExternalWalletsCallback)callback;
- (void)saveExternalWallet:(const std::string &)wallet_type wallet:(ledger::ExternalWalletPtr)wallet;
- (void)showNotification:(const std::string &)type args:(const std::vector<std::string>&)args callback:(ledger::ShowNotificationCallback)callback;
- (void)deleteActivityInfo:(const std::string&)publisher_key callback:(ledger::DeleteActivityInfoCallback)callback;
- (void)clearAndInsertServerPublisherList:(ledger::ServerPublisherInfoList)list callback:(ledger::ClearAndInsertServerPublisherListCallback)callback;
- (void)getServerPublisherInfo:(const std::string&)publisher_key callback:(ledger::GetServerPublisherInfoCallback) callback;
- (void)setTransferFee:(const std::string&)wallet_type transfer_fee:(ledger::TransferFeePtr)transfer_fee;
- (void)removeTransferFee:(const std::string&)wallet_type id:(const std::string&)id;
- (ledger::TransferFeeList)getTransferFees:(const std::string&)wallet_type;

@end
