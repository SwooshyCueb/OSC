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
        char chargeCreditCard(float amt);
        char setTransactionDate(string date);
        char setShoppingCart(ShoppingCart cart);
        char setTransactionAmt(float amt);
        char setPaymentInfo(PaymentInfo info);
        char setShippingAddress(ShippingAddress addr);

    private:
        uuid_t transaction_id;
        tm transaction_date;
        ShoppingCart shopping_cart;
        float transaction_amount;
        PaymentInfo payment_info;
        ShippingAddress shipping_address;
        bool is_finalized = false;

#endif // TRANSACTION_H
