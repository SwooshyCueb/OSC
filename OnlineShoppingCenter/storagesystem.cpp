#include "storagesystem.h"

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

    // Get transaction history
    TransactionHistory orders;
    err = userdb->Get(DBReadOptions(), entry_handles[1], "orders", &dbval);
    vector<string> transaction_ids = split(dbval, '\n');
    // Get transactions and add them to orders here

    User user(username, orders);

    err = userdb->Get(DBReadOptions(), entry_handles[1], "items_in_cart",
                      &dbval);
    // use dbval to create shopping cart

    ShippingAddress addr;

    err = userdb->Get(DBReadOptions(), entry_handles[1], "shipaddr_name",
                      &dbval);
    if (err.ok())
        addr.changeShippingName(dbval);
    err = userdb->Get(DBReadOptions(), entry_handles[1], "shipaddr_street",
                      &dbval);
    if (err.ok())
        addr.changeStreet(dbval);
    err = userdb->Get(DBReadOptions(), entry_handles[1], "shipaddr_city",
                      &dbval);
    if (err.ok())
        addr.changeCity(dbval);
    err = userdb->Get(DBReadOptions(), entry_handles[1], "shipaddr_state",
                      &dbval);
    if (err.ok())
        addr.changeState(dbval);
    err = userdb->Get(DBReadOptions(), entry_handles[1], "shipaddr_zip",
                      &dbval);
    if (err.ok())
        addr.changeZip((unsigned int)stoul(dbval));
    err = userdb->Get(DBReadOptions(), entry_handles[1], "shipaddr_country",
                      &dbval);
    if (err.ok())
        addr.changeCountry(dbval);

    user.changeShippingAddress(addr);

    err = userdb->Get(DBReadOptions(), entry_handles[1], "cc_num",
                      &dbval);
    if (err.ok()) {
        unsigned long cc_num = stoul(dbval);
        err = userdb->Get(DBReadOptions(), entry_handles[1], "cc_exp",
                          &dbval);
        if (err.ok()) {
            string cc_exp = dbval;
            err = userdb->Get(DBReadOptions(), entry_handles[1], "cc_exp",
                              &dbval);
            if (err.ok()) {
                unsigned int cc_cv2 = (unsigned int)stoul(dbval);
                PaymentInfo cc(cc_num, cc_exp, cc_cv2);
                user.changeCreditCard(cc);
            }
        }
    }

    return user;
}

int StorageSystem::initDB() {
    rocksErr err;
    vector<DBEntryDescriptor> entry_descriptors;
    vector<DBEntryHandle*> entry_handles;
    DB *userdb;
    DB *productdb;

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

    DBWriteBatch user_batch;
    user_batch.Put(entry_handles[1], "items_in_cart", "");
    user_batch.Put(entry_handles[1], "shipaddr_name", "Test User, I");
    user_batch.Put(entry_handles[1], "shipaddr_street", "56 Park Circle");
    user_batch.Put(entry_handles[1], "shipaddr_city", "Starkville");
    user_batch.Put(entry_handles[1], "shipaddr_state", "MS");
    user_batch.Put(entry_handles[1], "shipaddr_zip", "39759");
    user_batch.Put(entry_handles[1], "shipaddr_country", "USA");
    user_batch.Put(entry_handles[1], "cc_num", "5174928299721390");
    user_batch.Put(entry_handles[1], "cc_exp", "06/20");
    user_batch.Put(entry_handles[1], "cc_cv2", "040");
    user_batch.Put(entry_handles[1], "orders",
                                     "a4b1288b-606f-4437-a9d1-19834fa36953\n"
                                     "c0fad0d6-1e93-4f2f-9e92-1f1824c7bfcc\n"
                                     "73536a85-eafc-4dfb-946f-9e56f58e73be\n"
                                     "37781af8-21e9-41cb-b4cf-f23ae8fa6825\n"
                                     "7217a7bd-5be3-4ce3-8e51-afa4c640a1de\n"
                                     "0ca93973-a29e-4da9-bcad-c4a35b09b3c9\n");

    user_batch.Put(entry_handles[2], "items_in_cart", "999992778878:2\n"
                                                      "751492561585:2\n"
                                                      "630125948637:1");
    user_batch.Put(entry_handles[2], "shipaddr_name", "Test User, II");
    user_batch.Put(entry_handles[2], "shipaddr_street", "424-2529 Wark St.");
    user_batch.Put(entry_handles[2], "shipaddr_city", "Victoria");
    user_batch.Put(entry_handles[2], "shipaddr_state", "BC");
    user_batch.Put(entry_handles[2], "shipaddr_zip", "V8T4G7");
    user_batch.Put(entry_handles[2], "shipaddr_country", "CA");
    user_batch.Put(entry_handles[2], "orders", "");

    user_batch.Put(entry_handles[3], "items_in_cart", "");
    user_batch.Put(entry_handles[3], "orders", "");

    user_batch.Put(entry_handles[4], "items_in_cart", "");
    user_batch.Put(entry_handles[4], "orders", "");
    userdb->Write(DBWriteOptions(), &user_batch);

    for (auto entry : entry_handles) {
        delete entry;
    }
    delete userdb;

    entry_descriptors.clear();

    DB::Open(rocks_cfg, "./OSCdb/products", &productdb);

    DBEntryHandle *testproduct;
    // Bop It
    userdb->CreateDBEntry(rocks_entry_cfg, "653569401025", &testproduct);
    delete testproduct;
    // Mass Effect: Ascension
    userdb->CreateDBEntry(rocks_entry_cfg, "9780345507754", &testproduct);
    delete testproduct;
    // AAA Duracel Batteries (Pack of 60)
    userdb->CreateDBEntry(rocks_entry_cfg, "630125948637", &testproduct);
    delete testproduct;
    // Memorex DVD-R 16x (Spindle of 100)
    userdb->CreateDBEntry(rocks_entry_cfg, "999992778878", &testproduct);
    delete testproduct;
    // PNY Attaché USB 2.0 Flash Drive 128GB
    userdb->CreateDBEntry(rocks_entry_cfg, "751492561585", &testproduct);
    delete testproduct;

    delete productdb;

    entry_descriptors.push_back(DBEntryDescriptor(kDefaultDBEntry,
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor("653569401025",
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor("9780345507754",
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor("630125948637",
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor("999992778878",
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor("751492561585",
                                                  rocks_entry_cfg));

    DB::Open(rocks_db_cfg, "./OSCdb/products", entry_descriptors,
             &entry_handles, &productdb);

    DBWriteBatch product_batch;
    product_batch.Put(entry_handles[1], "name", "Bop It");
    product_batch.Put(entry_handles[1], "category", "Children's Toys");
    product_batch.Put(entry_handles[1], "price", "23.80");
    product_batch.Put(entry_handles[1], "on_hand", "1");

    product_batch.Put(entry_handles[2], "name",
                                        "Mass Effect: Ascension (Paperback)");
    product_batch.Put(entry_handles[2], "category", "Books");
    product_batch.Put(entry_handles[2], "price", "6.99");
    product_batch.Put(entry_handles[2], "on_hand", "6");

    product_batch.Put(entry_handles[3], "name",
                                        "AAA Duracel Batteries (Pack of 60)");
    product_batch.Put(entry_handles[3], "category", "Household Items");
    product_batch.Put(entry_handles[3], "price", "22.19");
    product_batch.Put(entry_handles[3], "on_hand", "12");

    product_batch.Put(entry_handles[4], "name",
                                        "Memorex DVD-R 16x (Spindle of 100)");
    product_batch.Put(entry_handles[4], "category", "Household Items");
    product_batch.Put(entry_handles[4], "price", "27.89");
    product_batch.Put(entry_handles[4], "on_hand", "23");

    product_batch.Put(entry_handles[5],
                      "name", "PNY Attaché USB 2.0 Flash Drive 128GB");
    product_batch.Put(entry_handles[5], "category", "Small Electronics");
    product_batch.Put(entry_handles[5], "price", "31.99");
    product_batch.Put(entry_handles[5], "on_hand", "21");

    productdb->Write(DBWriteOptions(), &product_batch);

    for (auto entry : entry_handles) {
        delete entry;
    }
    delete productdb;

    entry_descriptors.clear();

    return 1;
}
