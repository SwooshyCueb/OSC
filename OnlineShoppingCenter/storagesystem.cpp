#include "storagesystem.h"
#include "common.h"

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <glib.h>
#include <system_error>
#include <vector>

#define DIRPERMS S_IRWXU | S_IRGRP | S_IRGRP | S_IRWXU | S_IXOTH

using namespace std;

StorageSystem::StorageSystem() {
    // Check if database directory exists
    if(access("./OSCdb/", F_OK) != 0) {
        if (errno == ENOENT) {
            // Create database directory
            g_mkdir_with_parents("./OSCdb", DIRPERMS);
        } else if (errno == ENOTDIR) {
            g_printerr("FATAL: Problem accessing database directory.\n");
            exit(1);
        }
    }

    rocks_db_cfg.OptimizeForSmallDb();
    rocks_db_cfg.create_if_missing = true;
    rocks_db_cfg.create_missing_column_families = false;

    rocks_entry_cfg.OptimizeForSmallDb();

    rocks_cfg = rocksOptions(rocks_db_cfg, rocks_entry_cfg);
}

User StorageSystem::getUser(string username) {
    //if (!g_file_test(("./OSCdb/users/" + username), G_FILE_TEST_IS_REGULAR)) {
    //    throw runtime_error("User does not exist");
    //}

    rocksErr err;
    DB *userdb;
    vector<DBEntryDescriptor> entry_descriptors;
    vector<DBEntryHandle*> entry_handles;

    string dbval;

    entry_descriptors.push_back(DBEntryDescriptor(kDefaultDBEntry,
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor(username, rocks_entry_cfg));

    err = DB::OpenForReadOnly(rocks_db_cfg, "./OSCdb/users",
                              entry_descriptors , &entry_handles, &userdb);
    if (!err.ok()) {
        throw system_error(int(err.code()), generic_category(),
                           "Error opening user database: " + err.ToString());
    }

    err = userdb->Get(DBReadOptions(), entry_handles[1], "cc_num", &dbval);


}

int StorageSystem::initDB() {
    rocksErr err;
    DB *userdb;

    DB::Open(rocks_cfg, "./OSCdb/users", &userdb);

    DBEntryHandle *testuser;
    userdb->CreateDBEntry(rocks_entry_cfg, "testuser001", &testuser);
    delete testuser;
    userdb->CreateDBEntry(rocks_entry_cfg, "testuser002", &testuser);
    delete testuser;
    userdb->CreateDBEntry(rocks_entry_cfg, "testuser003", &testuser);
    delete testuser;
    userdb->CreateDBEntry(rocks_entry_cfg, "testuser004", &testuser);
    delete testuser;

    delete userdb;

    vector<DBEntryDescriptor> entry_descriptors;
    vector<DBEntryHandle*> entry_handles;

    entry_descriptors.push_back(DBEntryDescriptor(kDefaultDBEntry,
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor("testuser001",
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor("testuser002",
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor("testuser003",
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor("testuser004",
                                                  rocks_entry_cfg));

    DB::Open(rocks_db_cfg, "./OSCdb/users", entry_descriptors, &entry_handles,
             &userdb);

    DBWriteBatch batch;
    batch.Put(entry_handles[1], "items_in_cart", "");
    batch.Put(entry_handles[1], "shipaddr_name", "Test User, I");
    batch.Put(entry_handles[1], "shipaddr_street", "56 Park Circle");
    batch.Put(entry_handles[1], "shipaddr_city", "Starkville");
    batch.Put(entry_handles[1], "shipaddr_state", "MS");
    batch.Put(entry_handles[1], "shipaddr_zip", "39759");
    batch.Put(entry_handles[1], "shipaddr_country", "USA");
    batch.Put(entry_handles[1], "cc_num", "5174928299721390");
    batch.Put(entry_handles[1], "cc_exp", "06/20");
    batch.Put(entry_handles[1], "cc_cv2", "040");
    batch.Put(entry_handles[1], "orders",
                                "a4b1288b-606f-4437-a9d1-19834fa36953\n"
                                "c0fad0d6-1e93-4f2f-9e92-1f1824c7bfcc\n"
                                "73536a85-eafc-4dfb-946f-9e56f58e73be\n"
                                "37781af8-21e9-41cb-b4cf-f23ae8fa6825\n"
                                "7217a7bd-5be3-4ce3-8e51-afa4c640a1de\n"
                                "0ca93973-a29e-4da9-bcad-c4a35b09b3c9\n");

    batch.Put(entry_handles[1], "items_in_cart", "ISBN:9780345507754\n"
                                                 "ASIN:B00WM9G3C6\n"
                                                 "UPC:92636250744");
    batch.Put(entry_handles[2], "shipaddr_name", "Test User, II");
    batch.Put(entry_handles[2], "shipaddr_street", "424-2529 Wark St.");
    batch.Put(entry_handles[2], "shipaddr_city", "Victoria");
    batch.Put(entry_handles[2], "shipaddr_state", "BC");
    batch.Put(entry_handles[2], "shipaddr_zip", "V8T4G7");
    batch.Put(entry_handles[2], "shipaddr_country", "CA");
    batch.Put(entry_handles[2], "orders", "");

    batch.Put(entry_handles[3], "items_in_cart", "");
    batch.Put(entry_handles[3], "orders", "");

    batch.Put(entry_handles[4], "items_in_cart", "");
    batch.Put(entry_handles[4], "orders", "");
    userdb->Write(DBWriteOptions(), &batch);

    for (auto entry : entry_handles) {
        delete entry;
    }
    delete userdb;

    return 1;
}
