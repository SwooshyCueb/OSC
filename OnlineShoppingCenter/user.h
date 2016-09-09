#ifndef USER_H
#define USER_H


#include "shippingaddress.h"
//#include "transactionhistory.h"


class User
{
public:
    User();
    User(std::string, ShippingAddress, int, std::string, int, int, TransactionHistory);

    int changeCreditCard(int, string, int);
    int changeShippingAddress(ShippingAddress shipping_address);
    //int getTransactionHistory();


private:
    std::string user_name;
    ShippingAddress shipping_address;
    int credit_card_num;
    std::string credit_card_exp;
    int credit_card_cv2;
    int transaction_history_id;
    TransactionHistory transaction_history;

};

#endif // USER_H
