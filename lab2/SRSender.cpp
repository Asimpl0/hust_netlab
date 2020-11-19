#include "net.h"
#include "SRSender.h"

SRSender::SRSender() :send_base(0), nextseqnum(0) {
	memset(snd, false, 8);
}

SRSender::~SRSender() {

}

bool SRSender::getWaitingState() {
	if (nextseqnum == (send_base + 4) % 8) {
		return true;
	}
	else return false;
}

bool SRSender::send(const Message& message) {
	if (getWaitingState() == true)
		return false;
	snd[nextseqnum] = false;//未确认
	sndpkt[nextseqnum].acknum = -1;//忽略该字段

	sndpkt[nextseqnum].seqnum = nextseqnum;//当前包的序号即为nextseqnum
	sndpkt[nextseqnum].checksum = 0;//校验和为0
	memcpy(sndpkt[nextseqnum].payload, message.data, sizeof(message.data));
	//message打包
	sndpkt[nextseqnum].checksum = pUtils->calculateCheckSum(sndpkt[nextseqnum]);
	//计算当前包的校验和
	pUtils->printPacket("发送方发送报文", sndpkt[nextseqnum]);
	pns->stopTimer(SENDER, nextseqnum);
	pns->startTimer(SENDER, Configuration::TIME_OUT, nextseqnum);//为第一个元素设置时钟
	pns->sendToNetworkLayer(RECEIVER, sndpkt[nextseqnum]);//发送数据
	nextseqnum = (nextseqnum + 1) % M;//后移

	return true;
}

void SRSender::receive(const Packet& ackPkt) {
	if (send_base != nextseqnum) {//base没确认完
		//检查校验和是否正确
		int checkSum = pUtils->calculateCheckSum(ackPkt);
		if (checkSum == ackPkt.checksum) {
			if (((send_base + N - 1) % M > send_base && (ackPkt.acknum >= send_base && ackPkt.acknum <= (send_base + N - 1) % M))
				|| ((send_base + N - 1) % M < send_base && ((ackPkt.acknum >= send_base && ackPkt.acknum <= M - 1) || ackPkt.acknum <= (send_base + N - 1) % M)))
			{
				snd[ackPkt.acknum] = true;//标记为已确认
				pns->stopTimer(SENDER, ackPkt.acknum);
				pUtils->printPacket("发送方正确收到确认", ackPkt);
				if (send_base == ackPkt.acknum) {
					int i = send_base;
					for (; i != nextseqnum;)
					{
						if (snd[i] == false) break;
						i = (i + 1) % M;
					}
					cout << "窗口移动前：" << endl;
					for (int i = send_base; i != nextseqnum; ) {
						//输出窗口
						Message msg;

						memcpy(msg.data, sndpkt[i].payload, sizeof(sndpkt[i].payload));
						msg.data[21] = '\0';
						cout << msg.data << endl;
						i = (i + 1) % M;
					}
					cout << endl;
					send_base = i;
					cout << "窗口移动后：" << endl;
					for (int i = send_base; i != nextseqnum; ) {
						//输出窗口
						Message msg;
						memcpy(msg.data, sndpkt[i].payload, sizeof(sndpkt[i].payload));
						msg.data[21] = '\0';
						cout << msg.data << endl;
						i = (i + 1) % M;
					}
					cout << endl;
				}
			}
		}
	}

}

void SRSender::timeoutHandler(int seqNum) {
	pns->stopTimer(SENDER, seqNum);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);//为元素设置时钟
		//开始重发
	pns->sendToNetworkLayer(RECEIVER, sndpkt[seqNum]);//发送数据
	pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", sndpkt[seqNum]);
}