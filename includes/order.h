#pragma once

#define SPEED_MAX_WITH_CTR    40
#define SPEED_MAX_WITHOUT_CTR 60


class Order {
public:
	Order(short left = 0, short rigth = 0, bool ctr = true);
	short getOrderR() const;
	short getOrderL() const;
	bool getSpeedCtr() const;
	void setSpeedCtr(bool ctr);
	void setOrder(short left, short right);
private:
	short orderR;
	short orderL;
	bool speedCtr;

	void limit();
};
