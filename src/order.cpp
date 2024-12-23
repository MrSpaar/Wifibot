#include "includes/order.h"


Order::Order(short left, short right,bool ctr):
    orderR(right),
    orderL(left),
    speedCtr(ctr)
{
    limit();
}

short Order::getOrderL() const {
    return orderL;
}

short Order::getOrderR() const {
    return orderR;
}

bool Order::getSpeedCtr() const {
    return speedCtr;
}

void Order::setOrder(short left,short right) {
    orderL = left;
    orderR = right;

    limit();
}

void Order::setSpeedCtr(bool ctr) {
    speedCtr=ctr;
    limit();
}

void Order::limit() {
    if (speedCtr) {
        if(orderL > SPEED_MAX_WITH_CTR) orderL = SPEED_MAX_WITH_CTR;
        else if(orderL < -SPEED_MAX_WITH_CTR) orderL = -SPEED_MAX_WITH_CTR;
        if(orderR > SPEED_MAX_WITH_CTR) orderR = SPEED_MAX_WITH_CTR;
        else if(orderR < -SPEED_MAX_WITH_CTR) orderR = -SPEED_MAX_WITH_CTR;
    } else {
        if(orderL > SPEED_MAX_WITHOUT_CTR) orderL = SPEED_MAX_WITHOUT_CTR;
        else if(orderL < -SPEED_MAX_WITHOUT_CTR) orderL = -SPEED_MAX_WITHOUT_CTR;
        if(orderR > SPEED_MAX_WITHOUT_CTR) orderR = SPEED_MAX_WITHOUT_CTR;
        else if(orderR < -SPEED_MAX_WITHOUT_CTR) orderR = -SPEED_MAX_WITHOUT_CTR;
    }
}
