#pragma once

#define SPEED_MAX_WITH_CTR    40
#define SPEED_MAX_WITHOUT_CTR 60


class Order {
public:
    Order(short left = 0, short right = 0, bool ctr = true):
        orderR(right), orderL(left), speedCtr(ctr)
    {
        limit();
    }

    short getOrderR() const {
        return orderR;
    }

    short getOrderL() const {
        return orderL;
    }

    bool getSpeedCtr() const {
        return speedCtr;
    }

    void setSpeedCtr(bool ctr) {
        speedCtr = ctr;
        limit();
    }

    void setOrder(short left, short right) {
        orderL = left;
        orderR = right;
        limit();
    }
private:
    void limit() {
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
private:
    short orderR;
    short orderL;
    bool speedCtr;
};
