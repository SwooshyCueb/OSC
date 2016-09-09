#include "shoppingcart.h"
#include "product.h"
#include <iostream>
#include <algorithm>

ShoppingCart::ShoppingCart()
{

}
ShoppingCart::ShoppingCart(Product p){
    cart.push_back(p);
    product_count[p.getUPC()] = 1;

}
int ShoppingCart::addProduct(Product p) {


    if (ShoppingCart::checkCart(p) <  1) {
        cart.push_back(p);
        product_count[p.getUPC()] = 1;
    }
    else {
        product_count[p.getUPC()] = product_count[p.getUPC()] + 1;
    }

    return 1;
}

int ShoppingCart::getCount(Product p) {
    return product_count.count(p.getUPC());
}

int ShoppingCart::checkCart(Product p) {
    if (product_count.count(p.getUPC()) > 0) {
        return 1;
    }
    else {
        return -1;
    }
}

int ShoppingCart::emptyCart() {
    cart.clear();
    product_count.clear();
    std::cout << "You have emptied your shopping cart." << std::endl;
    return 1;
}

int ShoppingCart::deleteProduct(Product p) {
    if (product_count.count(p.getUPC()) < 1) {
        return -1;
    }
    else if (product_count.count(p.getUPC()) >= 1) {
        product_count.erase(p.getUPC());


        for (int i = 0; i < cart.size(); i++) {
            if (p.getUPC() == cart[i].getUPC()) {
                cart.erase(cart.begin() + i);
                return 1;
            }
        }

        return 1;
    }
}

void ShoppingCart::print() {
    if (cart.size() == 0) {
        std::cout << "Your shopping cart is empty." << std::endl;
    }
    else {
        std::cout << "Product\t UPC\t Catagory\t Price\t Quantity" << std::endl;
        for (int i = 0; i < cart.size(); i++) {
            cart[i].print();
            std::cout << "\t " << product_count[cart[i].getUPC()] << std::endl;
        }
    }
}
