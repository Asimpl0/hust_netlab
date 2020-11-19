#include "net.h"
#include "TCPReceiver.h"

TCPReciver::TCPReciver() :expectednum(0) {
	ackpacket.acknum = -1;//初始化为-1
	ackpacket.checksum = 0;
	ackpacket.seqnum = -1;//忽略
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		ackpacket.payload[i] = '.';
	}
	ackpacket.checksum = pUtils->calculateCheckSum(ackpacket);
}

TCPReciver::~TCPReciver() {

}

void TCPReciver::receive(const Packet& packet) {
	//检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(packet);
	//如果校验和正确，同时收到报文的序号等于接收方期待收到的报文序号一致
	if (checkSum == packet.checksum && expectednum == packet.seqnum) {//
		pUtils->printPacket("接收方正确收到发送方的报文", packet);
		//取出Message，向上递交给应用层
		Message msg;
		memcpy(msg.data, packet.payload, sizeof(packet.payload));
		pns->delivertoAppLayer(RECEIVER, msg);

		ackpacket.acknum = expectednum;  //确认序号等于收到的报文序号

		ackpacket.checksum = pUtils->calculateCheckSum(ackpacket);
		pUtils->printPacket("接收方发送确认报文", ackpacket);
		pns->sendToNetworkLayer(SENDER, ackpacket);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
		expectednum = (expectednum + 1) % M;
	}
	else {
		if (checkSum != packet.checksum) {
			pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
		}
		else {
			pUtils->printPacket("接收方没有正确收到发送方的报文,报文序号不对", packet);
		}
		pUtils->printPacket("接收方重新发送上次的确认报文", ackpacket);
		pns->sendToNetworkLayer(SENDER, ackpacket);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送上次的确认报文
	}
}