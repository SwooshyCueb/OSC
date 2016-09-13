#include "shoppingcart.h"

using namespace std;

ShoppingCart::ShoppingCart()
{

}
/*
 ShoppingCart::ShoppingCart(Product p){
    cart.push_back(p);
    product_count[p.getUPC()] = 1;

}
*/

int ShoppingCart::addProduct(Product p) {

    if (!ShoppingCart::checkCart(p))
        cart[p.getUPC()] = make_pair(p, 1);

    return 1;
}

int ShoppingCart::addProduct(Product p, unsigned int qty) {
    cart[p.getUPC()] = make_pair(p, qty);
    return 1;
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

int ShoppingCart::emptyCart() {
    cart.clear();
    std::cout << "You have emptied your shopping cart." << endl;
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

int ShoppingCart::buyCart() {
    //make transaction
    //update product quantity
    //empty cart
}
