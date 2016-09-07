#ifndef PRODUCT_H
#define PRODUCT_H


class Product
{
    int upc;
    std::string name;
    float price = 0;
    std::string catagory;
    int quantity;

public:
    Product();

    int setUPC(int upc);
    int getUPC();

    int setName(string name);
    string getName();

    int setPrice(float price);
    float getPrice();

    int setCatagory(string catagory);
    string getCatagory();

    int setQuantity(int quantity);
    int getQuantity();

    string toString();

};

#endif // PRODUCT_H
