#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <uuid/uuid.h>
#include <ctime>
#include "common.h"
#include "shoppingcart.h"
#include "shippingaddress.h"
#include "paymentinfo.h"

using namespace std;
class Transaction {
    public:
        Transaction();
        Transaction(ShoppingCart cart, PaymentInfo pay);
        Transaction(ShoppingCart cart, uuid_t id);
        Transaction(ShoppingCart cart);
        int chargeCreditCard();
        int setTransactionDate(tm date);
        int setShoppingCart(ShoppingCart cart);
        int setTransactionAmt(float amt);
        int setPaymentInfo(PaymentInfo info);
        int setShippingAddress(ShippingAddress addr);

        uuid_t transaction_id;
        tm transaction_date;
        ShoppingCart shopping_cart;
        float transaction_amount;
        PaymentInfo payment_info;
        ShippingAddress shipping_address;
        bool is_finalized = false;

};
#endif // TRANSACTION_H
