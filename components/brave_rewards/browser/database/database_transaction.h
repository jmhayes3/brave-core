/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_REWARDS_BROWSER_DATABASE_DATABASE_TRANSACTION_H_
#define BRAVE_COMPONENTS_BRAVE_REWARDS_BROWSER_DATABASE_DATABASE_TRANSACTION_H_

#include <string>

#include "base/containers/flat_map.h"
#include "bat/ledger/transaction.h"
#include "brave/components/brave_rewards/browser/database/database_table.h"

namespace brave_rewards {

class DatabaseTransaction: public DatabaseTable {
 public:
  explicit DatabaseTransaction(int current_db_version);
  ~DatabaseTransaction() override;

  bool Init(
      sql::Database* db) override;

  bool CreateTable(
      sql::Database* db) override;

  bool CreateIndex(
      sql::Database* db) override;

  bool InsertOrUpdate(
      sql::Database* db,
      ledger::TransactionPtr info);

  base::flat_map<std::string, std::string> GetRecord(
      sql::Database* db,
      const std::string& key);

 private:
  const char* table_name_ = "transaction";
  const int minimum_version_ = 7;
};

}  // namespace brave_rewards

#endif  // BRAVE_COMPONENTS_BRAVE_REWARDS_BROWSER_DATABASE_DATABASE_TRANSACTION_H_
