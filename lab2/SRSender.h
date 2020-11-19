#include"RdtSender.h"
#define N 4//���ڴ�С
#define M 8//���

class SRSender :public RdtSender {
private:
	//��Ų���3λ���룬��0-7������ģ8����
	int send_base;//����δȷ�Ϸ������
	int nextseqnum;//��һ�������ͷ������
	Packet sndpkt[M];
	bool snd[M];

public:
	bool send(const Message& message);		//����Ӧ�ò�������Message����NetworkService����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ�ȷ��״̬���ʹ����������ܾ�����Message���򷵻�false
	void receive(const Packet& ackPkt);	//����ȷ��Ack������NetworkService����	
	void timeoutHandler(int seqNum);  //Timeout handler������NetworkService����
	bool getWaitingState();	//����RdtSender�Ƿ��ڵȴ�״̬��������ͷ����ȴ�ȷ�ϻ��߷��ʹ�������������true
public:
	SRSender();
	~SRSender();
};
