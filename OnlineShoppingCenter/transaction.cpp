#include "transaction.h"
#include <string>

using namespace std;

Transaction::Transaction(ShoppingCart cart, PaymentInfo pay) {
    this->shopping_cart = cart;
    this->payment_info = pay;
    uuid_generate(transaction_id);
}

Transaction::Transaction(ShoppingCart cart, uuid_t id) {
    this->shopping_cart = cart;
    uuid_copy(transaction_id, id);
    // Calculate total, store in transaction_amount
}
Transaction::Transaction(ShoppingCart cart) {
    shopping_cart = cart;
    uuid_generate(transaction_id);
    // Calculate total, store in transaction_amount
}

char Transaction::chargeCreditCard() {
    time_t t = time(NULL);
    transaction_date = *(gmtime(&t));
    is_finalized = true;
    // update product quantity in-stock
    return 1;
}

char Transaction::setTransactionDate(tm date) {
    this->transaction_date = date;
    return 1;
}

char Transaction::setShoppingCart(ShoppingCart cart) {
    this->shopping_cart = cart;
    return 1;
}

char Transaction::setTransactionAmt(float amt) {
    this->transaction_amount = amt;
    return 1;
}

char Transaction::setPaymentInfo(PaymentInfo info) {
    this->payment_info = info;
    return 1;
}

char Transaction::setShippingAddress(ShippingAddress addr) {
    this->shipping_address = addr;
    return 1;
}
