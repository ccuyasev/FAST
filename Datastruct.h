#include <unordered_set>

//������·
struct Link{
     unsigned int linkid;//��·���
     bool isdirect;//true������ߣ�false:�����
     unsigned int predev;//ǰ��ڵ�ı��
     unsigned int postdev;//�����ڵ�ı��
     unsigned int ldelay;//��·ʱ��
     Link* link;//ָ����һ����·
};
//�豸�������ն˺ͽ��������Ľṹ��
struct Device{
     unsigned int devid;//�豸���
     unsigned int rdelay;//����ʱ��
     unsigned int fdelay;//ת��ʱ��
     unsigned int pdelay;//����ʱ��
     unsigned int sfdelay;//�洢��store��ת����forward��ʱ��
     Link* firstedge;//�豸����·��ָ��
     bool isterminal;//true:�նˣ�false:������
};

//·��
struct Route{
	unsigned int linkid;//��·���
	unsigned int preroute;//ǰһ��·��
};
//ҵ����
struct Flow{
     unsigned int fid;//ҵ�������
     unsigned int length;//���ĵĳ���
     unsigned int period;//ҵ����������
     unsigned int etedelay;//�˵���ʱ��
     unsigned int rdevnumber;//�����ն˵�����
     unsigned int linknumber;//·������·������
     unsigned int schedlinknumber;//·���б����ȵ���·����
	unsigned int offsetneed;//�涨��ʼ�ķ���ʱ��㣬����õ��Ľ���������ڸ�ʱ���
     Route* route;
     Route* route_change;//��·�ɵ�·��
     unsigned int linknumber_change;//��·�ɵ�·������·������
};
//ҵ�����ļ���
struct FlowSet{
	unsigned int fnumber;//ҵ����������
    unsigned int fnumberRC;//RCҵ����������
	Flow* flows;//ҵ����������
};
//��������
struct Network{
     unsigned int timescope;//ʱ������ 1ns
     unsigned int timeaccuracy;//ʱ��ͬ���ľ���
     unsigned int timeslot;//ʱ���20000ns 
     unsigned int maxframe;//����ĳ���
     unsigned int maxline;//����߳�
     unsigned int linerate;//��������
     unsigned int devnumber;//�豸����
     unsigned int linknumber;//��·����
     Device* devices;
};
//ҵ�����Ľ����̶�
struct FlowUrgency {
     unsigned int fid;//ҵ�������
     unsigned int accdelay;//�ۻ��Ĵ���ʱ��
     unsigned int remainmindelay;//ʣ����С����ʱ��
     bool isscheduled;//true:�Ѿ������ȣ�false:��δ������
     bool isfeasible;//true:���еĵ��ȣ�false:�޷�������
     bool isbreakloop;// true:�����ƻ���ҵ������false:����ҵ����
     bool isRCflow;// true:��RCҵ������false����TTҵ����

     unsigned int offsetdelay;//�ڽ������е�פ��ʱ��
     int urgency;//�����̶�
     unsigned int offset; //ҵ�����ķ���ʱ���
};
//�����ϵͼ�ı�
struct PreGEdge{
   unsigned int flowid;//ҵ�������
   unsigned int prelinkid;//ǰһ����·���
   unsigned int postlinkid;//��һ����·���
   PreGEdge* edge;//ָ����һ����
};
//�����ϵͼ�Ķ���
struct PreGVertex{
	Link link;
	unsigned int flownumber;//��·��ҵ����������
     unsigned int flownumberTT;//��·��TTҵ����������
     unsigned int flownumberRC;//��·��RCҵ����������
	FlowUrgency* flowurgency;//��·�ϴ��ڶ���ҵ����
	int indegree;//��ȣ�-1��ʾ�ýڵ��Ѿ��������������
	PreGEdge* firstedge;//���ýڵ�ı�
};

//�ƻ�
// template<class T>
struct BreakLoopPoint{
   unsigned int fid;//ҵ�������
   unsigned int linkid;//��ǰҵ���������ȵ���·���
};
//�����ϵͼ
struct PreGraph{
     unsigned int vnumber;//����ĸ���
     unsigned int schedfnumber;//�����ȵ�ҵ������������ʼΪ0
     PreGVertex* vertex;
};

FlowSet flowset;//ҵ����
Network network;//��������
PreGraph pregraph;//�����ϵͼ
Device devices[10001];//�豸
int MAPNUM[100001];//�豸��תΪ
int num[100001];//��¼�������е�linkid
int NumPre = 1;
Link link[100001];//������·
int unschedulenum = 0;//�޷����ȵ�ҵ��������
int unschedulenumRC = 0;//�޷����ȵ�ҵ��������
int MAX = 0x7fffffff;//�޷����ȵ�ҵ����������offsetΪ���ֵ
int Flownumber = 0;
