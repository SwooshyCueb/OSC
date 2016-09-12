#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "common.h"
#include "shoppingcart.h"
#include "shippingaddress.h"
#include "paymentinfo.h"

#include "uuid/uuid.h"

class Transaction
{
public:
    Transaction();
    Transaction(ShoppingCart cart);
    Transaction(ShoppingCart cart, uuid_t id);
    void chargeCreditCart(float amt);
    void changeProductQuantity(int a, int b);
    void addTransactionToHistory(std::string s, int a);


private:
    uuid_t transaction_id;
    std::string transaction_date;
    ShoppingCart shopping_cart;
    float transaction_amount;

};

#endif // TRANSACTION_H
