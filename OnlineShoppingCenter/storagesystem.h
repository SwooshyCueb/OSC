#ifndef STORAGESYSTEM_H
#define STORAGESYSTEM_H

#include "user.h"
#include "paymentinfo.h"
#include "product.h"
#include "shippingaddress.h"
#include "shoppingcart.h"
#include "transactionhistory.h"
//#include "transaction.h"
#include "common.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <rocksdb/db.h>
#pragma GCC diagnostic warning "-Wunused-parameter"

#define rocksErr rocksdb::Status
#define DB rocksdb::DB
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

#include <vector>

using namespace std;

class StorageSystem
{
public:
    StorageSystem();

    User getUser(string username);
    int storeUser(User user);

    int storeProduct(Product product);

    /*
    Transaction getTransaction(unsigned __int128);
    int storeTransaction(Transaction transaction);
    */

    int initDB();
private:
    DBOptions rocks_db_cfg;
    DBEntryOptions rocks_entry_cfg;
    rocksOptions rocks_cfg;

};

#endif // STORAGESYSTEM_H
