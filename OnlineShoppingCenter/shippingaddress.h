#ifndef SHIPPINGADDRESS_H
#define SHIPPINGADDRESS_H

#include "common.h"

#define S_NAME_SET 1
#define S_STREET_SET 2
#define S_CITY_SET 4
#define S_STATE_SET 8
#define S_ZIP_SET 16
#define S_COUNTRY_SET 32

#define S_ALL_SET 63

using namespace std;

class ShippingAddress
{

public:
    ShippingAddress();
    ShippingAddress(string new_shipping_name, string new_street,
                    string new_city, string new_state, unsigned int new_zip,
                    string new_country);

    int changeShippingName(string new_shipping_name);

    int changeStreet(string new_street);

    int changeCity(string new_city);

    int changeState(string new_state);

    int changeZip(unsigned int new_zip);

    int changeCountry(string new_country);

    void print();

    string shipping_name;
    string street;
    string city;
    string state;
    unsigned int zip;
    string country;
    bool is_set = false;

    unsigned int parts_set = 0;
};


#endif // SHIPPINGADDRESS_H
