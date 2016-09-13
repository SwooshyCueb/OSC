#ifndef USER_H
#define USER_H

#include "common.h"
#include "transactionhistory.h"
#include "paymentinfo.h"
#include "shippingaddress.h"
#include <iostream>

using namespace std;

class User
{
public:
    User();
    User(string new_user_name, TransactionHistory new_transaction_history);

    int changeCreditCard(PaymentInfo new_cc);
    int changeShippingAddress(ShippingAddress new_shipping_address);

    string user_name;
    ShippingAddress shipping_address;
    PaymentInfo payment_info;
    int transaction_history_id;
    TransactionHistory transaction_history;
    ShoppingCart shopping_cart;
    bool is_set = false;

};

#endif // USER_H
