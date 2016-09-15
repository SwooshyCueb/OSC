#ifndef TRANSACTIONHISTORY_H
#define TRANSACTIONHISTORY_H

#include <vector>
#include "common.h"
#include "transaction.h"

// tbh we could probably replace this entire class with a map or deque. --jam927

class TransactionHistory {
    public:
        TransactionHistory();
        TransactionHistory(Transaction trans);
        int addTransaction(Transaction trans);

        int transaction_list_id; //Dont think this is needed since ID is stored in the Transaction object

        std::vector<Transaction> transaction_list;
};

#endif // TRANSACTIONHISTORY_H
