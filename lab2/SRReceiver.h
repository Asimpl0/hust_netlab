#include "RdtReceiver.h"
#define N 4//窗口大小
#define M 8//序号

class SRReciver :public RdtReceiver {
	bool rcv[M];//接受窗口是否收到
	Packet rcvpkt[M];//收到的packet
	int rcv_base;
	Packet ackpacket;//确认packet

public:
	void receive(const Packet& packet);	//接收报文，将被NetworkService调用
	SRReciver();
	~SRReciver();

};