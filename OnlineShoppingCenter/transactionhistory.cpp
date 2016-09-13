#include "transactionhistory.h"

using namespace std;

TransactionHistory::TransactionHistory()
{

}

TransactionHistory::TransactionHistory(Transaction trans) {
    this->transaction_list.push_back(trans);
}
char TransactionHistory::addTransaction(Transaction trans) {
    this->transaction_list.push_back(trans);
    return 1;
}
