#include "transaction.h"
#include <string>

using namespace std;
Transaction::Transaction()
{

}

Transaction::Transaction(ShoppingCart cart) {
    this->shopping_cart = cart;
}

void Transaction::chargeCreditCart(float amt) {
    this->transaction_amount = amt;
}

void Transaction::changeProductQuantity(int a, int b) {
    ///IDK what this is tbh - if I had to guess, this is more of a shopping cart thing to deal with
}

void Transaction::addTransactionToHistory(string s, int a) {
    this->transaction_date = s;
    this->transaction_id = a;
}
