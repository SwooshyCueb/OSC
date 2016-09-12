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
private:
    DBOptions rocks_db_cfg;
    DBEntryOptions rocks_entry_cfg;

};

#endif // STORAGESYSTEM_H
