#ifndef PRODUCT_H
#define PRODUCT_H

#include "common.h"
#include <iostream>

#define P_NAME_SET 1
#define P_UPC_SET 2
#define P_PRICE_SET 4
#define P_CATEGORY_SET 8
#define P_QUANTITY_SET 16

#define P_ALL_SET 31

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

    bool is_set = false;

private:
    SKU UPC = 0;
    unsigned int quantity = 0;
    float price = 0;
    string category = "Uncategorized";
    string name = "UNDEFINED";
    unsigned int parts_set = 0;

};

#endif // PRODUCT_H
