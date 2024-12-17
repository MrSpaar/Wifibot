#include "includes/order.h"


Order::Order(short left, short right,bool ctr):
	m_order_R(right),
	m_order_L(left),
	m_speed_ctr(ctr)
{
	this->limit();
}

short Order::get_order_L() const {
	return m_order_L;
}

short Order::get_order_R() const {
	return m_order_R;
}

bool Order::get_speed_ctr() const {
	return m_speed_ctr;
}

void Order::set_order(short left,short right) {
	m_order_L=left;
	m_order_R=right;
	this->limit();
}

void Order::set_speed_ctr(bool ctr) {
	m_speed_ctr=ctr;
	this->limit();
}

void Order::limit() {
	if (m_speed_ctr==true){
		if(m_order_L > SPEED_MAX_WITH_CTR) m_order_L = SPEED_MAX_WITH_CTR;
		else if(m_order_L < -SPEED_MAX_WITH_CTR) m_order_L = -SPEED_MAX_WITH_CTR;
		if(m_order_R > SPEED_MAX_WITH_CTR) m_order_R = SPEED_MAX_WITH_CTR;
		else if(m_order_R < -SPEED_MAX_WITH_CTR) m_order_R = -SPEED_MAX_WITH_CTR;
	} else {
		if(m_order_L > SPEED_MAX_WITHOUT_CTR) m_order_L = SPEED_MAX_WITHOUT_CTR;
		else if(m_order_L < -SPEED_MAX_WITHOUT_CTR) m_order_L = -SPEED_MAX_WITHOUT_CTR;
		if(m_order_R > SPEED_MAX_WITHOUT_CTR) m_order_R = SPEED_MAX_WITHOUT_CTR;
		else if(m_order_R < -SPEED_MAX_WITHOUT_CTR) m_order_R = -SPEED_MAX_WITHOUT_CTR;
	}

}
