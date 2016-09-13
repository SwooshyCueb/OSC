#ifndef SHOPPINGCART_H
#define SHOPPINGCART_H

#include "common.h"
#include "product.h"

#include <map>
#include <iostream>

using namespace std;

class ShoppingCart
{
public:
    ShoppingCart();
    int addProduct(Product p);
    int addProduct(Product p, unsigned int qty);
    int deleteProduct(Product p);
    int addProduct(SKU UPC);
    int addProduct(SKU UPC, unsigned int qty);
    int deleteProduct(SKU UPC);
    int emptyCart();
    void print();
    bool checkCart(Product p);
    bool checkCart(SKU UPC);
    unsigned int getCount(Product p);
    unsigned int getCount(SKU UPC);
    int changeQuantity(Product p, unsigned int qty);
    int changeQuantity(SKU UPC, unsigned int qty);
    int buyCart();

    map<SKU, pair<Product, unsigned int>> cart;

};

#endif // SHOPPINGCART_H
