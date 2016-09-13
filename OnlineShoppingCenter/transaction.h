#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "common.h"
#include "shoppingcart.h"
#include "shippingaddress.h"
#include "paymentinfo.h"

using namespace std;
class Transaction {
    public:
        Transaction();
        Transaction(ShoppingCart cart, PaymentInfo pay);
        char chargeCreditCard(float amt);
        char setTransactionDate(string date);
        char setShoppingCart(ShoppingCart cart);
        char setTransactionAmt(float amt);
        char setPaymentInfo(PaymentInfo info);
        char setShippingAddress(ShippingAddress addr);

    private:
        int transaction_id;
        string transaction_date;
        ShoppingCart shopping_cart;
        float transaction_amount;
        PaymentInfo payment_info;
        ShippingAddress shipping_address;

};

#endif // TRANSACTION_H
