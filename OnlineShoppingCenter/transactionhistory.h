#ifndef TRANSACTIONHISTORY_H
#define TRANSACTIONHISTORY_H

#include <vector>
#include "transaction.h"

class TransactionHistory : public Transaction
{
public:
    TransactionHistory();


private:
    int transaction_list_id;
    std::vector<Transaction> transaction_list;

};

#endif // TRANSACTIONHISTORY_H
