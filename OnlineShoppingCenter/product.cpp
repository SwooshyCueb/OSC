#include "product.h"

using namespace std;

Product::Product()
{
    cout << "product created.";
}
Product::Product(SKU new_upc, string new_name, float new_price,
                 string new_catagory, unsigned int new_quantity){
    UPC = new_upc;
    name = new_name;
    price = new_price;
    catagory = new_catagory;
    quantity = new_quantity;
}

int Product::setUPC(SKU new_upc){
    UPC = new_upc;
    return 1;
}
int Product::getUPC() {
    return UPC;
}

int Product::setName(string new_name) {
    name = new_name;
    return 1;
}
string Product::getName() {
    return name;
}
int Product::setPrice(float new_price){
    price = new_price;
    return 1;
}
float Product::getPrice() {
    return price;
}
int Product::setCatagory(string new_catagory) {
    catagory = new_catagory;
    return 1;
}
int Product::setQuantity(unsigned int new_quantity) {
    quantity = new_quantity;
    return 1;
}
int Product::getQuantity(){
    return quantity;
}
void Product::print() {
    std::cout << UPC << " " << name << " " << catagory << " " << price << " " << quantity << endl;
}

std::ostream& operator<<(ostream &strm, const Product& a) {
    cout << a.UPC;
    strm << "A(" << a.UPC << ", " << a.name << ", $" << a.price << ", " << a.catagory << ", " << a.quantity << ")";
    return strm;
}
