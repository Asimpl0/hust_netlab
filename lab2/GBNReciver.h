#include "RdtReceiver.h"
#define N 4//窗口大小
#define M 8//序号

class GBNReciver :public RdtReceiver {
	int expectednum;//期待收的的序号
	Packet ackpacket;//发送的确认packet

public:
	void receive(const Packet& packet);	//接收报文，将被NetworkService调用
	GBNReciver();
	~GBNReciver();

};