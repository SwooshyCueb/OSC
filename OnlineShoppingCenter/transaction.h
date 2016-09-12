#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "common.h"
#include "shoppingcart.h"
#include "shippingaddress.h"
#include "paymentinfo.h"

#include <uuid/uuid.h>
#include <ctime>

using namespace std;

class Transaction
{
public:
    Transaction(ShoppingCart cart);
    Transaction(ShoppingCart cart, uuid_t id);
    void chargeCreditCard();

    uuid_t transaction_id;
    tm transaction_date;
    ShoppingCart shopping_cart;
    float transaction_amount;
    bool is_finalized = false;

};

#endif // TRANSACTION_H
