#include "net.h"
#include "GBNReciver.h"
#include "GBNSender.h"
#include "SRReceiver.h"
#include "SRSender.h"
#include "TCPReceiver.h"
#include "TCPSender.h"

int main(void) {
	
	//RdtSender* ps = new GBNSender();
	//RdtReceiver* pr = new GBNReciver();
	//RdtSender* ps = new SRSender();
	//RdtReceiver* pr = new SRReciver();
	RdtSender* ps = new TCPSender();
	RdtReceiver* pr = new TCPReciver();
	//pns->setRunMode(0);  //VERBOSģʽ
	pns->setRunMode(1);  //����ģʽ
	

	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("C:\\Users\\13110\\Desktop\\input.txt");
	pns->setOutputFile("C:\\Users\\13110\\Desktop\\output.txt");

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
	delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete

	return 0;

}