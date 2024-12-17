#pragma once

#define SPEED_MAX_WITH_CTR 40
#define SPEED_MAX_WITHOUT_CTR 60


class Order {
public:
	Order(short left=0, short rigth=0,bool ctr=true);
	short get_order_R() const;
	short get_order_L() const;
	bool get_speed_ctr() const;
	void set_speed_ctr(bool ctr);
	void set_order(short left,short right);
private:
	short m_order_R;
	short m_order_L;
	bool m_speed_ctr;
	void limit();
};
