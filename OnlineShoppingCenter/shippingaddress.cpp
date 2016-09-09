#include "shippingaddress.h"

#include <string>
#include <iostream>

ShippingAddress::ShippingAddress()
{

}
ShippingAddress::ShippingAddress(std::string new_shipping_name, std::string new_street, std::string new_city, std::string new_state, int new_zip, std::string new_country){
    shipping_name = new_shipping_name;
    street = new_street;
    city = new_city;
    state = new_state;
    zip = new_zip;
    country = new_country;
}

int ShippingAddress::changeShippingName(std::string new_shipping_name){
    shipping_name = new_shipping_name;
    return 1;
}

int ShippingAddress::changeStreet(std::string new_street) {
    street = new_street;
    return 1;
}

int ShippingAddress::changeCity(std::string new_city) {
    city = new_city;
    return 1;
}
int ShippingAddress::changeState(std::string new_state) {
    state = new_state;
    return 1;
}
int ShippingAddress::changeZip(int new_zip) {
    zip = new_zip;
    return 1;
}
int ShippingAddress::changeCountry(std::string new_country) {
    country = new_country;
    return 1;
}


void ShippingAddress::print() {
    std::cout << shipping_name << " " << street << " " << city << " " << state << " " << zip << " " << country << std::endl;
}


