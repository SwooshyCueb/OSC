#include "shippingaddress.h"

#include <iostream>
#include <cstdio>

using namespace std;

ShippingAddress::ShippingAddress() {

}

ShippingAddress::ShippingAddress(string new_shipping_name, string new_street,
                                 string new_city, string new_state,
                                 unsigned int new_zip,
                                 string new_country) {
    shipping_name = new_shipping_name;
    street = new_street;
    city = new_city;
    state = new_state;
    zip = new_zip;
    country = new_country;

    is_set = true;
    parts_set = S_ALL_SET;
}

int ShippingAddress::changeShippingName(string new_shipping_name) {
    shipping_name = new_shipping_name;

    parts_set |= S_NAME_SET;
    if (parts_set == S_ALL_SET) {
        is_set = true;
    }

    return 1;
}

int ShippingAddress::changeStreet(string new_street) {
    street = new_street;

    parts_set |= S_STREET_SET;
    if (parts_set == S_ALL_SET) {
        is_set = true;
    }


    return 1;
}

int ShippingAddress::changeCity(string new_city) {
    city = new_city;

    parts_set |= S_CITY_SET;
    if (parts_set == S_ALL_SET) {
        is_set = true;
    }

    return 1;
}

int ShippingAddress::changeState(string new_state) {
    state = new_state;

    parts_set |= S_STATE_SET;
    if (parts_set == S_ALL_SET) {
        is_set = true;
    }

    return 1;
}

int ShippingAddress::changeZip(unsigned int new_zip) {
    zip = new_zip;

    parts_set |= S_ZIP_SET;
    if (parts_set == S_ALL_SET) {
        is_set = true;
    }

    return 1;
}

int ShippingAddress::changeCountry(std::string new_country) {
    country = new_country;

    parts_set |= S_COUNTRY_SET;
    if (parts_set == S_ALL_SET) {
        is_set = true;
    }

    return 1;
}

void ShippingAddress::print() {
    cout << shipping_name << endl << street << endl << city << ", "
              << state << " ";
    printf("%05u", zip);
    cout << endl << country << endl;
}


