#include "product.h"

using namespace std;

Product::Product()
{
    //cout << "product created.";
}

Product::Product(SKU new_upc, string new_name, float new_price,
                 string new_category, unsigned int new_quantity){
    UPC = new_upc;
    name = new_name;
    price = new_price;
    category = new_category;
    quantity = new_quantity;

    is_set = true;
    parts_set = P_ALL_SET;
}

int Product::setUPC(SKU new_upc){
    UPC = new_upc;

    parts_set |= P_UPC_SET;
    if (parts_set == P_ALL_SET) {
        is_set = true;
    }

    return 1;
}
SKU Product::getUPC() {
    return UPC;
}

int Product::setName(string new_name) {
    name = new_name;

    parts_set |= P_NAME_SET;
    if (parts_set == P_ALL_SET) {
        is_set = true;
    }

    return 1;
}
string Product::getName() {
    return name;
}
int Product::setPrice(float new_price){
    price = new_price;

    parts_set |= P_PRICE_SET;
    if (parts_set == P_ALL_SET) {
        is_set = true;
    }

    return 1;
}
float Product::getPrice() {
    return price;
}

int Product::setCategory(string new_category) {
    category = new_category;

    parts_set |= P_CATEGORY_SET;
    if (parts_set == P_ALL_SET) {
        is_set = true;
    }

    return 1;
}
string Product::getCategory() {
    return category;
}

int Product::setQuantity(unsigned int new_quantity) {
    if (new_quantity < 0)
        return -1;

    quantity = new_quantity;
    //update storagesystem

    parts_set |= P_QUANTITY_SET;
    if (parts_set == P_ALL_SET) {
        is_set = true;
    }

    return 1;
}
unsigned int Product::getQuantity(){
    return quantity;
}
void Product::print() {
    cout << UPC << " " << name << " " << category << " " << price << " " << quantity << endl;
}

std::ostream& operator<<(ostream &strm, const Product& a) {
    cout << a.UPC;
    strm << "A(" << a.UPC << ", " << a.name << ", $" << a.price << ", " << a.category << ", " << a.quantity << ")";
    return strm;
}
