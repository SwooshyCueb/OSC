#include "product.h"

#include <string>
#include <iostream>

Product::Product()
{
    std::cout << "product created.";
}
Product::Product(int new_upc, std::string new_name, float new_price, std::string new_catagory, int new_quantity){
    UPC = new_upc;
    name = new_name;
    price = new_price;
    catagory = new_catagory;
    quantity = new_quantity;
}

int Product::setUPC(int new_upc){
    UPC = new_upc;
    return 1;
}
int Product::getUPC() {
    return UPC;
}

int Product::setName(std::string new_name) {
    name = new_name;
    return 1;
}
std::string Product::getName() {
    return name;
}
int Product::setPrice(float new_price){
    price = new_price;
    return 1;
}
float Product::getPrice() {
    return price;
}
int Product::setCatagory(std::string new_catagory) {
    catagory = new_catagory;
    return 1;
}
int Product::setQuantity(int new_quantity) {
    quantity = new_quantity;
    return 1;
}
int Product::getQuantity(){
    return quantity;
}
void Product::print() {
    std::cout << UPC << " " << name << " " << catagory << " " << price << " " << quantity << std::endl;
}

std::ostream& operator<<(std::ostream &strm, const Product& a) {
    std::cout << a.UPC;
    strm << "A(" << a.UPC << ", " << a.name << ", $" << a.price << ", " << a.catagory << ", " << a.quantity << ")";
    return strm;
}
