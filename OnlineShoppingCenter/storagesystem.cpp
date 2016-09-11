#include "storagesystem.h"

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <glib.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <rocksdb/db.h>
#pragma GCC diagnostic warning "-Wunused-parameter"

using namespace std;

StorageSystem::StorageSystem() {
    // Check if database directory exists
    if(access("./OSCdb/", F_OK) != 0) {
        if (errno == ENOENT) {
            // Create database directory tree
            g_mkdir_with_parents("./OSCdb/users",
                                 S_IRWXU | S_IRGRP | S_IRGRP | S_IRWXU | S_IXOTH);
            g_mkdir_with_parents("./OSCdb/products",
                                 S_IRWXU | S_IRGRP | S_IRGRP | S_IRWXU | S_IXOTH);
            g_mkdir_with_parents("./OSCdb/transactions",
                                 S_IRWXU | S_IRGRP | S_IRGRP | S_IRWXU | S_IXOTH);

        } else if (errno == ENOTDIR) {
            g_printerr("FATAL: Problem accessing database directory.\n");
            exit(1);
        }
    }
}
