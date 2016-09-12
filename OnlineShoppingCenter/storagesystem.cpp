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
    entry_descriptors.push_back(DBEntryDescriptor(username,
                                                  rocks_entry_cfg));

    err = DB::OpenForReadOnly(rocks_db_cfg, "./OSCdb/users",
                              entry_descriptors , &entry_handles, &userdb);
    if (!err.ok()) {
        throw system_error(int(err.code()), generic_category(),
                           "Error opening user database: " + err.ToString());
    }

    err = userdb->Get(DBReadOptions(), entry_handles[1], "cc_num", &dbval);


}
