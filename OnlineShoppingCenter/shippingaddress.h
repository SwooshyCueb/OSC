#ifndef SHIPPINGADDRESS_H
#define SHIPPINGADDRESS_H


#import <string>

class ShippingAddress
{

public:
    ShippingAddress();
    ShippingAddress(std::string, std::string, std::string, std::string, int, std::string);

    int changeShippingName(std::string new_shipping_name);

    int changeStreet(std::string new_street);

    int changeCity(std::string new_city);

    int changeState(std::string new_state);

    int changeZip(int new_zip);

    int changeCountry(std::string new_country);


    void print();

private:
    std::string shipping_name;
    std::string street;
    std::string city;
    std::string state;
    int zip;
    std::string country;
};


#endif // SHIPPINGADDRESS_H
