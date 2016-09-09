#include "user.h"
#include "shippingaddress.h"
//#include "transactionhistory.h"
#include <iostream>


User::User()
{

}
User::User(std::string new_user_name, ShippingAddress new_shipping_address, int new_cc_num, std::string new_cc_exp, int new_cc_cv2, int, new_transaction_history_id, TransactionHistory new_transaction_history){

    user_name = new_user_name;
    shipping_address = new_shipping_address;
    credit_card_num = new_cc_num;
    credit_card_exp = new_cc_exp;
    credit_card_cv2 = new_cc_cv2;
    transaction_history_id = new_transaction_history_id;
    transaction_history = new_transaction_history;
}
int User::changeCreditCard(int new_num, std::string new_exp, int new_cv2) {

    credit_card_num = new_num;
    credit_card_exp = new_exp;
    credit_card_cv2 = new_cv2;

    return 1;
}

int User::changeShippingAddress(ShippingAddress new_shipping_address){
    shipping_address = new_shipping_address;

    return 1;
}

/*
int User::getTransactionHistory(){

}
*/
