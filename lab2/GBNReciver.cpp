#include "net.h"
#include "GBNReciver.h"

GBNReciver::GBNReciver() :expectednum(0) {
	


	ackpacket.acknum = -1;//��ʼ��Ϊ-1
	ackpacket.checksum = 0;
	ackpacket.seqnum = -1;//����
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		ackpacket.payload[i] = '.';
	}
	ackpacket.checksum = pUtils->calculateCheckSum(ackpacket);
}

GBNReciver::~GBNReciver() {

}

void GBNReciver::receive(const Packet& packet) {
	//���У����Ƿ���ȷ
	int checkSum = pUtils->calculateCheckSum(packet);
	//���У�����ȷ��ͬʱ�յ����ĵ���ŵ��ڽ��շ��ڴ��յ��ı������һ��
	if (checkSum == packet.checksum && expectednum == packet.seqnum) {//
		pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���", packet);
		//ȡ��Message�����ϵݽ���Ӧ�ò�
		Message msg;
		memcpy(msg.data, packet.payload, sizeof(packet.payload));
		pns->delivertoAppLayer(RECEIVER, msg);

		ackpacket.acknum = expectednum;  //ȷ����ŵ����յ��ı������

		ackpacket.checksum = pUtils->calculateCheckSum(ackpacket);
		pUtils->printPacket("���շ�����ȷ�ϱ���", ackpacket);
		pns->sendToNetworkLayer(SENDER, ackpacket);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�
		expectednum = (expectednum + 1) % M;
	}
	else {
		if (checkSum != packet.checksum) {
			pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
		}
		else {
			pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,������Ų���", packet);
		}
		pUtils->printPacket("���շ����·����ϴε�ȷ�ϱ���", ackpacket);
		pns->sendToNetworkLayer(SENDER, ackpacket);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢���ϴε�ȷ�ϱ���
	}
}