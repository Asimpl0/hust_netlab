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
	//pns->setRunMode(0);  //VERBOS模式
	pns->setRunMode(1);  //安静模式
	

	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("C:\\Users\\13110\\Desktop\\input.txt");
	pns->setOutputFile("C:\\Users\\13110\\Desktop\\output.txt");

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
	delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete

	return 0;

}