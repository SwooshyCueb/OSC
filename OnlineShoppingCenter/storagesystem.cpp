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

int StorageSystem::storeUser(User user) {
    rocksErr err;
    vector<DBEntryDescriptor> entry_descriptors;
    vector<DBEntryHandle*> entry_handles;
    DB *userdb;

    vector<string> username_sv;
    err = DBgetEntries(rocks_db_cfg, "./OSCdb/users", &username_sv);
    list<string> username_sl(username_sv.begin(), username_sv.end());

    username_sl.remove(kDefaultDBEntry);
    username_sl.remove(user.user_name);
    entry_descriptors.push_back(DBEntryDescriptor(kDefaultDBEntry,
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor(user.user_name,
                                                  rocks_entry_cfg));

    for (auto username_s : username_sl) {
        entry_descriptors.push_back(DBEntryDescriptor(username_s,
                                                      rocks_entry_cfg));
    }

    DB::Open(rocks_db_cfg, "./OSCdb/users", entry_descriptors,
             &entry_handles, &userdb);

    DBWriteBatch user_batch;

    if (user.shipping_address.parts_set && S_NAME_SET) {
        user_batch.Put(entry_handles[1], "shipaddr_name",
                                         user.shipping_address.shipping_name);
    }
    if (user.shipping_address.parts_set && S_STREET_SET) {
        user_batch.Put(entry_handles[1], "shipaddr_street",
                                         user.shipping_address.street);
    }
    if (user.shipping_address.parts_set && S_CITY_SET) {
        user_batch.Put(entry_handles[1], "shipaddr_city",
                                         user.shipping_address.city);
    }
    if (user.shipping_address.parts_set && S_STATE_SET) {
        user_batch.Put(entry_handles[1], "shipaddr_state",
                                         user.shipping_address.state);
    }
    if (user.shipping_address.parts_set && S_ZIP_SET) {
        user_batch.Put(entry_handles[1], "shipaddr_zip",
                                         to_string(user.shipping_address.zip));
    }
    if (user.shipping_address.parts_set && S_COUNTRY_SET) {
        user_batch.Put(entry_handles[1], "shipaddr_country",
                                         user.shipping_address.country);
    }

    if (user.payment_info.is_set == true) {
        user_batch.Put(entry_handles[1], "cc_num",
                                         to_string(user.payment_info.cc_num));
        user_batch.Put(entry_handles[1], "cc_exp",
                                         user.payment_info.cc_exp);
        user_batch.Put(entry_handles[1], "cc_cv2",
                                         to_string(user.payment_info.cc_cv2));
    }

    string orders_s = "";
    char tid_cs[37];
    string tid_s;
    for (auto order : user.transaction_history.transaction_list) {
        if (order.is_finalized == false)
            continue;

        if (orders_s.empty()) {
            orders_s.append("\n");
        }

        uuid_unparse_lower(order.transaction_id, (char *)tid_cs);
        tid_s.assign((char *)tid_cs);
        orders_s.append(tid_s);
    }
    user_batch.Put(entry_handles[1], "orders", orders_s);

    string cart_s = "";
    for (auto item : user.shopping_cart.cart) {
        if (!cart_s.empty()) {
            cart_s.append("\n");
        }
        cart_s.append(to_string(item.first));
        cart_s.append(":");
        cart_s.append(to_string(item.second.second));
    }
    user_batch.Put(entry_handles[1], "items_in_cart", cart_s);

    userdb->Write(DBWriteOptions(), &user_batch);

    for (auto entry : entry_handles) {
        delete entry;
    }
    delete userdb;

    entry_descriptors.clear();

    return 1;
}

User StorageSystem::getUser(string username) {
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
    uuid_t tid;
    err = userdb->Get(DBReadOptions(), entry_handles[1], "orders", &dbval);
    vector<string> transaction_ids = split(dbval, '\n');
    for (auto tid_s : transaction_ids) {
        uuid_parse(tid_s.c_str(), tid);
        orders.addTransaction(this->getTransaction(tid));
    }

    User user(username, orders);

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

    userdb->Get(DBReadOptions(), entry_handles[1], "items_in_cart", &dbval);
    for (auto item_ps : split(dbval, '\n')) {
        vector<string> item_pv = split(item_ps, ':');
        user.shopping_cart.addProduct(this->getProduct(stoul(item_pv[0])),
                        (unsigned int)stoul(item_pv[1]));
    }

    return user;
}

int StorageSystem::storeProduct(Product product) {
    rocksErr err;
    vector<DBEntryDescriptor> entry_descriptors;
    vector<DBEntryHandle*> entry_handles;
    DB *productdb;

    if (product.is_set == false)
        return 0;

    vector<string> upc_sv;
    err = DBgetEntries(rocks_db_cfg, "./OSCdb/products", &upc_sv);
    list<string> upc_sl(upc_sv.begin(), upc_sv.end());

    upc_sl.remove(kDefaultDBEntry);
    upc_sl.remove(to_string(product.getUPC()));
    entry_descriptors.push_back(DBEntryDescriptor(kDefaultDBEntry,
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor(to_string(product.getUPC()),
                                                  rocks_entry_cfg));

    for (auto upc_s : upc_sl) {
        entry_descriptors.push_back(DBEntryDescriptor(upc_s, rocks_entry_cfg));
    }

    DB::Open(rocks_db_cfg, "./OSCdb/products", entry_descriptors,
             &entry_handles, &productdb);

    DBWriteBatch product_batch;
    product_batch.Put(entry_handles[1], "name", product.getName());
    product_batch.Put(entry_handles[1], "category", product.getCategory());
    product_batch.Put(entry_handles[1], "price", to_string(product.getPrice()));
    product_batch.Put(entry_handles[1], "on_hand",
                                        to_string(product.getQuantity()));
    productdb->Write(DBWriteOptions(), &product_batch);

    for (auto entry : entry_handles) {
        delete entry;
    }
    delete productdb;

    entry_descriptors.clear();

    return 1;

}

Product StorageSystem::getProduct(SKU UPC) {
    rocksErr err;
    vector<DBEntryDescriptor> entry_descriptors;
    vector<DBEntryHandle*> entry_handles;
    DB *productdb;

    entry_descriptors.push_back(DBEntryDescriptor(kDefaultDBEntry,
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor(to_string(UPC),
                                                  rocks_entry_cfg));

    DB::OpenForReadOnly(rocks_db_cfg, "./OSCdb/products", entry_descriptors,
                        &entry_handles, &productdb);

    Product ret;
    ret.setUPC(UPC);

    string dbval;

    productdb->Get(DBReadOptions(), entry_handles[1], "name", &dbval);
    ret.setName(dbval);
    productdb->Get(DBReadOptions(), entry_handles[1], "category", &dbval);
    ret.setCategory(dbval);
    productdb->Get(DBReadOptions(), entry_handles[1], "price", &dbval);
    ret.setPrice(stof(dbval));
    productdb->Get(DBReadOptions(), entry_handles[1], "on_hand", &dbval);
    ret.setQuantity((unsigned int)stoul(dbval));

    for (auto entry : entry_handles) {
        delete entry;
    }
    delete productdb;

    return ret;
}

int StorageSystem::storeTransaction(Transaction transaction) {
    rocksErr err;
    vector<DBEntryDescriptor> entry_descriptors;
    vector<DBEntryHandle*> entry_handles;
    DB *orderdb;
    char id_cs[37];
    uuid_unparse_lower(transaction.transaction_id, (char *)id_cs);
    string id_s((char *)id_cs);

    // Do not store orders that were not actually placed
    if (transaction.is_finalized == false)
        return 0;

    vector<string> tid_sv;
    err = DBgetEntries(rocks_db_cfg, "./OSCdb/orders", &tid_sv);
    list<string> tid_sl(tid_sv.begin(), tid_sv.end());

    tid_sl.remove(kDefaultDBEntry);
    tid_sl.remove(id_s);
    entry_descriptors.push_back(DBEntryDescriptor(kDefaultDBEntry,
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor(id_s,
                                                  rocks_entry_cfg));

    for (auto tid_s : tid_sl) {
        entry_descriptors.push_back(DBEntryDescriptor(tid_s, rocks_entry_cfg));
    }

    DB::Open(rocks_db_cfg, "./OSCdb/orders", entry_descriptors,
             &entry_handles, &orderdb);

    DBWriteBatch order_batch;

    char time_cs[81];
    strftime((char *)time_cs, 81, "%a, %d %b %Y %H:%M:%S %z",
             &transaction.transaction_date);
    string time_s((char *)time_cs);
    order_batch.Put(entry_handles[1], "date", time_s);

    string cart_s = "";
    for (auto item :transaction.shopping_cart.cart) {
        if (!cart_s.empty()) {
            cart_s.append("\n");
        }
        cart_s.append(to_string(item.first));
        cart_s.append(":");
        cart_s.append(to_string(item.second.second));
    }
    order_batch.Put(entry_handles[1], "items", cart_s);

    order_batch.Put(entry_handles[1], "total",
                    to_string(transaction.transaction_amount));

    order_batch.Put(entry_handles[1], "shipaddr_name",
                    transaction.shipping_address.shipping_name);
    order_batch.Put(entry_handles[1], "shipaddr_street",
                                      transaction.shipping_address.street);
    order_batch.Put(entry_handles[1], "shipaddr_city",
                                      transaction.shipping_address.city);
    order_batch.Put(entry_handles[1], "shipaddr_state",
                                      transaction.shipping_address.state);
    order_batch.Put(entry_handles[1], "shipaddr_zip",
                    to_string(transaction.shipping_address.zip));
    order_batch.Put(entry_handles[1], "shipaddr_country",
                                      transaction.shipping_address.country);
    order_batch.Put(entry_handles[1], "cc_num",
                    to_string(transaction.payment_info.cc_num));
    order_batch.Put(entry_handles[1], "cc_exp",
                                      transaction.payment_info.cc_exp);
    order_batch.Put(entry_handles[1], "cc_cv2",
                    to_string(transaction.payment_info.cc_cv2));

    orderdb->Write(DBWriteOptions(), &order_batch);

    for (auto entry : entry_handles) {
        delete entry;
    }
    delete orderdb;

    entry_descriptors.clear();

    return 1;
}

Transaction StorageSystem::getTransaction(uuid_t id) {
    rocksErr err;
    vector<DBEntryDescriptor> entry_descriptors;
    vector<DBEntryHandle*> entry_handles;
    DB *orderdb;
    char id_cs[37];
    uuid_unparse_lower(id, (char *)id_cs);
    string id_s((char *)id_cs);

    entry_descriptors.push_back(DBEntryDescriptor(kDefaultDBEntry,
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor(id_s, rocks_entry_cfg));

    DB::OpenForReadOnly(rocks_db_cfg, "./OSCdb/orders", entry_descriptors,
                        &entry_handles, &orderdb);

    string dbval;
    ShoppingCart cart;
    orderdb->Get(DBReadOptions(), entry_handles[1], "items", &dbval);
    for (auto item_ps : split(dbval, '\n')) {
        vector<string> item_pv = split(item_ps, ':');
        cart.addProduct(this->getProduct(stoul(item_pv[0])),
                        (unsigned int)stoul(item_pv[1]));
    }

    Transaction ret(cart, id);
    ret.is_finalized = true;

    tm t_date;
    orderdb->Get(DBReadOptions(), entry_handles[1], "date", &dbval);
    strptime(dbval.c_str(), "%a, %d %b %Y %H:%M:%S %z", &t_date);
    ret.setTransactionDate(t_date);

    orderdb->Get(DBReadOptions(), entry_handles[1], "total", &dbval);
    ret.setTransactionAmt(stof(dbval));

    ShippingAddress addr;
    orderdb->Get(DBReadOptions(), entry_handles[1], "shipaddr_name", &dbval);
    addr.changeShippingName(dbval);
    orderdb->Get(DBReadOptions(), entry_handles[1], "shipaddr_street", &dbval);
    addr.changeStreet(dbval);
    orderdb->Get(DBReadOptions(), entry_handles[1], "shipaddr_city", &dbval);
    addr.changeCity(dbval);
    orderdb->Get(DBReadOptions(), entry_handles[1], "shipaddr_state", &dbval);
    addr.changeState(dbval);
    orderdb->Get(DBReadOptions(), entry_handles[1], "shipaddr_zip", &dbval);
    addr.changeZip((unsigned int)stoul(dbval));
    orderdb->Get(DBReadOptions(), entry_handles[1], "shipaddr_country", &dbval);
    addr.changeCountry(dbval);
    ret.setShippingAddress(addr);

    PaymentInfo cc;
    orderdb->Get(DBReadOptions(), entry_handles[1], "cc_num", &dbval);
    cc.cc_num = stoul(dbval);
    orderdb->Get(DBReadOptions(), entry_handles[1], "cc_exp", &dbval);
    cc.cc_exp = dbval;
    orderdb->Get(DBReadOptions(), entry_handles[1], "cc_cv2", &dbval);
    cc.cc_cv2 = (unsigned int)stoul(dbval);
    ret.setPaymentInfo(cc);

    for (auto entry : entry_handles) {
        delete entry;
    }
    delete orderdb;

    return ret;
}

int StorageSystem::initDB() {
    rocksErr err;
    vector<DBEntryDescriptor> entry_descriptors;
    vector<DBEntryHandle*> entry_handles;
    DB *userdb;
    DB *productdb;
    DB *orderdb;

    DestroyDB("./OSCdb/users", rocks_cfg);
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
    user_batch.Put(entry_handles[1], "cc_num", "8299721390");
    user_batch.Put(entry_handles[1], "cc_exp", "06/20");
    user_batch.Put(entry_handles[1], "cc_cv2", "040");
    user_batch.Put(entry_handles[1], "orders",
                                     "a4b1288b-606f-4437-a9d1-19834fa36953\n"
                                     "c0fad0d6-1e93-4f2f-9e92-1f1824c7bfcc\n"
                                     "73536a85-eafc-4dfb-946f-9e56f58e73be\n"
                                     "37781af8-21e9-41cb-b4cf-f23ae8fa6825");

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

    DestroyDB("./OSCdb/products", rocks_cfg);
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
    // /////////////////HOLDER
    // Staples
    userdb->CreateDBEntry(rocks_entry_cfg, "653562346025", &testproduct);
    delete testproduct;
    // Corn
    userdb->CreateDBEntry(rocks_entry_cfg, "978091685754", &testproduct);
    delete testproduct;
    // Bread
    userdb->CreateDBEntry(rocks_entry_cfg, "630037948637", &testproduct);
    delete testproduct;
    // paper
    userdb->CreateDBEntry(rocks_entry_cfg, "945682378878", &testproduct);
    delete testproduct;
    // Pens
    userdb->CreateDBEntry(rocks_entry_cfg, "452368735260", &testproduct);
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
    //Holder
    entry_descriptors.push_back(DBEntryDescriptor("653562346025",
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor("978091685754",
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor("630037948637",
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor("945682378878",
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor("452368735260",
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

    //HOLDER
    DBWriteBatch product_batch;
    product_batch.Put(entry_handles[6], "name", "Staples");
    product_batch.Put(entry_handles[6], "category", "Household Items");
    product_batch.Put(entry_handles[6], "price", "2.00");
    product_batch.Put(entry_handles[6], "on_hand", "89");

    product_batch.Put(entry_handles[7], "name", "Corn");
    product_batch.Put(entry_handles[7], "category", "Household Items");
    product_batch.Put(entry_handles[7], "price", "6.99");
    product_batch.Put(entry_handles[7], "on_hand", "7");

    product_batch.Put(entry_handles[8], "name", "Bread");
    product_batch.Put(entry_handles[8], "category", "Household Items");
    product_batch.Put(entry_handles[8], "price", "1.19");
    product_batch.Put(entry_handles[8], "on_hand", "12");

    product_batch.Put(entry_handles[9], "name", "Paper");
    product_batch.Put(entry_handles[9], "category", "Household Items");
    product_batch.Put(entry_handles[9], "price", "0.99");
    product_batch.Put(entry_handles[9], "on_hand", "23");

    product_batch.Put(entry_handles[10], "name", "Pens");
    product_batch.Put(entry_handles[10], "category", "Household Items");
    product_batch.Put(entry_handles[10], "price", "2.99");
    product_batch.Put(entry_handles[10], "on_hand", "21");

    productdb->Write(DBWriteOptions(), &product_batch);

    for (auto entry : entry_handles) {
        delete entry;
    }
    delete productdb;

    entry_descriptors.clear();

    DestroyDB("./OSCdb/orders", rocks_cfg);
    DB::Open(rocks_cfg, "./OSCdb/orders", &orderdb);

    DBEntryHandle *testorder;
    orderdb->CreateDBEntry(rocks_entry_cfg,
                          "a4b1288b-606f-4437-a9d1-19834fa36953", &testorder);
    delete testorder;
    orderdb->CreateDBEntry(rocks_entry_cfg,
                          "c0fad0d6-1e93-4f2f-9e92-1f1824c7bfcc", &testorder);
    delete testorder;
    orderdb->CreateDBEntry(rocks_entry_cfg,
                          "73536a85-eafc-4dfb-946f-9e56f58e73be", &testorder);
    delete testorder;
    orderdb->CreateDBEntry(rocks_entry_cfg,
                          "37781af8-21e9-41cb-b4cf-f23ae8fa6825", &testorder);
    delete testorder;

    delete orderdb;

    entry_descriptors.push_back(DBEntryDescriptor(kDefaultDBEntry,
                                                  rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor(
          "a4b1288b-606f-4437-a9d1-19834fa36953", rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor(
          "c0fad0d6-1e93-4f2f-9e92-1f1824c7bfcc", rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor(
          "73536a85-eafc-4dfb-946f-9e56f58e73be", rocks_entry_cfg));
    entry_descriptors.push_back(DBEntryDescriptor(
          "37781af8-21e9-41cb-b4cf-f23ae8fa6825", rocks_entry_cfg));

    DB::Open(rocks_db_cfg, "./OSCdb/orders", entry_descriptors, &entry_handles,
             &orderdb);

    DBWriteBatch order_batch;
    order_batch.Put(entry_handles[1], "date",
                                      "Tue, 02 Sep 2016 11:04:07 +0000");
    order_batch.Put(entry_handles[1], "items", "9780345507754:1");
    order_batch.Put(entry_handles[1], "total", "7.99");
    order_batch.Put(entry_handles[1], "shipaddr_name", "Test User, I");
    order_batch.Put(entry_handles[1], "shipaddr_street", "56 Park Circle");
    order_batch.Put(entry_handles[1], "shipaddr_city", "Starkville");
    order_batch.Put(entry_handles[1], "shipaddr_state", "MS");
    order_batch.Put(entry_handles[1], "shipaddr_zip", "39759");
    order_batch.Put(entry_handles[1], "shipaddr_country", "USA");
    order_batch.Put(entry_handles[1], "cc_num", "8299721390");
    order_batch.Put(entry_handles[1], "cc_exp", "06/20");
    order_batch.Put(entry_handles[1], "cc_cv2", "040");

    order_batch.Put(entry_handles[2], "date",
                                      "Tue, 06 Sep 2016 13:10:27 +0000");
    order_batch.Put(entry_handles[2], "items", "9780345507754:24");
    order_batch.Put(entry_handles[2], "total", "191.76");
    order_batch.Put(entry_handles[2], "shipaddr_name", "Test User, I");
    order_batch.Put(entry_handles[2], "shipaddr_street",
                                      "603 Yellow Jacket Dr.");
    order_batch.Put(entry_handles[2], "shipaddr_city", "Starkville");
    order_batch.Put(entry_handles[2], "shipaddr_state", "MS");
    order_batch.Put(entry_handles[2], "shipaddr_zip", "39759");
    order_batch.Put(entry_handles[2], "shipaddr_country", "USA");
    order_batch.Put(entry_handles[2], "cc_num", "5034967580");
    order_batch.Put(entry_handles[2], "cc_exp", "12/18");
    order_batch.Put(entry_handles[2], "cc_cv2", "960");

    order_batch.Put(entry_handles[3], "date",
                                      "Tue, 07 Sep 2016 16:23:26 +0000");
    order_batch.Put(entry_handles[3], "items", "751492561585:1\n"
                                               "999992778878:1");
    order_batch.Put(entry_handles[3], "total", "74.98");
    order_batch.Put(entry_handles[3], "shipaddr_name", "Test User, I");
    order_batch.Put(entry_handles[3], "shipaddr_street",
                                      "603 Yellow Jacket Dr.");
    order_batch.Put(entry_handles[3], "shipaddr_city", "Starkville");
    order_batch.Put(entry_handles[3], "shipaddr_state", "MS");
    order_batch.Put(entry_handles[3], "shipaddr_zip", "39759");
    order_batch.Put(entry_handles[3], "shipaddr_country", "USA");
    order_batch.Put(entry_handles[3], "cc_num", "8299721390");
    order_batch.Put(entry_handles[3], "cc_exp", "06/20");
    order_batch.Put(entry_handles[3], "cc_cv2", "040");

    order_batch.Put(entry_handles[4], "date",
                                      "Tue, 10 Sep 2016 14:43:31 +0000");
    order_batch.Put(entry_handles[4], "items", "630125948637:1");
    order_batch.Put(entry_handles[4], "total", "22.19");
    order_batch.Put(entry_handles[4], "shipaddr_name", "Test User, I");
    order_batch.Put(entry_handles[4], "shipaddr_street", "56 Park Circle");
    order_batch.Put(entry_handles[4], "shipaddr_city", "Starkville");
    order_batch.Put(entry_handles[4], "shipaddr_state", "MS");
    order_batch.Put(entry_handles[4], "shipaddr_zip", "39759");
    order_batch.Put(entry_handles[4], "shipaddr_country", "USA");
    order_batch.Put(entry_handles[4], "cc_num", "8299721390");
    order_batch.Put(entry_handles[4], "cc_exp", "06/20");
    order_batch.Put(entry_handles[4], "cc_cv2", "040");
    orderdb->Write(DBWriteOptions(), &order_batch);

    for (auto entry : entry_handles) {
        delete entry;
    }
    delete orderdb;

    entry_descriptors.clear();

    return 1;
}
