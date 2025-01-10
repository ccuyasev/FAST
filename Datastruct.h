#include <unordered_set>

//数据链路
struct Link{
     unsigned int linkid;//链路编号
     bool isdirect;//true：有向边；false:无向边
     unsigned int predev;//前序节点的编号
     unsigned int postdev;//后续节点的编号
     unsigned int ldelay;//链路时延
     Link* link;//指向另一条链路
};
//设备（包括终端和交换机）的结构体
struct Device{
     unsigned int devid;//设备编号
     unsigned int rdelay;//接收时延
     unsigned int fdelay;//转发时延
     unsigned int pdelay;//处理时延
     unsigned int sfdelay;//存储（store）转发（forward）时延
     Link* firstedge;//设备间链路的指针
     bool isterminal;//true:终端；false:交换机
};

//路由
struct Route{
	unsigned int linkid;//链路编号
	unsigned int preroute;//前一跳路由
};
//业务流
struct Flow{
     unsigned int fid;//业务流编号
     unsigned int length;//报文的长度
     unsigned int period;//业务流的周期
     unsigned int etedelay;//端到端时延
     unsigned int rdevnumber;//接收终端的数量
     unsigned int linknumber;//路由中链路的数量
     unsigned int schedlinknumber;//路由中被调度的链路数量
	unsigned int offsetneed;//规定初始的发送时间点，计算得到的结果不能早于该时间点
     Route* route;
     Route* route_change;//重路由的路径
     unsigned int linknumber_change;//重路由的路径中链路的数量
};
//业务流的集合
struct FlowSet{
	unsigned int fnumber;//业务流的条数
    unsigned int fnumberRC;//RC业务流的条数
	Flow* flows;//业务流的数组
};
//网络拓扑
struct Network{
     unsigned int timescope;//时间粒度 1ns
     unsigned int timeaccuracy;//时间同步的精度
     unsigned int timeslot;//时间槽20000ns 
     unsigned int maxframe;//最大报文长度
     unsigned int maxline;//最大线长
     unsigned int linerate;//网络线速
     unsigned int devnumber;//设备数量
     unsigned int linknumber;//链路数量
     Device* devices;
};
//业务流的紧急程度
struct FlowUrgency {
     unsigned int fid;//业务流编号
     unsigned int accdelay;//累积的传输时延
     unsigned int remainmindelay;//剩余最小传输时延
     bool isscheduled;//true:已经被调度；false:尚未被调度
     bool isfeasible;//true:可行的调度；false:无法被调度
     bool isbreakloop;// true:用于破环的业务流；false:正常业务流
     bool isRCflow;// true:是RC业务流；false：是TT业务流

     unsigned int offsetdelay;//在交换机中的驻留时间
     int urgency;//紧急程度
     unsigned int offset; //业务流的发送时间点
};
//先序关系图的边
struct PreGEdge{
   unsigned int flowid;//业务流编号
   unsigned int prelinkid;//前一条链路编号
   unsigned int postlinkid;//后一条链路编号
   PreGEdge* edge;//指向另一条边
};
//先序关系图的顶点
struct PreGVertex{
	Link link;
	unsigned int flownumber;//链路上业务流的数量
     unsigned int flownumberTT;//链路上TT业务流的数量
     unsigned int flownumberRC;//链路上RC业务流的数量
	FlowUrgency* flowurgency;//链路上存在多条业务流
	int indegree;//入度，-1表示该节点已经输出到拓扑序列
	PreGEdge* firstedge;//出该节点的边
};

//破环
// template<class T>
struct BreakLoopPoint{
   unsigned int fid;//业务流编号
   unsigned int linkid;//当前业务流待调度的链路编号
};
//先序关系图
struct PreGraph{
     unsigned int vnumber;//顶点的个数
     unsigned int schedfnumber;//被调度的业务流数量，初始为0
     PreGVertex* vertex;
};

FlowSet flowset;//业务流
Network network;//网络拓扑
PreGraph pregraph;//线序关系图
Device devices[10001];//设备
int MAPNUM[100001];//设备号转为
int num[100001];//记录数据流中的linkid
int NumPre = 1;
Link link[100001];//数据链路
int unschedulenum = 0;//无法调度的业务流条数
int unschedulenumRC = 0;//无法调度的业务流条数
int MAX = 0x7fffffff;//无法调度的业务流，定义offset为最大值
int Flownumber = 0;
