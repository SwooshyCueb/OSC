#ifndef SHOPPINGCART_H
#define SHOPPINGCART_H

#include "common.h"
#include "product.h"

#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

using namespace std;

class ShoppingCart
{
public:
    ShoppingCart();
    //ShoppingCart(Product p);

    int addProduct(Product p);
    int addProduct(Product p, unsigned int qty);
    int deleteProduct(Product p);
    int addProduct(SKU UPC); // NYI
    int addProduct(SKU UPC, unsigned int qty); // NYI
    int deleteProduct(SKU UPC);
    int emptyCart();
    void print();
    bool checkCart(Product p);
    bool checkCart(SKU UPC);
    unsigned int getCount(Product p);
    unsigned int getCount(SKU UPC);
    int changeQuantity(Product p, unsigned int qty);
    int changeQuantity(SKU UPC, unsigned int qty); // NYI
    int buyCart(); // NYI

    map<SKU, pair<Product, unsigned int>> cart;

};

#endif // SHOPPINGCART_H
