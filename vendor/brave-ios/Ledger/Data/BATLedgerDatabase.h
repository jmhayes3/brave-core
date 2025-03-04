/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#import <Foundation/Foundation.h>
#import "Records.h"
#import "ledger.mojom.objc.h"

NS_ASSUME_NONNULL_BEGIN

typedef void (^BATLedgerDatabaseWriteCompletion)(BOOL success);

/// An interface into the ledger database
///
/// This class mirrors brave-core's `publisher_info_database.h/cc` file. This file will actually
/// likely be removed at a future date when database managment happens in the ledger library
@interface BATLedgerDatabase : NSObject

#pragma mark - Publisher Info

/// Get bare bones publisher info based on a publisher ID
+ (nullable BATPublisherInfo *)publisherInfoWithPublisherID:(NSString *)publisherID;

/// Get the publisher that will be displayed on the main brave rewards panel
+ (BATPublisherInfo *)panelPublisherWithFilter:(BATActivityInfoFilter *)filter;

/// Insert or update publisher info in the database given a BATPublisherInfo object
+ (void)insertOrUpdatePublisherInfo:(BATPublisherInfo *)info
                         completion:(nullable BATLedgerDatabaseWriteCompletion)completion;

/// Get a list of all excluded publishers
+ (NSArray<BATPublisherInfo *> *)excludedPublishers;

/// Restores all of the publishers to default excluded state
+ (void)restoreExcludedPublishers:(nullable BATLedgerDatabaseWriteCompletion)completion;

/// Get the number of publishers the user has excluded from Auto-Contribute
+ (NSUInteger)excludedPublishersCount;

#pragma mark - Contribution Info

/// Insert contribution info into the database given all the information for a contribution
+ (void)insertContributionInfo:(NSString *)probi
                         month:(const int)month
                          year:(const int)year
                          date:(const uint32_t)date
                  publisherKey:(NSString *)publisherKey
                      category:(BATRewardsCategory)category
                    completion:(nullable BATLedgerDatabaseWriteCompletion)completion;

/// Get a list of publishers you have supported with one time tips given some month and year
+ (NSArray<BATPublisherInfo *> *)oneTimeTipsPublishersForMonth:(BATActivityMonth)month
                                                          year:(int)year;

#pragma mark - Activity Info

/// Insert or update activity info from a publisher
+ (void)insertOrUpdateActivityInfoFromPublisher:(BATPublisherInfo *)info
                                     completion:(nullable BATLedgerDatabaseWriteCompletion)completion;

/// Insert or update a set of activity info based on a set of publishers
+ (void)insertOrUpdateActivitiesInfoFromPublishers:(NSArray<BATPublisherInfo *> *)publishers
                                        completion:(nullable BATLedgerDatabaseWriteCompletion)completion;

/// Get a list of publishers with activity info given some start, limit and
/// filter
+ (NSArray<BATPublisherInfo *> *)publishersWithActivityFromOffset:(uint32_t)start
                                                            limit:(uint32_t)limit
                                                           filter:(nullable BATActivityInfoFilter *)filter;

/// Delete activity info for a publisher with a given ID and reconcile stamp
+ (void)deleteActivityInfoWithPublisherID:(NSString *)publisherID
                           reconcileStamp:(uint64_t)reconcileStamp
                               completion:(nullable BATLedgerDatabaseWriteCompletion)completion;

#pragma mark - Media Publisher Info

/// Get a publisher linked with some media key
+ (nullable BATPublisherInfo *)mediaPublisherInfoWithMediaKey:(NSString *)mediaKey;

/// Insert or update some media info given some media key and publisher ID that it is linked to
+ (void)insertOrUpdateMediaPublisherInfoWithMediaKey:(NSString *)mediaKey
                                         publisherID:(NSString *)publisherID
                                          completion:(nullable BATLedgerDatabaseWriteCompletion)completion;

#pragma mark - Recurring Tips

/// Get a list of publishers you have supported with recurring tips
+ (NSArray<BATPublisherInfo *> *)recurringTips;

/// Insert a recurring tip linked to a given publisher ID for some amount
+ (void)insertOrUpdateRecurringTipWithPublisherID:(NSString *)publisherID
                                           amount:(double)amount
                                        dateAdded:(uint32_t)dateAdded
                                       completion:(nullable BATLedgerDatabaseWriteCompletion)completion;

/// Remove a recurring tip linked to a given publisher ID
+ (void)removeRecurringTipWithPublisherID:(NSString *)publisherID
                               completion:(nullable BATLedgerDatabaseWriteCompletion)completion;

#pragma mark - Pending Contributions

/// Get a list of pending contributions
+ (NSArray<BATPendingContributionInfo *> *)pendingContributions;

/// Inserts a set of pending contributions from a contribution list
+ (void)insertPendingContributions:(NSArray<BATPendingContribution *> *)contributions
                        completion:(nullable BATLedgerDatabaseWriteCompletion)completion;

/// Remove a pending contribution for a given publisher, viewing ID and added date
+ (void)removePendingContributionForPublisherID:(NSString *)publisherID
                                      viewingID:(NSString *)viewingID
                                      addedDate:(UInt64)addedDate
                                     completion:(nullable BATLedgerDatabaseWriteCompletion)completion;

/// Removes all the users pending contributions
+ (void)removeAllPendingContributions:(nullable BATLedgerDatabaseWriteCompletion)completion;

/// Get the amount of BAT allocated for pending contributions
+ (double)reservedAmountForPendingContributions;

#pragma mark - Publisher List

+ (nullable BATServerPublisherInfo *)serverPublisherInfoWithPublisherID:(NSString *)publisherID;

+ (void)clearAndInsertList:(NSArray<BATServerPublisherInfo *> *)list
                completion:(nullable BATLedgerDatabaseWriteCompletion)completion;

#pragma mark - Publisher List / Test Helpers

+ (nullable BATPublisherBanner *)bannerForPublisherID:(NSString *)publisherID;
+ (nullable NSArray<NSNumber *> *)bannerAmountsForPublisherWithPublisherID:(NSString *)publisherID;
+ (nullable NSDictionary<NSString *, NSString *> *)publisherLinksWithPublisherID:(NSString *)publisherID;

#pragma mark -

- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithCoder:(NSCoder *)aDecoder NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
