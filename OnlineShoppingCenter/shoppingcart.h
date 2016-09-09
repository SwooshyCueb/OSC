#ifndef SHOPPINGCART_H
#define SHOPPINGCART_H

#include "product.h"
#include <vector>
#include <map>

class ShoppingCart
{
public:
    ShoppingCart();
    ShoppingCart(Product p);

    int addProduct(Product p);
    int deleteProduct(Product p);
    int deleteProduct(int UPC); // NYI
    int emptyCart();
    void print();
    int checkCart(Product p);
    int getCount(Product p);
    int changeQuantity(Product p, int count);

private:
    std::vector<Product> cart;
    std::map<int,int> product_count;

};

#endif // SHOPPINGCART_H
