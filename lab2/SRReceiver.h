#include "RdtReceiver.h"
#define N 4//���ڴ�С
#define M 8//���

class SRReciver :public RdtReceiver {
	bool rcv[M];//���ܴ����Ƿ��յ�
	Packet rcvpkt[M];//�յ���packet
	int rcv_base;
	Packet ackpacket;//ȷ��packet

public:
	void receive(const Packet& packet);	//���ձ��ģ�����NetworkService����
	SRReciver();
	~SRReciver();

};