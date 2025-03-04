/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#import "NativeLedgerClient.h"
#import "NativeLedgerClientBridge.h"

// Constructor & Destructor
NativeLedgerClient::NativeLedgerClient(id<NativeLedgerClientBridge> bridge) : bridge_(bridge) { }
NativeLedgerClient::~NativeLedgerClient() {
  bridge_ = nil;
}

void NativeLedgerClient::ConfirmationsTransactionHistoryDidChange() {
  [bridge_ confirmationsTransactionHistoryDidChange];
}
void NativeLedgerClient::FetchFavIcon(const std::string & url, const std::string & favicon_key, ledger::FetchIconCallback callback) {
  [bridge_ fetchFavIcon:url faviconKey:favicon_key callback:callback];
}
std::string NativeLedgerClient::GenerateGUID() const {
  return [bridge_ generateGUID];
}
void NativeLedgerClient::GetActivityInfoList(uint32_t start, uint32_t limit, ledger::ActivityInfoFilterPtr filter, ledger::PublisherInfoListCallback callback) {
  [bridge_ getActivityInfoList:start limit:limit filter:std::move(filter) callback:callback];
}
void NativeLedgerClient::GetOneTimeTips(ledger::PublisherInfoListCallback callback) {
  [bridge_ getOneTimeTips:callback];
}
void NativeLedgerClient::GetPendingContributions(ledger::PendingContributionInfoListCallback callback) {
  [bridge_ getPendingContributions:callback];
}
void NativeLedgerClient::GetPendingContributionsTotal(ledger::PendingContributionsTotalCallback callback) {
  [bridge_ getPendingContributionsTotal:callback];
}
void NativeLedgerClient::SaveRecurringTip(ledger::ContributionInfoPtr info, ledger::SaveRecurringTipCallback callback) {
  [bridge_ saveRecurringTip:std::move(info) callback:callback];
}
void NativeLedgerClient::GetRecurringTips(ledger::PublisherInfoListCallback callback) {
  [bridge_ getRecurringTips:callback];
}
void NativeLedgerClient::KillTimer(const uint32_t timer_id) {
  [bridge_ killTimer:timer_id];
}
void NativeLedgerClient::LoadActivityInfo(ledger::ActivityInfoFilterPtr filter, ledger::PublisherInfoCallback callback) {
  [bridge_ loadActivityInfo:std::move(filter) callback:callback];
}
void NativeLedgerClient::LoadLedgerState(ledger::OnLoadCallback callback) {
  [bridge_ loadLedgerState:callback];
}
void NativeLedgerClient::LoadMediaPublisherInfo(const std::string & media_key, ledger::PublisherInfoCallback callback) {
  [bridge_ loadMediaPublisherInfo:media_key callback:callback];
}
void NativeLedgerClient::LoadNicewareList(ledger::GetNicewareListCallback callback) {
  [bridge_ loadNicewareList:callback];
}
void NativeLedgerClient::LoadPanelPublisherInfo(ledger::ActivityInfoFilterPtr filter, ledger::PublisherInfoCallback callback) {
  [bridge_ loadPanelPublisherInfo:std::move(filter) callback:callback];
}
void NativeLedgerClient::LoadPublisherInfo(const std::string & publisher_key, ledger::PublisherInfoCallback callback) {
  [bridge_ loadPublisherInfo:publisher_key callback:callback];
}
void NativeLedgerClient::LoadPublisherState(ledger::OnLoadCallback callback) {
  [bridge_ loadPublisherState:callback];
}
void NativeLedgerClient::LoadState(const std::string & name, ledger::OnLoadCallback callback) {
  [bridge_ loadState:name callback:callback];
}
void NativeLedgerClient::LoadURL(const std::string & url, const std::vector<std::string> & headers, const std::string & content, const std::string & contentType, const ledger::URL_METHOD method, ledger::LoadURLCallback callback) {
  [bridge_ loadURL:url headers:headers content:content contentType:contentType method:method callback:callback];
}
std::unique_ptr<ledger::LogStream> NativeLedgerClient::Log(const char * file, int line, const ledger::LogLevel log_level) const {
  return [bridge_ log:file line:line logLevel:log_level];
}
void NativeLedgerClient::OnGrantFinish(ledger::Result result, ledger::GrantPtr grant) {
  [bridge_ onGrantFinish:result grant:std::move(grant)];
}
void NativeLedgerClient::OnPanelPublisherInfo(ledger::Result result, ledger::PublisherInfoPtr publisher_info, uint64_t windowId) {
  [bridge_ onPanelPublisherInfo:result publisherInfo:std::move(publisher_info) windowId:windowId];
}
void NativeLedgerClient::OnReconcileComplete(ledger::Result result, const std::string & viewing_id, ledger::REWARDS_CATEGORY category, const std::string & probi) {
  [bridge_ onReconcileComplete:result viewingId:viewing_id category:category probi:probi];
}
void NativeLedgerClient::RemoveRecurringTip(const std::string & publisher_key, ledger::RemoveRecurringTipCallback callback) {
  [bridge_ removeRecurringTip:publisher_key callback:callback];
}
void NativeLedgerClient::RestorePublishers(ledger::RestorePublishersCallback callback) {
  [bridge_ restorePublishers:callback];
}
void NativeLedgerClient::OnWalletProperties(ledger::Result result, ledger::WalletPropertiesPtr arg1) {
  [bridge_ onWalletProperties:result arg1:std::move(arg1)];
}
void NativeLedgerClient::RemoveAllPendingContributions(ledger::RemovePendingContributionCallback callback) {
  [bridge_ removeAllPendingContributions:callback];
}
void NativeLedgerClient::RemovePendingContribution(const std::string & publisher_key, const std::string & viewing_id, uint64_t added_date, ledger::RemovePendingContributionCallback callback) {
  [bridge_ removePendingContribution:publisher_key viewingId:viewing_id addedDate:added_date callback:callback];
}
void NativeLedgerClient::ResetState(const std::string & name, ledger::OnResetCallback callback) {
  [bridge_ resetState:name callback:callback];
}
void NativeLedgerClient::SaveActivityInfo(ledger::PublisherInfoPtr publisher_info, ledger::PublisherInfoCallback callback) {
  [bridge_ saveActivityInfo:std::move(publisher_info) callback:callback];
}
void NativeLedgerClient::SaveContributionInfo(const std::string & probi, const int month, const int year, const uint32_t date, const std::string & publisher_key, const ledger::REWARDS_CATEGORY category) {
  [bridge_ saveContributionInfo:probi month:month year:year date:date publisherKey:publisher_key category:category];
}
void NativeLedgerClient::SaveLedgerState(const std::string & ledger_state, ledger::LedgerCallbackHandler * handler) {
  [bridge_ saveLedgerState:ledger_state handler:handler];
}
void NativeLedgerClient::SaveMediaPublisherInfo(const std::string & media_key, const std::string & publisher_id) {
  [bridge_ saveMediaPublisherInfo:media_key publisherId:publisher_id];
}
void NativeLedgerClient::SaveNormalizedPublisherList(ledger::PublisherInfoList normalized_list) {
  [bridge_ saveNormalizedPublisherList:std::move(normalized_list)];
}
void NativeLedgerClient::SavePendingContribution(ledger::PendingContributionList list, ledger::SavePendingContributionCallback callback) {
  [bridge_ savePendingContribution:std::move(list) callback:callback];
}
void NativeLedgerClient::SavePublisherInfo(ledger::PublisherInfoPtr publisher_info, ledger::PublisherInfoCallback callback) {
  [bridge_ savePublisherInfo:std::move(publisher_info) callback:callback];
}
void NativeLedgerClient::SavePublisherState(const std::string & publisher_state, ledger::LedgerCallbackHandler * handler) {
  [bridge_ savePublisherState:publisher_state handler:handler];
}
void NativeLedgerClient::SaveState(const std::string & name, const std::string & value, ledger::OnSaveCallback callback) {
  [bridge_ saveState:name value:value callback:callback];
}
void NativeLedgerClient::SetConfirmationsIsReady(const bool is_ready) {
  [bridge_ setConfirmationsIsReady:is_ready];
}
void NativeLedgerClient::SetTimer(uint64_t time_offset, uint32_t * timer_id) {
  [bridge_ setTimer:time_offset timerId:timer_id];
}
std::string NativeLedgerClient::URIEncode(const std::string & value) {
  return [bridge_ URIEncode:value];
}
std::unique_ptr<ledger::LogStream> NativeLedgerClient::VerboseLog(const char * file, int line, int vlog_level) const {
  return [bridge_ verboseLog:file line:line vlogLevel:vlog_level];
}
void NativeLedgerClient::OnContributeUnverifiedPublishers(ledger::Result result, const std::string& publisher_key, const std::string& publisher_name) {
  return [bridge_ onContributeUnverifiedPublishers:result publisherKey:publisher_key publisherName:publisher_name];
}
void NativeLedgerClient::SetBooleanState(const std::string& name, bool value) {
  [bridge_ setBooleanState:name value:value];
}
bool NativeLedgerClient::GetBooleanState(const std::string& name) const {
  return [bridge_ getBooleanState:name];
}
void NativeLedgerClient::SetIntegerState(const std::string& name, int value) {
  [bridge_ setIntegerState:name value:value];
}
int NativeLedgerClient::GetIntegerState(const std::string& name) const {
  return [bridge_ getIntegerState:name];
}
void NativeLedgerClient::SetDoubleState(const std::string& name, double value) {
  [bridge_ setDoubleState:name value:value];
}
double NativeLedgerClient::GetDoubleState(const std::string& name) const {
  return [bridge_ getDoubleState:name];
}
void NativeLedgerClient::SetStringState(const std::string& name, const std::string& value) {
  [bridge_ setStringState:name value:value];
}
std::string NativeLedgerClient::GetStringState(const std::string& name) const {
  return [bridge_ getStringState:name];
}
void NativeLedgerClient::SetInt64State(const std::string& name, int64_t value) {
  [bridge_ setInt64State:name value:value];
}
int64_t NativeLedgerClient::GetInt64State(const std::string& name) const {
  return [bridge_ getInt64State:name];
}
void NativeLedgerClient::SetUint64State(const std::string& name, uint64_t value) {
  [bridge_ setUint64State:name value:value];
}
uint64_t NativeLedgerClient::GetUint64State(const std::string& name) const {
  return [bridge_ getUint64State:name];
}
void NativeLedgerClient::ClearState(const std::string& name) {
  [bridge_ clearState:name];
}
void NativeLedgerClient::GetExternalWallets(ledger::GetExternalWalletsCallback callback) {
  [bridge_ getExternalWallets:callback];
}
void NativeLedgerClient::SaveExternalWallet(const std::string& wallet_type, ledger::ExternalWalletPtr wallet) {
  [bridge_ saveExternalWallet:wallet_type wallet:std::move(wallet)];
}
void NativeLedgerClient::ShowNotification(const std::string& type, const std::vector<std::string>& args, ledger::ShowNotificationCallback callback) {
  [bridge_ showNotification:type args:args callback:callback];
}
void NativeLedgerClient::DeleteActivityInfo(const std::string& publisher_key, ledger::DeleteActivityInfoCallback callback) {
  [bridge_ deleteActivityInfo:publisher_key callback:callback];
}
void NativeLedgerClient::ClearAndInsertServerPublisherList(ledger::ServerPublisherInfoList list, ledger::ClearAndInsertServerPublisherListCallback callback) {
  [bridge_ clearAndInsertServerPublisherList:std::move(list) callback:callback];
}
void NativeLedgerClient::GetServerPublisherInfo(const std::string& publisher_key, ledger::GetServerPublisherInfoCallback callback) {
  [bridge_ getServerPublisherInfo:publisher_key callback:callback];
}
void NativeLedgerClient::SetTransferFee(const std::string& wallet_type, ledger::TransferFeePtr transfer_fee) {
  [bridge_ setTransferFee:wallet_type transfer_fee:std::move(transfer_fee)];
}
ledger::TransferFeeList NativeLedgerClient::GetTransferFees(const std::string& wallet_type) {
  return [bridge_ getTransferFees:wallet_type];
}
void NativeLedgerClient::RemoveTransferFee(const std::string& wallet_type, const std::string& id) {
  [bridge_ removeTransferFee:wallet_type id:id];
}