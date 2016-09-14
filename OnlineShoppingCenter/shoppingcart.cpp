#include "shoppingcart.h"
#include "transaction.h"
#include "storagesystem.h"

using namespace std;

ShoppingCart::ShoppingCart()
{

}

int ShoppingCart::addProduct(Product p) {

    if (!ShoppingCart::checkCart(p))
        cart[p.getUPC()] = make_pair(p, 1);

    return 1;
}

int ShoppingCart::addProduct(Product p, unsigned int qty) {
    if (qty == 0) {
        deleteProduct(p);
    }
    cart[p.getUPC()] = make_pair(p, qty);
    return 1;
}

int ShoppingCart::addProduct(SKU UPC, unsigned int qty) {
    Product p = globals::local_storage.getProduct(UPC);
    return addProduct(p, qty);
}

int ShoppingCart::addProduct(SKU UPC) {
    Product p = globals::local_storage.getProduct(UPC);
    return addProduct(p, 1);
}

int ShoppingCart::deleteProduct(Product p) {
    cart.erase(p.getUPC());
    return 1;
}

int ShoppingCart::deleteProduct(SKU UPC) {
    cart.erase(UPC);
    return 1;
}

int ShoppingCart::changeQuantity(Product p, unsigned int qty) {
    return addProduct(p, qty);
}

int ShoppingCart::changeQuantity(SKU UPC, unsigned int qty) {
    Product p = globals::local_storage.getProduct(UPC);
    return addProduct(p, qty);
}

unsigned int ShoppingCart::getCount(Product p) {
    try {
        return cart.at(p.getUPC()).second;
    } catch (const out_of_range &e) {
        return 0;
    }
}

unsigned int ShoppingCart::getCount(SKU UPC) {
    try {
        return cart.at(UPC).second;
    } catch (const out_of_range &e) {
        return 0;
    }
}

bool ShoppingCart::checkCart(Product p) {
    try {
        if (cart.at(p.getUPC()).second > 0) {
            return true;
        }
    } catch (const out_of_range &e) {
        return false;
    }
    return false;
}

bool ShoppingCart::checkCart(SKU UPC) {
    try {
        if (cart.at(UPC).second > 0) {
            return true;
        }
    } catch (const out_of_range &e) {
        return false;
    }
    return false;
}

bool ShoppingCart::isEmpty() {
    return cart.empty();
}

int ShoppingCart::emptyCart() {
    cart.clear();
    return 1;
}

void ShoppingCart::print() {
    if (cart.empty()) {
        cout << "Your shopping cart is empty." << endl;
    }
    else {
        cout << "Product\t UPC\t Catagory\t Price\t Quantity" << endl;
        for (auto p : cart) {
            p.second.first.print();
            cout << "\t " << p.second.second << endl;
        }
    }
}

Transaction ShoppingCart::buyCart() {
    // Make sure we're actually the right cart
    if (this != &(globals::logged_in.shopping_cart)) {
        throw runtime_error("buyCart method called on non-user ShoppingCart.");
    }

    // Make sure quantities are valid
    for (auto p : cart) {
        if (p.second.first.getQuantity() < p.second.second) {
            throw runtime_error("buyCart method called on cart containing invalid quantities.");
        }
        //p.second.first.setQuantity(p.second.first.getQuantity() - p.second.second);
        // This should actually only be done once the transaction has been finalized.
    }

    //make transaction
    Transaction t = Transaction(*this);

    // create new empty cart for logged in user.
    ShoppingCart new_sc;
    globals::logged_in.shopping_cart = new_sc;

    return t;
}
