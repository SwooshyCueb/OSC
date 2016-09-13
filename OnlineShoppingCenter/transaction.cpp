#include "transaction.h"
#include <string>

using namespace std;

Transaction::Transaction(ShoppingCart cart) {
    shopping_cart = cart;
    uuid_generate(transaction_id);
    // Calculate total, store in transaction_amount
}

Transaction::Transaction(ShoppingCart cart, uuid_t id) {
    shopping_cart = cart;
    uuid_copy(transaction_id, id);
    // Calculate total, store in transaction_amount
}

void Transaction::chargeCreditCard() {
    time_t t = time(NULL);
    transaction_date = *(gmtime(&t));
    is_finalized = true;
    // update product quantity in-stock
}

