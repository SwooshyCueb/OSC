#include "paymentinfo.h"

using namespace std;

PaymentInfo::PaymentInfo() {

}

PaymentInfo::PaymentInfo(unsigned long new_num, string new_exp,
                         unsigned int new_cv2) {
    cc_num = new_num;
    cc_exp = new_exp;
    cc_cv2 = new_cv2;
    is_set = true;
}

