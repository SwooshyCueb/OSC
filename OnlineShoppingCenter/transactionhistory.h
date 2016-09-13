#ifndef TRANSACTIONHISTORY_H
#define TRANSACTIONHISTORY_H

#include <vector>
#include "common.h"
#include "transaction.h"


using namespace std;

class TransactionHistory {
    public:
        TransactionHistory();
        TransactionHistory(Transaction trans);
        char addTransaction(Transaction trans);
        int transaction_list_id; //Dont think this is needed since ID is stored in the Transaction object

    private:

        vector<Transaction> transaction_list;
};

#endif // TRANSACTIONHISTORY_H
