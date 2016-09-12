#include "user.h"

using namespace std;

/* User is created with just a username. Shipping address and payment info are
 * added later.
 */

/* If this is an existing user, we will already have a TransactionHistory
 * object for them.
 *
 * Since registration is beyond the scope of this assignment, this will be our
 * only constructor.
 */
User::User(string new_user_name, TransactionHistory new_transaction_history)
{

    user_name = new_user_name;
    transaction_history_id = new_transaction_history.transaction_list_id;
    transaction_history = new_transaction_history;
}

int User::changeCreditCard(PaymentInfo new_cc) {

    payment_info = new_cc;

    return 1;
}

int User::changeShippingAddress(ShippingAddress new_shipping_address){
    shipping_address = new_shipping_address;
    return 1;
}

