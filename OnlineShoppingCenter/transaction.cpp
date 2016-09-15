#include "transaction.h"
#include <string>
#include "storagesystem.h"

using namespace std;

Transaction::Transaction() {

}

Transaction::Transaction(ShoppingCart cart, PaymentInfo pay) {
    this->shopping_cart = cart;
    this->payment_info = pay;
    uuid_generate(transaction_id);
    transaction_amount = 0;
    for (auto item : cart.cart) {
        transaction_amount += item.second.first.getPrice() * float(item.second.second);
    }
}

Transaction::Transaction(ShoppingCart cart, uuid_t id) {
    this->shopping_cart = cart;
    uuid_copy(transaction_id, id);
    transaction_amount = 0;
    for (auto item : cart.cart) {
        transaction_amount += item.second.first.getPrice() * float(item.second.second);
    }
}
Transaction::Transaction(ShoppingCart cart) {
    shopping_cart = cart;
    uuid_generate(transaction_id);
    transaction_amount = 0;
    for (auto item : cart.cart) {
        transaction_amount += item.second.first.getPrice() * float(item.second.second);
    }
}

char Transaction::chargeCreditCard() {
    time_t t = time(NULL);
    transaction_date = *(gmtime(&t));
    is_finalized = true;
    for (auto p : shopping_cart.cart) {
        if (p.second.first.getQuantity() < p.second.second) {
            throw runtime_error("Cannot finalize a transaction with invalid quantities.");
        }
        p.second.first.setQuantity(p.second.first.getQuantity() - p.second.second);
        globals::local_storage.storeProduct(p.second.first);
    }
    globals::local_storage.storeTransaction(*this);

    ShoppingCart new_sc;
    globals::logged_in.shopping_cart = new_sc;
    globals::logged_in.transaction_history.addTransaction(*this);

    globals::local_storage.storeUser(globals::logged_in);
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
