#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>

class Product
{

public:
    Product();
    Product(int, std::string, float, std::string, int);

    int setUPC(int new_upc);
    int getUPC();

    int setName(std::string new_name);
    std::string getName();

    int setPrice(float new_price);
    float getPrice();

    int setCatagory(std::string new_catagory);
    std::string getCatagory();

    int setQuantity(int new_quantity);
    int getQuantity();

    void print();

    friend std::ostream& operator<<(std::ostream&, const Product&);

private:
    int UPC;
    int quantity;
    float price = 0;
    std::string catagory;
    std::string name;



};

#endif // PRODUCT_H
