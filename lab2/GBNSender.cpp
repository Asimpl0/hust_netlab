#include"net.h"
#include"GBNSender.h"


GBNSender::GBNSender() :base(0), nextseqnum(0) {
}

GBNSender::~GBNSender() {

}

bool  GBNSender::getWaitingState() {
	if (nextseqnum == (base + 4) % 8) {
		return true;
	}
	else return false;
}

bool GBNSender::send(const Message &message) {
	if (getWaitingState() == true)
		return false;
	sndpkt[nextseqnum].acknum = -1;//忽略该字段

	sndpkt[nextseqnum].seqnum = nextseqnum;//当前包的序号即为nextseqnum
	sndpkt[nextseqnum].checksum = 0;//校验和为0
	memcpy(sndpkt[nextseqnum].payload, message.data, sizeof(message.data));
	
	//message打包
	sndpkt[nextseqnum].checksum= pUtils->calculateCheckSum(sndpkt[nextseqnum]);
	//计算当前包的校验和
	pUtils->printPacket("发送方发送报文", sndpkt[nextseqnum]);
	if ((base % M) == (nextseqnum % M))
	{
		pns->stopTimer(SENDER, 1);
		pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//为第一个元素设置时钟
	}
	pns->sendToNetworkLayer(RECEIVER, sndpkt[nextseqnum]);//发送数据
	nextseqnum = (nextseqnum + 1) % M;//后移
	
	return true;

}

void GBNSender::receive(const Packet &ackPkt) {
	if (base != nextseqnum) {//base没确认完
		//检查校验和是否正确
		int checkSum = pUtils->calculateCheckSum(ackPkt);
		if (checkSum == ackPkt.checksum) {
			cout << "窗口移动前："<<endl;
			for (int i = base; i != nextseqnum; ) {
				//输出窗口
				Message msg;
				
				memcpy(msg.data, sndpkt[i].payload, sizeof(sndpkt[i].payload));
				msg.data[21] = '\0';
				cout << msg.data << endl;
				i = (i + 1) % M;
			}
			cout << endl;
			base = (ackPkt.acknum + 1) % M;
			cout << "窗口移动后：" << endl;
			for (int i = base; i != nextseqnum; ) {
				//输出窗口
				Message msg;
				memcpy(msg.data, sndpkt[i].payload, sizeof(sndpkt[i].payload));
				msg.data[21] = '\0';
				cout<< msg.data <<endl;
				i = (i + 1) % M;
			}
			cout << endl;
			pUtils->printPacket("发送方正确收到确认", ackPkt);
			
			if (base == nextseqnum) {//所有都确认完
				pns->stopTimer(SENDER, 1);
			}
			else {
				pns->stopTimer(SENDER, 1);
				pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//为第一个元素设置时钟
			}
		}
	}
	
}

void GBNSender::timeoutHandler(int seqNum) {
	//唯一一个定时器,无需考虑seqNum
	pns->stopTimer(SENDER, 1);
	pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//为第一个元素设置时钟
	for (int i = base; i != nextseqnum; ) {
	    //开始重发
		pns->sendToNetworkLayer(RECEIVER, sndpkt[i]);//发送数据
		pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", sndpkt[i]);
		i = (i + 1) % M;
	}
	
}