#ifndef STORAGESYSTEM_H
#define STORAGESYSTEM_H

#include "common.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <rocksdb/db.h>
#pragma GCC diagnostic warning "-Wunused-parameter"

#define rocksErr rocksdb::Status
#define DB rocksdb::DB
#define DestroyDB rocksdb::DestroyDB
#define DBEntryDescriptor rocksdb::ColumnFamilyDescriptor
#define DBEntryHandle rocksdb::ColumnFamilyHandle
#define DBOptions rocksdb::DBOptions
#define DBEntryOptions rocksdb::ColumnFamilyOptions
#define rocksOptions rocksdb::Options
#define DBReadOptions rocksdb::ReadOptions
#define DBWriteOptions rocksdb::WriteOptions
#define kDefaultDBEntry rocksdb::kDefaultColumnFamilyName
#define CreateDBEntry CreateColumnFamily
#define DBWriteBatch rocksdb::WriteBatch
#define DBgetEntries DB::ListColumnFamilies

#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <system_error>
#include <vector>
#include <sstream>
#include <list>
#include <set>
#include <cstdio>
#include <uuid/uuid.h>

#include "paymentinfo.h"
#include "shippingaddress.h"
#include "shoppingcart.h"
#include "product.h"
#include "transaction.h"
#include "transactionhistory.h"
#include "user.h"

using namespace std;

class StorageSystem
{
public:
    StorageSystem();

    User getUser(string username);
    int storeUser(User user);

    Product getProduct(SKU UPC);
    int storeProduct(Product product);
    vector<SKU> getProducts();

    Transaction getTransaction(uuid_t id);
    int storeTransaction(Transaction transaction);

    int initDB();
private:
    DBEntryOptions rocks_entry_cfg;

    DBOptions rocks_db_cfg_r;
    rocksOptions rocks_cfg_r;

    DBOptions rocks_db_cfg_w;
    rocksOptions rocks_cfg_w;

};

#endif // STORAGESYSTEM_H
