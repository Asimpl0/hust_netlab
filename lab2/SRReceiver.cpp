#include "net.h"
#include "SRReceiver.h"

SRReciver::SRReciver() :rcv_base(0) {
	memset(rcv, false, 8);

	ackpacket.acknum = -1;//��ʼ��Ϊ-1
	ackpacket.checksum = 0;
	ackpacket.seqnum = -1;//����
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		ackpacket.payload[i] = '.';
	}
	ackpacket.checksum = pUtils->calculateCheckSum(ackpacket);
}

SRReciver::~SRReciver() {

}


void SRReciver::receive(const Packet& packet) {
	//���У����Ƿ���ȷ
	int checkSum = pUtils->calculateCheckSum(packet);
	//���У�����ȷ��ͬʱ�յ����ĵ���ŵ��ڽ��շ��ڴ��յ��ı������һ��
	if (checkSum == packet.checksum) {//
		pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���", packet);

		if(((rcv_base+N-1)%M>rcv_base&&(packet.seqnum>=rcv_base&& packet.seqnum<= (rcv_base + N- 1) % M))
			|| ((rcv_base + N - 1) % M < rcv_base &&((packet.seqnum>=rcv_base&& packet.seqnum<=M-1)|| packet.seqnum<= (rcv_base + N - 1) % M )))
		 {
			ackpacket.acknum = packet.seqnum;  //ȷ����ŵ����յ��ı������
			ackpacket.checksum = pUtils->calculateCheckSum(ackpacket);
			pUtils->printPacket("���շ�����ȷ�ϱ���", ackpacket);
			pns->sendToNetworkLayer(SENDER, ackpacket);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�
			if (rcv[packet.seqnum] == false) {
				rcvpkt[packet.seqnum] = packet;//����
				rcv[packet.seqnum] = true;//���յ�
			}
			if (packet.seqnum == rcv_base) {
				int i = rcv_base;
				for (; i != (rcv_base + N)%M;) {
					if (rcv[i] == true)//���յ�
					{
						//ȡ��Message�����ϵݽ���Ӧ�ò�
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
				cout << "���մ����ƶ�ǰ��" << endl;
				for (int j = rcv_base; j != (rcv_base+N)%M; ) {
					//�������

						Message msg;
						memcpy(msg.data, rcvpkt[j].payload, sizeof(rcvpkt[j].payload));
						msg.data[21] = '\0';
						cout << msg.data << endl;
						
					j = (j + 1) % M;
				}
				cout << endl;
				rcv_base = i;
				cout << "���մ����ƶ���" << endl;
				for (int j = rcv_base; j != (rcv_base + N) % M; ) {
					//�������
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
			ackpacket.acknum = packet.seqnum;  //ȷ����ŵ����յ��ı������
			ackpacket.checksum = pUtils->calculateCheckSum(ackpacket);
			pUtils->printPacket("���շ�����ȷ�ϱ���", ackpacket);
			pns->sendToNetworkLayer(SENDER, ackpacket);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�
		}


	}
}