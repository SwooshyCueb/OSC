#ifndef TRANSACTIONHISTORY_H
#define TRANSACTIONHISTORY_H

#include <deque>
#include "transaction.h"

using namespace std;

class TransactionHistory {
public:
    TransactionHistory();
    deque<Transaction> transaction_list;
    int transaction_list_id = -1;
};

#endif // TRANSACTIONHISTORY_H
