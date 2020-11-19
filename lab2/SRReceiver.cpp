#include "net.h"
#include "SRReceiver.h"

SRReciver::SRReciver() :rcv_base(0) {
	memset(rcv, false, 8);

	ackpacket.acknum = -1;//初始化为-1
	ackpacket.checksum = 0;
	ackpacket.seqnum = -1;//忽略
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		ackpacket.payload[i] = '.';
	}
	ackpacket.checksum = pUtils->calculateCheckSum(ackpacket);
}

SRReciver::~SRReciver() {

}


void SRReciver::receive(const Packet& packet) {
	//检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(packet);
	//如果校验和正确，同时收到报文的序号等于接收方期待收到的报文序号一致
	if (checkSum == packet.checksum) {//
		pUtils->printPacket("接收方正确收到发送方的报文", packet);

		if(((rcv_base+N-1)%M>rcv_base&&(packet.seqnum>=rcv_base&& packet.seqnum<= (rcv_base + N- 1) % M))
			|| ((rcv_base + N - 1) % M < rcv_base &&((packet.seqnum>=rcv_base&& packet.seqnum<=M-1)|| packet.seqnum<= (rcv_base + N - 1) % M )))
		 {
			ackpacket.acknum = packet.seqnum;  //确认序号等于收到的报文序号
			ackpacket.checksum = pUtils->calculateCheckSum(ackpacket);
			pUtils->printPacket("接收方发送确认报文", ackpacket);
			pns->sendToNetworkLayer(SENDER, ackpacket);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
			if (rcv[packet.seqnum] == false) {
				rcvpkt[packet.seqnum] = packet;//存入
				rcv[packet.seqnum] = true;//已收到
			}
			if (packet.seqnum == rcv_base) {
				int i = rcv_base;
				for (; i != (rcv_base + N)%M;) {
					if (rcv[i] == true)//已收到
					{
						//取出Message，向上递交给应用层
						Message msg;
						memcpy(msg.data, rcvpkt[i].payload, sizeof(rcvpkt[i].payload));
						pns->delivertoAppLayer(RECEIVER, msg);
						rcv[i] = false;
						i = (i + 1) % M;
					}
					else {
						break;
					}
				}
				cout << "接收窗口移动前：" << endl;
				for (int j = rcv_base; j != (rcv_base+N)%M; ) {
					//输出窗口

						Message msg;
						memcpy(msg.data, rcvpkt[j].payload, sizeof(rcvpkt[j].payload));
						msg.data[21] = '\0';
						cout << msg.data << endl;
						
					j = (j + 1) % M;
				}
				cout << endl;
				rcv_base = i;
				cout << "接收窗口移动后：" << endl;
				for (int j = rcv_base; j != (rcv_base + N) % M; ) {
					//输出窗口
						Message msg;
						memcpy(msg.data, rcvpkt[j].payload, sizeof(rcvpkt[j].payload));
						msg.data[21] = '\0';
						cout << msg.data << endl;
						
				
					j = (j + 1) % M;
				}
				cout << endl;
			}

		}
		else if (((rcv_base - N+8) % M <(rcv_base-1+8)%M && (packet.seqnum >= (rcv_base - N+8) % M && packet.seqnum <= (rcv_base - 1+8) % M))
			|| ((rcv_base - N+8) % M > (rcv_base - 1+8) % M && ((packet.seqnum >= (rcv_base - N+8) % M && packet.seqnum <= M - 1) || packet.seqnum <= (rcv_base - 1+8) % M)))
		{
			ackpacket.acknum = packet.seqnum;  //确认序号等于收到的报文序号
			ackpacket.checksum = pUtils->calculateCheckSum(ackpacket);
			pUtils->printPacket("接收方发送确认报文", ackpacket);
			pns->sendToNetworkLayer(SENDER, ackpacket);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
		}


	}
}