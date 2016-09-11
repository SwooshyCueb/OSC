#ifndef PAYMENTINFO_H
#define PAYMENTINFO_H

#include <string>

using namespace std;

/* This should be all we need unless we need equality/assignment operators or
 * something along those lines.
 */
class PaymentInfo
{
public:
    PaymentInfo();
    PaymentInfo(unsigned long new_num, string new_exp, unsigned int new_cv2);

    unsigned long cc_num;
    string cc_exp;
    unsigned int cc_cv2;
    bool is_set = false;
};

#endif // PAYMENTINFO_H
