#include "transaction.h"
#include <string>

using namespace std;
Transaction::Transaction()
{

}

Transaction::Transaction(ShoppingCart cart, PaymentInfo pay) {
    this->shopping_cart = cart;
    this->payment_info = pay;
}

char Transaction::chargeCreditCard(float amt) {
    this->transaction_amount = amt;
    return 1;
}


char Transaction::setTransactionDate(string date) {
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
