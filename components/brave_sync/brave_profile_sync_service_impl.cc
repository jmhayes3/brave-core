/* Copyright 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_sync/brave_profile_sync_service_impl.h"

#include <utility>
#include <vector>

#include "base/bind.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "brave/components/brave_sync/brave_sync_prefs.h"
#include "brave/components/brave_sync/brave_sync_service_observer.h"
#include "brave/components/brave_sync/client/brave_sync_client_impl.h"
#include "brave/components/brave_sync/jslib_const.h"
#include "brave/components/brave_sync/jslib_messages.h"
#include "brave/components/brave_sync/settings.h"
#include "brave/components/brave_sync/sync_devices.h"
#include "brave/components/brave_sync/syncer_helper.h"
#include "brave/components/brave_sync/tools.h"
#include "brave/components/brave_sync/values_conv.h"
#include "chrome/browser/sync/chrome_sync_client.h"
#include "components/bookmarks/browser/bookmark_model.h"
#include "components/signin/public/identity_manager/account_info.h"
#include "components/sync/engine_impl/syncer.h"
#include "content/public/browser/browser_thread.h"
#include "net/base/network_interfaces.h"
#include "ui/base/models/tree_node_iterator.h"

namespace brave_sync {

using browser_sync::ChromeSyncClient;
using jslib::Device;
using jslib::MetaInfo;
using jslib::SyncRecord;
using jslib_const::kBookmarks;
using jslib_const::kHistorySites;
using jslib_const::kPreferences;
using jslib_const::SyncObjectData_DEVICE;
using jslib_const::SyncRecordType_PREFERENCES;
using tools::IsTimeEmpty;

const std::vector<unsigned> BraveProfileSyncServiceImpl::kExponentialWaits = {
    10, 20, 40, 80};
const int BraveProfileSyncServiceImpl::kMaxSendRetries =
    BraveProfileSyncServiceImpl::kExponentialWaits.size() - 1;

namespace {

AccountInfo GetDummyAccountInfo() {
  AccountInfo account_info;
  account_info.account_id = "dummy_account_id";
  return account_info;
}

void NotifyLogMessage(const std::string& message) {
  DLOG(INFO) << message;
}

std::string GetDeviceName() {
  std::string hostname = net::GetHostName();
  if (hostname.empty()) {
#if defined(OS_MACOSX)
    hostname = std::string("Mac Desktop");
#elif defined(OS_LINUX)
    hostname = std::string("Linux Desktop");
#elif defined(OS_WIN)
    hostname = std::string("Windows Desktop");
#endif
  }
  return hostname;
}

RecordsListPtr CreateDeviceCreationRecordExtension(
    const std::string& deviceName,
    const std::string& objectId,
    const SyncRecord::Action& action,
    const std::string& deviceId) {
  RecordsListPtr records = std::make_unique<RecordsList>();

  SyncRecordPtr record = std::make_unique<SyncRecord>();

  record->action = action;
  record->deviceId = deviceId;
  record->objectId = objectId;
  record->objectData = SyncObjectData_DEVICE;  // "device"

  std::unique_ptr<Device> device = std::make_unique<Device>();
  device->name = deviceName;
  record->SetDevice(std::move(device));

  records->emplace_back(std::move(record));

  return records;
}

const bookmarks::BookmarkNode* FindByObjectId(bookmarks::BookmarkModel* model,
                                              const std::string& object_id) {
  ui::TreeNodeIterator<const bookmarks::BookmarkNode> iterator(
      model->root_node());
  while (iterator.has_next()) {
    const bookmarks::BookmarkNode* node = iterator.Next();
    std::string node_object_id;
    node->GetMetaInfo("object_id", &node_object_id);

    if (!node_object_id.empty() && object_id == node_object_id)
      return node;
  }
  return nullptr;
}

std::unique_ptr<SyncRecord> CreateDeleteBookmarkByObjectId(
    const prefs::Prefs* brave_sync_prefs,
    const std::string& object_id) {
  auto record = std::make_unique<SyncRecord>();
  record->deviceId = brave_sync_prefs->GetThisDeviceId();
  record->objectData = jslib_const::SyncObjectData_BOOKMARK;
  record->objectId = object_id;
  record->action = jslib::SyncRecord::Action::A_DELETE;
  record->syncTimestamp = base::Time::Now();
  auto bookmark = std::make_unique<jslib::Bookmark>();
  record->SetBookmark(std::move(bookmark));
  return record;
}

void DoDispatchGetRecordsCallback(
    GetRecordsCallback cb,
    std::unique_ptr<brave_sync::RecordsList> records) {
  std::move(cb).Run(std::move(records));
}

void AddSyncEntityInfo(jslib::Bookmark* bookmark,
                       const bookmarks::BookmarkNode* node,
                       const std::string& key) {
  std::string value;
  if (node->GetMetaInfo(key, &value)) {
    MetaInfo metaInfo;
    metaInfo.key = key;
    metaInfo.value = value;
    bookmark->metaInfo.push_back(metaInfo);
  }
}

SyncRecordPtr PrepareResolvedDevice(SyncDevice* device,
                                    SyncRecord::Action action) {
  auto record = std::make_unique<jslib::SyncRecord>();
  record->action = action;
  record->deviceId = device->device_id_;
  record->objectId = device->object_id_;
  record->objectData = jslib_const::SyncObjectData_DEVICE;  // "device"
  std::unique_ptr<jslib::Device> device_record =
      std::make_unique<jslib::Device>();
  device_record->name = device->name_;
  record->SetDevice(std::move(device_record));
  return record;
}
}  // namespace

BraveProfileSyncServiceImpl::BraveProfileSyncServiceImpl(Profile* profile,
                                                         InitParams init_params)
    : BraveProfileSyncService(std::move(init_params)),
      brave_sync_client_(BraveSyncClient::Create(this, profile)) {
  brave_sync_words_ = std::string();
  brave_sync_prefs_ =
      std::make_unique<prefs::Prefs>(sync_client_->GetPrefService());

  // Moniter syncs prefs required in GetSettingsAndDevices
  brave_pref_change_registrar_.Init(sync_client_->GetPrefService());
  brave_pref_change_registrar_.Add(
      prefs::kSyncEnabled,
      base::Bind(&BraveProfileSyncServiceImpl::OnBraveSyncPrefsChanged,
                 base::Unretained(this)));
  brave_pref_change_registrar_.Add(
      prefs::kSyncDeviceName,
      base::Bind(&BraveProfileSyncServiceImpl::OnBraveSyncPrefsChanged,
                 base::Unretained(this)));
  brave_pref_change_registrar_.Add(
      prefs::kSyncDeviceList,
      base::Bind(&BraveProfileSyncServiceImpl::OnBraveSyncPrefsChanged,
                 base::Unretained(this)));
  brave_pref_change_registrar_.Add(
      prefs::kSyncBookmarksEnabled,
      base::Bind(&BraveProfileSyncServiceImpl::OnBraveSyncPrefsChanged,
                 base::Unretained(this)));
  brave_pref_change_registrar_.Add(
      prefs::kSyncSiteSettingsEnabled,
      base::Bind(&BraveProfileSyncServiceImpl::OnBraveSyncPrefsChanged,
                 base::Unretained(this)));
  brave_pref_change_registrar_.Add(
      prefs::kSyncHistoryEnabled,
      base::Bind(&BraveProfileSyncServiceImpl::OnBraveSyncPrefsChanged,
                 base::Unretained(this)));
  // TODO(darkdh): find another way to obtain bookmark model
  // change introduced in 83b9663e3814ef7e53af5009d10033b89955db44
  model_ =
      static_cast<ChromeSyncClient*>(sync_client_.get())->GetBookmarkModel();

  if (!brave_sync_prefs_->GetSeed().empty() &&
      !brave_sync_prefs_->GetThisDeviceName().empty()) {
    brave_sync_configured_ = true;
  }
  network_connection_tracker_->AddNetworkConnectionObserver(this);
}

void BraveProfileSyncServiceImpl::OnNudgeSyncCycle(RecordsListPtr records) {
  if (!IsBraveSyncEnabled())
    return;

  for (auto& record : *records) {
    record->deviceId = brave_sync_prefs_->GetThisDeviceId();
  }
  if (!records->empty()) {
    if (((!brave_sync::tools::IsTimeEmpty(chain_created_time_) &&
          (base::Time::Now() - chain_created_time_).InSeconds() < 30u) ||
         brave_sync_prefs_->GetSyncDevices()->size() < 2)) {
      // Store records for now
      pending_send_records_.push_back(std::move(records));
      return;
    }
    SendAndPurgePendingRecords();
    SendSyncRecords(jslib_const::SyncRecordType_BOOKMARKS, std::move(records));
  }
}

BraveProfileSyncServiceImpl::~BraveProfileSyncServiceImpl() {
  network_connection_tracker_->RemoveNetworkConnectionObserver(this);
}

void BraveProfileSyncServiceImpl::OnSetupSyncHaveCode(
    const std::string& sync_words,
    const std::string& device_name) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  if (sync_words.empty()) {
    OnSyncSetupError("ERR_SYNC_WRONG_WORDS");
    return;
  }

  if (brave_sync_initializing_) {
    NotifyLogMessage("currently initializing");
    return;
  }

  if (IsBraveSyncConfigured()) {
    NotifyLogMessage("already configured");
    return;
  }

  ForceCompleteReset();
  DCHECK(!brave_sync_prefs_->GetSyncEnabled());

  if (device_name.empty())
    brave_sync_prefs_->SetThisDeviceName(GetDeviceName());
  else
    brave_sync_prefs_->SetThisDeviceName(device_name);
  brave_sync_initializing_ = true;

  brave_sync_prefs_->SetSyncEnabled(true);
  brave_sync_words_ = sync_words;
}

void BraveProfileSyncServiceImpl::OnSetupSyncNewToSync(
    const std::string& device_name) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  if (brave_sync_initializing_) {
    NotifyLogMessage("currently initializing");
    return;
  }

  if (IsBraveSyncConfigured()) {
    NotifyLogMessage("already configured");
    return;
  }

  ForceCompleteReset();
  DCHECK(!brave_sync_prefs_->GetSyncEnabled());

  // If the previous attempt was connect to sync chain
  // and failed to receive save-init-data
  brave_sync_words_.clear();

  if (device_name.empty())
    brave_sync_prefs_->SetThisDeviceName(GetDeviceName());
  else
    brave_sync_prefs_->SetThisDeviceName(device_name);

  brave_sync_initializing_ = true;

  brave_sync_prefs_->SetSyncEnabled(true);
}

void BraveProfileSyncServiceImpl::OnDeleteDevice(const std::string& device_id) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  auto sync_devices = brave_sync_prefs_->GetSyncDevices();

  const SyncDevice* device = sync_devices->GetByDeviceId(device_id);
  if (device) {
    const std::string device_name = device->name_;
    const std::string object_id = device->object_id_;
    SendDeviceSyncRecord(SyncRecord::Action::A_DELETE, device_name, device_id,
                         object_id);
  }
}

void BraveProfileSyncServiceImpl::OnResetSync() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  auto sync_devices = brave_sync_prefs_->GetSyncDevices();

  if (sync_devices->size() == 0) {
    // Fail safe option
    VLOG(2) << "[Sync] " << __func__ << " unexpected zero device size";
    ResetSyncInternal();
  } else {
    // We have to send delete record and wait for library deleted response then
    // we can reset it by ResetSyncInternal()
    const std::string device_id = brave_sync_prefs_->GetThisDeviceId();
    OnDeleteDevice(device_id);
    reseting_ = true;
  }
}

void BraveProfileSyncServiceImpl::GetSettingsAndDevices(
    const GetSettingsAndDevicesCallback& callback) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  auto settings = brave_sync_prefs_->GetBraveSyncSettings();
  auto devices = brave_sync_prefs_->GetSyncDevices();
  callback.Run(std::move(settings), std::move(devices));
}

void BraveProfileSyncServiceImpl::GetSyncWords() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  // Ask sync client
  std::string seed = brave_sync_prefs_->GetSeed();
  brave_sync_client_->NeedSyncWords(seed);
}

std::string BraveProfileSyncServiceImpl::GetSeed() {
  return brave_sync_prefs_->GetSeed();
}

void BraveProfileSyncServiceImpl::OnSetSyncEnabled(
    const bool sync_this_device) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  brave_sync_prefs_->SetSyncEnabled(sync_this_device);
}

void BraveProfileSyncServiceImpl::OnSetSyncBookmarks(
    const bool sync_bookmarks) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  syncer::UserSelectableTypeSet type_set =
      ProfileSyncService::GetUserSettings()->GetSelectedTypes();
  if (sync_bookmarks)
    type_set.Put(syncer::UserSelectableType::kBookmarks);
  else
    type_set.Remove(syncer::UserSelectableType::kBookmarks);
  ProfileSyncService::GetUserSettings()->SetSelectedTypes(false, type_set);
  if (brave_sync_prefs_->GetSyncBookmarksEnabled() != sync_bookmarks)
    brave_sync_prefs_->SetSyncBookmarksEnabled(sync_bookmarks);
}

void BraveProfileSyncServiceImpl::OnSetSyncBrowsingHistory(
    const bool sync_browsing_history) {
  brave_sync_prefs_->SetSyncHistoryEnabled(sync_browsing_history);
}

void BraveProfileSyncServiceImpl::OnSetSyncSavedSiteSettings(
    const bool sync_saved_site_settings) {
  brave_sync_prefs_->SetSyncSiteSettingsEnabled(sync_saved_site_settings);
}

void BraveProfileSyncServiceImpl::BackgroundSyncStarted(bool startup) {}

void BraveProfileSyncServiceImpl::BackgroundSyncStopped(bool shutdown) {}

void BraveProfileSyncServiceImpl::OnSyncDebug(const std::string& message) {
  NotifyLogMessage(message);
}

void BraveProfileSyncServiceImpl::OnSyncSetupError(const std::string& error) {
  if (brave_sync_initializing_) {
    brave_sync_prefs_->Clear();
    brave_sync_initializing_ = false;
  }
  NotifySyncSetupError(error);
}

void BraveProfileSyncServiceImpl::OnGetInitData(
    const std::string& sync_version) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  Uint8Array seed;
  if (!brave_sync_words_.empty()) {
    VLOG(1) << "[Brave Sync] Init from sync words";
  } else if (!brave_sync_prefs_->GetSeed().empty()) {
    seed = Uint8ArrayFromString(brave_sync_prefs_->GetSeed());
    VLOG(1) << "[Brave Sync] Init from prefs";
  } else {
    VLOG(1) << "[Brave Sync] Init new chain";
  }

  Uint8Array device_id;
  if (!brave_sync_prefs_->GetThisDeviceId().empty()) {
    device_id = Uint8ArrayFromString(brave_sync_prefs_->GetThisDeviceId());
    VLOG(1) << "[Brave Sync] Init device id from prefs: "
            << StrFromUint8Array(device_id);
  } else {
    VLOG(1) << "[Brave Sync] Init empty device id";
  }

  DCHECK(!sync_version.empty());
  // TODO(bridiver) - this seems broken because using the version we get back
  // from the server (currently v1.4.2) causes things to break. What is the
  // the point of having this value?
  brave_sync_prefs_->SetApiVersion("0");

  client_data::Config config;
  config.api_version = brave_sync_prefs_->GetApiVersion();
  config.server_url = "https://sync.brave.com";
  config.debug = true;
  brave_sync_client_->SendGotInitData(seed, device_id, config,
                                      brave_sync_words_);
}

void BraveProfileSyncServiceImpl::OnSaveInitData(const Uint8Array& seed,
                                                 const Uint8Array& device_id) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  DCHECK(!brave_sync_initialized_);
  // If we are here and brave_sync_initializing_ is false, we have came
  // not from OnSetupSyncNewToSync or OnSetupSyncHaveCode.
  // One case is we put wrong code words and then restarted before cleared
  // kSyncEnabled pref. This should not happen.
  DCHECK(brave_sync_initializing_);

  std::string seed_str = StrFromUint8Array(seed);
  std::string device_id_str = StrFromUint8Array(device_id);

  std::string prev_seed_str = brave_sync_prefs_->GetPrevSeed();

  brave_sync_words_.clear();
  DCHECK(!seed_str.empty());

  if (prev_seed_str == seed_str) {  // reconnecting to previous sync chain
    brave_sync_prefs_->SetPrevSeed(std::string());
  } else if (!prev_seed_str.empty()) {  // connect/create to new sync chain
    // bookmark_change_processor_->Reset(true);
    brave_sync_prefs_->SetPrevSeed(std::string());
  } else {
    // This is not required, because when there is no previous seed, bookmarks
    // should not have a metadata. However, this is done by intention, to be
    // a remedy for cases when sync had been reset and prev_seed_str had been
    // cleared when it shouldn't (brave-browser#3188).
    // bookmark_change_processor_->Reset(true);
  }

  brave_sync_prefs_->SetSeed(seed_str);
  brave_sync_prefs_->SetThisDeviceId(device_id_str);

  brave_sync_configured_ = true;

  brave_sync_initializing_ = false;
}

void BraveProfileSyncServiceImpl::OnSyncReady() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  const std::string bookmarks_base_order =
      brave_sync_prefs_->GetBookmarksBaseOrder();
  if (bookmarks_base_order.empty()) {
    std::string platform = tools::GetPlatformName();
    brave_sync_client_->SendGetBookmarksBaseOrder(
        brave_sync_prefs_->GetThisDeviceId(), platform);
    // OnSyncReady will be called by OnSaveBookmarksBaseOrder
    return;
  }

  DCHECK(false == brave_sync_initialized_);
  brave_sync_initialized_ = true;

  // For launching from legacy sync profile and also brand new profile
  if (brave_sync_prefs_->GetMigratedBookmarksVersion() < 2)
    SetPermanentNodesOrder(brave_sync_prefs_->GetBookmarksBaseOrder());

  syncer::SyncPrefs sync_prefs(sync_client_->GetPrefService());
  // first time setup sync or migrated from legacy sync
  if (sync_prefs.GetLastSyncedTime().is_null()) {
    ProfileSyncService::GetUserSettings()->SetSelectedTypes(
        false, syncer::UserSelectableTypeSet());
    // default enable bookmark
    // this is important, don't change
    // to brave_sync_prefs_->SetSyncBookmarksEnabled(true);
    OnSetSyncBookmarks(true);
    ProfileSyncService::GetUserSettings()->SetSyncRequested(true);
  }
}

syncer::ModelTypeSet BraveProfileSyncServiceImpl::GetPreferredDataTypes()
    const {
  // Force DEVICE_INFO type to have nudge cycle each time to fetch
  // Brave sync devices.
  // Will be picked up by ProfileSyncService::ConfigureDataTypeManager
  return Union(ProfileSyncService::GetPreferredDataTypes(),
               {syncer::DEVICE_INFO});
}

std::unique_ptr<SyncRecordAndExistingList>
BraveProfileSyncServiceImpl::PrepareResolvedPreferences(
    const RecordsList& records) {
  auto sync_devices = brave_sync_prefs_->GetSyncDevices();
  auto records_and_existing_objects =
      std::make_unique<SyncRecordAndExistingList>();

  for (const SyncRecordPtr& record : records) {
    auto resolved_record = std::make_unique<SyncRecordAndExisting>();
    resolved_record->first = jslib::SyncRecord::Clone(*record);
    auto* device = sync_devices->GetByObjectId(record->objectId);
    if (device)
      resolved_record->second = PrepareResolvedDevice(device, record->action);
    records_and_existing_objects->emplace_back(std::move(resolved_record));
  }
  return records_and_existing_objects;
}

void BraveProfileSyncServiceImpl::OnGetExistingObjects(
    const std::string& category_name,
    std::unique_ptr<RecordsList> records,
    const base::Time& last_record_time_stamp,
    const bool is_truncated) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  // TODO(bridiver) - what do we do with is_truncated ?
  // It appears to be ignored in b-l
  if (category_name == kBookmarks) {
    if (!IsTimeEmpty(last_record_time_stamp)) {
      brave_sync_prefs_->SetLatestRecordTime(last_record_time_stamp);
    }
    auto records_and_existing_objects =
        std::make_unique<SyncRecordAndExistingList>();
    CreateResolveList(*records.get(), records_and_existing_objects.get());
    brave_sync_client_->SendResolveSyncRecords(
        category_name, std::move(records_and_existing_objects));
  } else if (category_name == kPreferences) {
    if (!tools::IsTimeEmpty(last_record_time_stamp)) {
      brave_sync_prefs_->SetLatestDeviceRecordTime(last_record_time_stamp);
    }
    auto existing_records = PrepareResolvedPreferences(*records.get());
    brave_sync_client_->SendResolveSyncRecords(category_name,
                                               std::move(existing_records));
  }
}

void BraveProfileSyncServiceImpl::OnResolvedSyncRecords(
    const std::string& category_name,
    std::unique_ptr<RecordsList> records) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  if (category_name == jslib_const::kPreferences) {
    OnResolvedPreferences(*records.get());
  } else if (category_name == kBookmarks) {
    for (auto& record : *records) {
      LoadSyncEntityInfo(record.get());
    }
    // Send records to syncer
    if (get_record_cb_)
      sync_thread_->task_runner()->PostTask(
          FROM_HERE,
          base::BindOnce(&DoDispatchGetRecordsCallback,
                         std::move(get_record_cb_), std::move(records)));
    SignalWaitableEvent();
  } else if (category_name == kHistorySites) {
    NOTIMPLEMENTED();
  }
}

void BraveProfileSyncServiceImpl::OnDeletedSyncUser() {
  NOTIMPLEMENTED();
}

void BraveProfileSyncServiceImpl::OnDeleteSyncSiteSettings() {
  NOTIMPLEMENTED();
}

void BraveProfileSyncServiceImpl::OnSaveBookmarksBaseOrder(
    const std::string& order) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  DCHECK(!order.empty());
  brave_sync_prefs_->SetBookmarksBaseOrder(order);
  OnSyncReady();
}

void BraveProfileSyncServiceImpl::OnSyncWordsPrepared(
    const std::string& words) {
  NotifyHaveSyncWords(words);
}

int BraveProfileSyncServiceImpl::GetDisableReasons() const {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

  // legacy sync only support bookmark sync so we have to wait for migration
  // complete before enable sync engine
  if (IsBraveSyncEnabled() &&
      brave_sync_prefs_->GetMigratedBookmarksVersion() >= 2)
    return syncer::SyncService::DISABLE_REASON_NONE;
  // kSyncManaged is set by Brave so it will contain
  // DISABLE_REASON_ENTERPRISE_POLICY and
  // SaveCardBubbleControllerImpl::ShouldShowSignInPromo will return false.
  return ProfileSyncService::GetDisableReasons();
}

CoreAccountInfo BraveProfileSyncServiceImpl::GetAuthenticatedAccountInfo()
    const {
  return GetDummyAccountInfo();
}

bool BraveProfileSyncServiceImpl::IsAuthenticatedAccountPrimary() const {
  return true;
}

void BraveProfileSyncServiceImpl::OnConnectionChanged(
    network::mojom::ConnectionType type) {
  if (type == network::mojom::ConnectionType::CONNECTION_NONE)
    SignalWaitableEvent();
}

void BraveProfileSyncServiceImpl::Shutdown() {
  SignalWaitableEvent();
  syncer::ProfileSyncService::Shutdown();
}

void BraveProfileSyncServiceImpl::NotifySyncSetupError(
    const std::string& error) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  for (auto& observer : BraveSyncService::observers_)
    observer.OnSyncSetupError(this, error);
}

void BraveProfileSyncServiceImpl::NotifySyncStateChanged() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  for (auto& observer : BraveSyncService::observers_)
    observer.OnSyncStateChanged(this);
}

void BraveProfileSyncServiceImpl::NotifyHaveSyncWords(
    const std::string& sync_words) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  for (auto& observer : BraveSyncService::observers_)
    observer.OnHaveSyncWords(this, sync_words);
}

void BraveProfileSyncServiceImpl::ResetSyncInternal() {
  SignalWaitableEvent();
  brave_sync_prefs_->SetPrevSeed(brave_sync_prefs_->GetSeed());

  brave_sync_prefs_->Clear();

  brave_sync_configured_ = false;
  brave_sync_initialized_ = false;

  brave_sync_prefs_->SetSyncEnabled(false);
  ProfileSyncService::GetUserSettings()->SetSyncRequested(false);

  // brave sync doesn't support pause sync so treating every new sync chain as
  // first time setup
  syncer::SyncPrefs sync_prefs(sync_client_->GetPrefService());
  sync_prefs.SetLastSyncedTime(base::Time());

  reseting_ = false;
}

void BraveProfileSyncServiceImpl::ForceCompleteReset() {
  if (reseting_) {
    ResetSyncInternal();
  }
}

void BraveProfileSyncServiceImpl::SetPermanentNodesOrder(
    const std::string& base_order) {
  DCHECK(model_);
  DCHECK(!base_order.empty());
  std::string order;
  model_->bookmark_bar_node()->GetMetaInfo("order", &order);
  if (order.empty()) {
    model_->SetNodeMetaInfo(model_->bookmark_bar_node(), "order",
                            base_order + "1");
  }
  order.clear();
  model_->other_node()->GetMetaInfo("order", &order);
  if (order.empty()) {
    model_->SetNodeMetaInfo(model_->other_node(), "order", base_order + "2");
  }
  brave_sync_prefs_->SetMigratedBookmarksVersion(2);
}

std::unique_ptr<SyncRecord>
BraveProfileSyncServiceImpl::BookmarkNodeToSyncBookmark(
    const bookmarks::BookmarkNode* node) {
  if (node->is_permanent_node() || !node->parent())
    return std::unique_ptr<SyncRecord>();

  auto record = std::make_unique<SyncRecord>();
  record->deviceId = brave_sync_prefs_->GetThisDeviceId();
  record->objectData = jslib_const::SyncObjectData_BOOKMARK;

  auto bookmark = std::make_unique<jslib::Bookmark>();
  bookmark->site.location = node->url().spec();
  bookmark->site.title = base::UTF16ToUTF8(node->GetTitledUrlNodeTitle());
  bookmark->site.customTitle = base::UTF16ToUTF8(node->GetTitle());
  // bookmark->site.lastAccessedTime - ignored
  bookmark->site.creationTime = node->date_added();
  bookmark->site.favicon = node->icon_url() ? node->icon_url()->spec() : "";
  // Url may have type OTHER_NODE if it is in Deleted Bookmarks
  bookmark->isFolder = (node->type() != bookmarks::BookmarkNode::URL &&
                        node->type() != bookmarks::BookmarkNode::OTHER_NODE);
  bookmark->hideInToolbar = node->parent() != model_->bookmark_bar_node();

  std::string object_id;
  node->GetMetaInfo("object_id", &object_id);
  record->objectId = object_id;

  std::string parent_object_id;
  node->parent()->GetMetaInfo("object_id", &parent_object_id);
  bookmark->parentFolderObjectId = parent_object_id;

  std::string order;
  node->GetMetaInfo("order", &order);
  DCHECK(!order.empty());
  bookmark->order = order;

  std::string sync_timestamp;
  node->GetMetaInfo("sync_timestamp", &sync_timestamp);
  DCHECK(!sync_timestamp.empty());

  record->syncTimestamp = base::Time::FromJsTime(std::stod(sync_timestamp));

  record->action = jslib::SyncRecord::Action::A_UPDATE;

  AddSyncEntityInfo(bookmark.get(), node, "originator_cache_guid");
  AddSyncEntityInfo(bookmark.get(), node, "originator_client_item_id");
  AddSyncEntityInfo(bookmark.get(), node, "version");
  AddSyncEntityInfo(bookmark.get(), node, "position_in_parent");

  record->SetBookmark(std::move(bookmark));

  return record;
}

void BraveProfileSyncServiceImpl::SaveSyncEntityInfo(
    const jslib::SyncRecord* record) {
  auto* node = FindByObjectId(model_, record->objectId);
  // no need to save for DELETE
  if (node) {
    auto& bookmark = record->GetBookmark();
    for (auto& meta_info : bookmark.metaInfo) {
      model_->SetNodeMetaInfo(node, meta_info.key, meta_info.value);
    }
  }
}

void BraveProfileSyncServiceImpl::LoadSyncEntityInfo(
    jslib::SyncRecord* record) {
  auto* node = FindByObjectId(model_, record->objectId);
  if (node) {
    auto* bookmark = record->mutable_bookmark();
    if (!bookmark->metaInfo.empty())
      return;
    AddSyncEntityInfo(bookmark, node, "originator_cache_guid");
    AddSyncEntityInfo(bookmark, node, "originator_client_item_id");
    AddSyncEntityInfo(bookmark, node, "position_in_parent");
    std::string s_version;
    // Version needs to be incremented by 1 for legacy sync to emulate
    // GetUpdateProcessor behavior
    if (node->GetMetaInfo("version", &s_version)) {
      int64_t version;
      bool result = base::StringToInt64(s_version, &version);
      DCHECK(result);
      MetaInfo metaInfo;
      metaInfo.key = "version";
      metaInfo.value = std::to_string(++version);
      bookmark->metaInfo.push_back(metaInfo);
    }
  }
}

void BraveProfileSyncServiceImpl::CreateResolveList(
    const std::vector<std::unique_ptr<SyncRecord>>& records,
    SyncRecordAndExistingList* records_and_existing_objects) {
  const auto& this_device_id = brave_sync_prefs_->GetThisDeviceId();
  for (const auto& record : records) {
    // Ignore records from ourselves to avoid mess on merge
    if (record->deviceId == this_device_id) {
      // Remove Acked sent records
      brave_sync_prefs_->RemoveFromRecordsToResend(record->objectId);
      continue;
    }
    auto resolved_record = std::make_unique<SyncRecordAndExisting>();
    resolved_record->first = SyncRecord::Clone(*record);
    auto* node = FindByObjectId(model_, record->objectId);
    if (node) {
      resolved_record->second = BookmarkNodeToSyncBookmark(node);
    }

    records_and_existing_objects->push_back(std::move(resolved_record));
  }
}

void BraveProfileSyncServiceImpl::FetchSyncRecords(const bool bookmarks,
                                                   const bool history,
                                                   const bool preferences,
                                                   int max_records) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  DCHECK(bookmarks || history || preferences);
  if (!(bookmarks || history || preferences)) {
    return;
  }

  std::vector<std::string> category_names;
  if (history) {
    category_names.push_back(kHistorySites);  // "HISTORY_SITES";
  }
  if (bookmarks) {
    category_names.push_back(kBookmarks);  // "BOOKMARKS";
  }
  if (preferences) {
    category_names.push_back(kPreferences);  // "PREFERENCES";
  }

  base::Time start_at_time = brave_sync_prefs_->GetLatestRecordTime();
  brave_sync_client_->SendFetchSyncRecords(category_names, start_at_time,
                                           max_records);
}

void BraveProfileSyncServiceImpl::SendCreateDevice() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

  std::string device_name = brave_sync_prefs_->GetThisDeviceName();
  std::string object_id = tools::GenerateObjectId();
  std::string device_id = brave_sync_prefs_->GetThisDeviceId();
  CHECK(!device_id.empty());

  SendDeviceSyncRecord(SyncRecord::Action::A_CREATE, device_name, device_id,
                       object_id);
}

void BraveProfileSyncServiceImpl::SendDeviceSyncRecord(
    const int action,
    const std::string& device_name,
    const std::string& device_id,
    const std::string& object_id) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  RecordsListPtr records = CreateDeviceCreationRecordExtension(
      device_name, object_id, static_cast<SyncRecord::Action>(action),
      device_id);
  SendSyncRecords(SyncRecordType_PREFERENCES, std::move(records));
}

void BraveProfileSyncServiceImpl::OnResolvedPreferences(
    const RecordsList& records) {
  const std::string this_device_id = brave_sync_prefs_->GetThisDeviceId();
  bool this_device_deleted = false;
  bool contains_only_one_device = false;

  auto sync_devices = brave_sync_prefs_->GetSyncDevices();
  auto old_devices_size = sync_devices->size();
  for (const auto& record : records) {
    DCHECK(record->has_device() || record->has_sitesetting());
    if (record->has_device()) {
      bool actually_merged = false;
      sync_devices->Merge(
          SyncDevice(record->GetDevice().name, record->objectId,
                     record->deviceId, record->syncTimestamp.ToJsTime()),
          record->action, &actually_merged);
      this_device_deleted =
          this_device_deleted ||
          (record->deviceId == this_device_id &&
           record->action == SyncRecord::Action::A_DELETE && actually_merged);
      contains_only_one_device =
          sync_devices->size() < 2 &&
          record->action == SyncRecord::Action::A_DELETE && actually_merged;
    }
  }  // for each device

  brave_sync_prefs_->SetSyncDevices(*sync_devices);

  if (old_devices_size < 2 && sync_devices->size() >= 2) {
    // Save chain creation time to send bookmarks 30 sec after
    chain_created_time_ = base::Time::Now();
  }
  if (!tools::IsTimeEmpty(chain_created_time_) &&
      (base::Time::Now() - chain_created_time_).InSeconds() > 30u) {
    SendAndPurgePendingRecords();
  }

  if (this_device_deleted) {
    ResetSyncInternal();
  } else if (contains_only_one_device) {
    // We see amount of devices had been decreased to 1 and it is not this
    // device had been deleted. So call OnResetSync which will send DELETE
    // record for this device
    OnResetSync();
  }
}

void BraveProfileSyncServiceImpl::OnBraveSyncPrefsChanged(
    const std::string& pref) {
  if (pref == prefs::kSyncEnabled) {
    brave_sync_client_->OnSyncEnabledChanged();
  }
  NotifySyncStateChanged();
}

#if BUILDFLAG(ENABLE_EXTENSIONS)
BraveSyncClient* BraveProfileSyncServiceImpl::GetBraveSyncClient() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  return brave_sync_client_.get();
}
#endif

bool BraveProfileSyncServiceImpl::IsBraveSyncEnabled() const {
  return brave_sync_prefs_->GetSyncEnabled();
}

bool BraveProfileSyncServiceImpl::IsBraveSyncInitialized() const {
  return brave_sync_initialized_;
}

bool BraveProfileSyncServiceImpl::IsBraveSyncConfigured() const {
  return brave_sync_configured_ &&
         // When there is 0 or 1 device, it means chain is not completely
         // created, so we should give a chance to make force reset in
         // |OnSetupSyncHaveCode| or in |OnSetupSyncNewToSync|
         (brave_sync_prefs_->GetSyncDevices()->size() >= 2);
}
void BraveProfileSyncServiceImpl::FetchDevices() {
  DCHECK(sync_client_);
  brave_sync_prefs_->SetLastFetchTime(base::Time::Now());
  // When the chain is not fully created or our device preferences queue is
  // not available, it may happened we will not find required device sync
  // record when we will be switched to use SQS instead of S3 in sync lib.
  // Default Chromium fetch interval is 60 sec.
  // So during 70 sec after the chain creation forcing use of S3
  // by set start_at_time to 0.

  base::Time start_at_time;
  if (brave_sync_prefs_->GetSyncDevices()->size() <= 1 ||
      (!tools::IsTimeEmpty(chain_created_time_) &&
       (base::Time::Now() - chain_created_time_).InSeconds() < 70u)) {
    start_at_time = base::Time();
  } else {
    start_at_time = brave_sync_prefs_->GetLatestDeviceRecordTime();
  }

  brave_sync_client_->SendFetchSyncRecords(
      {brave_sync::jslib_const::kPreferences}, start_at_time, 1000);
}
void BraveProfileSyncServiceImpl::OnPollSyncCycle(GetRecordsCallback cb,
                                                  base::WaitableEvent* wevent) {
  if (!IsBraveSyncEnabled())
    return;

  if (IsTimeEmpty(brave_sync_prefs_->GetLastFetchTime()))
    SendCreateDevice();

  FetchDevices();

  if (!brave_sync_initialized_) {
    wevent->Signal();
    return;
  }

  get_record_cb_ = std::move(cb);
  wevent_ = wevent;

  const bool bookmarks = brave_sync_prefs_->GetSyncBookmarksEnabled();
  const bool history = brave_sync_prefs_->GetSyncHistoryEnabled();
  const bool preferences = brave_sync_prefs_->GetSyncSiteSettingsEnabled();
  FetchSyncRecords(bookmarks, history, preferences, 1000);
  ResendSyncRecords(jslib_const::SyncRecordType_BOOKMARKS);
}

void BraveProfileSyncServiceImpl::SignalWaitableEvent() {
  std::move(get_record_cb_);
  if (wevent_) {
    wevent_->Signal();
    wevent_ = nullptr;
  }
}

BraveSyncService* BraveProfileSyncServiceImpl::GetSyncService() const {
  return static_cast<BraveSyncService*>(
      const_cast<BraveProfileSyncServiceImpl*>(this));
}

void BraveProfileSyncServiceImpl::SendAndPurgePendingRecords() {
  for (auto& records_to_send : pending_send_records_) {
    SendSyncRecords(jslib_const::SyncRecordType_BOOKMARKS,
                    std::move(records_to_send));
  }
  pending_send_records_.clear();
}

void BraveProfileSyncServiceImpl::SendSyncRecords(
    const std::string& category_name,
    RecordsListPtr records) {
  brave_sync_client_->SendSyncRecords(category_name, *records);
  if (category_name == kBookmarks) {
    for (auto& record : *records) {
      SaveSyncEntityInfo(record.get());
      std::unique_ptr<base::DictionaryValue> meta =
          std::make_unique<base::DictionaryValue>();
      meta->SetInteger("send_retry_number", 0);
      meta->SetDouble("sync_timestamp", record->syncTimestamp.ToJsTime());
      brave_sync_prefs_->AddToRecordsToResend(record->objectId,
                                              std::move(meta));
    }
  }
}

void BraveProfileSyncServiceImpl::ResendSyncRecords(
    const std::string& category_name) {
  if (category_name == kBookmarks) {
    RecordsListPtr records = std::make_unique<RecordsList>();
    std::vector<std::string> records_to_resend =
        brave_sync_prefs_->GetRecordsToResend();
    if (records_to_resend.empty())
      return;
    for (auto& object_id : records_to_resend) {
      auto* node = FindByObjectId(model_, object_id);

      // Check resend interval
      const base::DictionaryValue* meta =
          brave_sync_prefs_->GetRecordToResendMeta(object_id);
      DCHECK(meta);
      int current_retry_number = kMaxSendRetries;
      meta->GetInteger("send_retry_number", &current_retry_number);
      DCHECK_GE(current_retry_number, 0);
      double sync_timestamp = 0;
      meta->GetDouble("sync_timestamp", &sync_timestamp);
      DCHECK(!base::Time::FromJsTime(sync_timestamp).is_null());

      if ((base::Time::Now() - base::Time::FromJsTime(sync_timestamp)) <
          GetRetryExponentialWaitAmount(current_retry_number))
        continue;

      // Increase retry number
      if (++current_retry_number > kMaxSendRetries)
        current_retry_number = kMaxSendRetries;
      std::unique_ptr<base::DictionaryValue> new_meta =
          base::DictionaryValue::From(
              std::make_unique<base::Value>(meta->Clone()));
      new_meta->SetInteger("send_retry_number", current_retry_number);
      new_meta->SetDouble("sync_timestamp", base::Time::Now().ToJsTime());
      brave_sync_prefs_->SetRecordToResendMeta(object_id, std::move(new_meta));

      if (node) {
        records->push_back(BookmarkNodeToSyncBookmark(node));
      } else {
        records->push_back(
            CreateDeleteBookmarkByObjectId(brave_sync_prefs_.get(), object_id));
      }
    }
    if (!records->empty())
      brave_sync_client_->SendSyncRecords(category_name, *records);
  }
}

// static
base::TimeDelta BraveProfileSyncServiceImpl::GetRetryExponentialWaitAmount(
    int retry_number) {
  DCHECK_LE(retry_number, kMaxSendRetries);

  if (retry_number > kMaxSendRetries) {
    retry_number = kMaxSendRetries;
  }
  return base::TimeDelta::FromMinutes(kExponentialWaits[retry_number]);
}

// static
std::vector<unsigned>
BraveProfileSyncServiceImpl::GetExponentialWaitsForTests() {
  return kExponentialWaits;
}

}  // namespace brave_sync
