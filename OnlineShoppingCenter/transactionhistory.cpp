#include "transactionhistory.h"

using namespace std;

TransactionHistory::TransactionHistory()
{

}

TransactionHistory::TransactionHistory(Transaction trans) {
    transaction_list.push_back(trans);
}
int TransactionHistory::addTransaction(Transaction trans) {
    transaction_list.push_back(trans);
    return 1;
}
