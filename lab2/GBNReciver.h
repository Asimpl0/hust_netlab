#include "RdtReceiver.h"
#define N 4//���ڴ�С
#define M 8//���

class GBNReciver :public RdtReceiver {
	int expectednum;//�ڴ��յĵ����
	Packet ackpacket;//���͵�ȷ��packet

public:
	void receive(const Packet& packet);	//���ձ��ģ�����NetworkService����
	GBNReciver();
	~GBNReciver();

};