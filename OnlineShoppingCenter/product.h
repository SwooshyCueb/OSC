#ifndef PRODUCT_H
#define PRODUCT_H

#include "common.h"
#include <iostream>

using namespace std;

class Product
{

public:
    Product();
    Product(SKU new_upc, string new_name, float new_price, string new_category,
            unsigned int new_quantity);

    int setUPC(SKU new_upc);
    SKU getUPC();

    int setName(string new_name);
    string getName();

    int setPrice(float new_price);
    float getPrice();

    int setCategory(string new_category);
    string getCategory();

    int setQuantity(unsigned int new_quantity);
    unsigned int getQuantity();

    void print();

    friend std::ostream& operator<<(std::ostream&, const Product&);

private:
    SKU UPC;
    unsigned int quantity;
    float price = 0;
    string category;
    string name;

};

#endif // PRODUCT_H
