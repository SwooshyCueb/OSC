#ifndef COMMON_H
#define COMMON_H

#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <rocksdb/db.h>
#pragma GCC diagnostic warning "-Wunused-parameter"

#define rocksErr rocksdb::Status
#define DB rocksdb::DB
#define DBEntryDescriptor rocksdb::ColumnFamilyDescriptor
#define DBEntryHandle rocksdb::ColumnFamilyHandle
#define DBOptions rocksdb::DBOptions
#define DBEntryOptions rocksdb::ColumnFamilyOptions
#define DBReadOptions rocksdb::ReadOptions
#define kDefaultDBEntry rocksdb::kDefaultColumnFamilyName

#endif // COMMON_H
