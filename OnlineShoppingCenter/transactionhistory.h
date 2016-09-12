#ifndef TRANSACTIONHISTORY_H
#define TRANSACTIONHISTORY_H

#include "common.h"
#include "transaction.h"
#include <deque>

using namespace std;

class TransactionHistory {
public:
    TransactionHistory();
    deque<Transaction> transaction_list;
    int transaction_list_id = -1;
};

#endif // TRANSACTIONHISTORY_H
