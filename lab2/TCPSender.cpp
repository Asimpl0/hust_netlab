#include"net.h"
#include"TCPSender.h"

TCPSender::TCPSender() :base(0), nextseqnum(0), acked(0) {

}

TCPSender::~TCPSender() {

}

bool  TCPSender::getWaitingState() {
	if (nextseqnum == (base + 4) % 8) {
		return true;
	}
	else return false;
}


bool TCPSender::send(const Message& message) {
	if (getWaitingState() == true)
		return false;
	sndpkt[nextseqnum].acknum = -1;//���Ը��ֶ�

	sndpkt[nextseqnum].seqnum = nextseqnum;//��ǰ������ż�Ϊnextseqnum
	sndpkt[nextseqnum].checksum = 0;//У���Ϊ0
	memcpy(sndpkt[nextseqnum].payload, message.data, sizeof(message.data));
	//message���
	sndpkt[nextseqnum].checksum = pUtils->calculateCheckSum(sndpkt[nextseqnum]);
	//���㵱ǰ����У���
	pUtils->printPacket("���ͷ����ͱ���", sndpkt[nextseqnum]);
	if ((base % M) == (nextseqnum % M))
	{
		pns->stopTimer(SENDER, 1);
		pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//Ϊ��һ��Ԫ������ʱ��
	}
	pns->sendToNetworkLayer(RECEIVER, sndpkt[nextseqnum]);//��������
	nextseqnum = (nextseqnum + 1) % M;//����

	return true;

}



void TCPSender::receive(const Packet& ackPkt) {
	if (base != nextseqnum) {//baseûȷ����
		//���У����Ƿ���ȷ
		int checkSum = pUtils->calculateCheckSum(ackPkt);
		if (checkSum == ackPkt.checksum) {
			if (((base + N - 1) % M > base && (ackPkt.acknum >= base && ackPkt.acknum <= (base + N - 1) % M))
				|| ((base + N - 1) % M < base && ((ackPkt.acknum >= base && ackPkt.acknum <= M - 1) || ackPkt.acknum <= (base + N - 1) % M)))

			{
				cout << "�����ƶ�ǰ��" << endl;
				for (int i = base; i != nextseqnum; ) {
					//�������
					Message msg;

					memcpy(msg.data, sndpkt[i].payload, sizeof(sndpkt[i].payload));
					msg.data[21] = '\0';
					cout << msg.data << endl;
					i = (i + 1) % M;
				}
				cout << endl;
				base = (ackPkt.acknum + 1) % M;
				cout << "�����ƶ���" << endl;
				for (int i = base; i != nextseqnum; ) {
					//�������
					Message msg;
					memcpy(msg.data, sndpkt[i].payload, sizeof(sndpkt[i].payload));
					msg.data[21] = '\0';
					cout << msg.data << endl;
					i = (i + 1) % M;
				}
				cout << endl;
				pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
				if (base == nextseqnum) {//���ж�ȷ����
					pns->stopTimer(SENDER, 1);
				}
				else {
					pns->stopTimer(SENDER, 1);
					pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//Ϊ��һ��Ԫ������ʱ��
				}
			}
			else {
				//����Ȼ������ackֻ����base֮ǰһ�����
				acked++;//�յ�����
				if (acked == 3) {//�����ش�base
					pUtils->printPacket("���ͷ��յ�3������ack�����п����ش�", sndpkt[base]);
					pns->sendToNetworkLayer(RECEIVER, sndpkt[base]);//��������
					pns->stopTimer(SENDER, 1);
					pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//Ϊ��һ��Ԫ������ʱ��
					acked = 0;//��0
				}
			}
		}
	}

}


void TCPSender::timeoutHandler(int seqNum) {
	//Ψһһ����ʱ��,���迼��seqNum
	pns->stopTimer(SENDER, 1);
	pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//Ϊ��һ��Ԫ������ʱ��
	//�ط�base
	//��ʼ�ط�
	pns->sendToNetworkLayer(RECEIVER, sndpkt[base]);//��������
	pUtils->printPacket("���ͷ���ʱ��ʱ�䵽���ط��ϴη��͵ı���", sndpkt[base]);
}