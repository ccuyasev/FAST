#include <iostream>
#include "Print.h"
using namespace std;
/*
 *初始化网络拓扑
*/
void initializeNetwork(Network& network){
	FILE* fp;
    fp = fopen("1device.txt", "r");
	if (NULL == fp)  //条件成立，则说明文件打开成功
        cout << "1device.txt doesn't exist" << endl;

	int timescope, timeaccuracy, timeslot, maxframe, maxline;
	int linerate, devnumber, linknumber;
	fscanf(fp,"%d %d %d %d %d %d %d %d",&timescope,&timeaccuracy,&timeslot,&maxframe,&maxline,&linerate,&devnumber,&linknumber);
	network.timescope = timescope;//时间粒度 1ns
	network.timeaccuracy = timeaccuracy;//时间同步的精度1000ns
    network.timeslot = timeslot;//时间槽 20000ns 
    network.maxframe = maxframe;//最大报文长度 1518 Bytes
    network.maxline = maxline;//最大线长 100m
    network.linerate = linerate;//网络线速 1000 Mbps
    network.devnumber = devnumber;//设备数量
    network.linknumber = linknumber;//链路数量
	network.devices = new Device[1000000];
	int devid, rdelay, fdelay, pdelay, sfdelay;
	bool isterminal;
	for(int i = 1; i <= network.devnumber; i++){
		fscanf(fp,"%d %d %d %d %d %d",&devid,&rdelay,&fdelay,&pdelay,&sfdelay,&isterminal);
		MAPNUM[devid] = i;
		network.devices[i].devid = devid;//设备编号
		network.devices[i].rdelay = rdelay;//接收时延
		network.devices[i].fdelay = fdelay;//转发时延
		network.devices[i].pdelay = pdelay;//处理时延
		network.devices[i].sfdelay = sfdelay/network.timeslot + 1;//存储（store）转发（forward）时延
		network.devices[i].isterminal = isterminal;//true:终端；false:交换机
		network.devices[i].firstedge = NULL;//设备间链路的指针
	}

	int linkid,predev,postdev,ldelay;
	bool isdirect = 0;
	Link *e;
	for(int i = 1;i <= network.linknumber; i++){
		fscanf(fp,"%d %d %d %d",&linkid,&predev,&postdev,&ldelay);
		link[i].linkid = linkid;//链路编号
		link[i].isdirect = 1;//true：有向边；false:无向边
		link[i].predev = predev;//前序节点的编号
		link[i].postdev = postdev;//后续节点的编号
		link[i].ldelay = ldelay;//链路时延
		if(network.devices[MAPNUM[predev]].firstedge == NULL){//当前顶点predev后面没有顶点
			e = new Link;
			e->linkid = linkid;
			e->isdirect = isdirect;
			e->predev = predev;
			e->postdev = postdev;
			e->ldelay = ldelay;
			e->link = NULL;
			
			network.devices[MAPNUM[predev]].firstedge = e;
		}
		else{
			Link *p = network.devices[MAPNUM[predev]].firstedge;
			while(p->link)//尾插法
				p = p->link;
			e = new Link;
			e->linkid = linkid;
			e->isdirect = isdirect;
			e->predev = predev;
			e->postdev = postdev;
			e->ldelay = ldelay;
			e->link = NULL;
			p->link = e;
		}
	}
	PrintGraph();
	
}
//初始化业务流
void initializeFlows(FlowSet& flowset){
	FILE* fp;
	FILE* dp;
    fp = fopen("2flowTT.txt", "r");


	if (NULL == fp)  //条件成立，则说明文件打开成功
        cout << "2flowTT.txt doesn't exist" << endl;


	dp = fopen("3flowlinkTT.txt", "r");
	if (NULL == dp)  //条件成立，则说明文件打开成功
        cout << "3flowlinkTT.txt doesn't exist" << endl;

	int fnumber;
	fscanf(fp,"%d",&fnumber);
	flowset.fnumber = fnumber;
	flowset.flows = new Flow[fnumber+100000];

	int fid, length, period, etedelay;
	int rdevnumber, linknumber, schedlinknumber;
	int offsetneed;

	for(int i = 1; i <= fnumber; i++){

		fscanf(fp, "%d %d %d %d %d %d %d %d", &fid, &length, &period, &etedelay, &rdevnumber, &linknumber, &schedlinknumber, &offsetneed);
		//fscanf(fp, "%d %d %d %d %d %d %d", &fid, &length, &period, &etedelay, &rdevnumber, &linknumber, &schedlinknumber); offsetneed = 0;

		flowset.flows[i].fid = fid;//业务流编号
		flowset.flows[i].length = length;//报文的长度
		flowset.flows[i].period = period/network.timeslot ;//业务流的周期
		flowset.flows[i].etedelay = etedelay/network.timeslot ;//端到端时延
		flowset.flows[i].rdevnumber = rdevnumber;//接收终端的数量
		flowset.flows[i].linknumber = linknumber;//路由中链路的数量
		flowset.flows[i].schedlinknumber = schedlinknumber;//路由中被调度的链路数量
		if(offsetneed % network.timeslot == 0)
			flowset.flows[i].offsetneed = offsetneed/network.timeslot;//规定初始的发送时间点，计算得到的结果不能早于该时间点
		else
			flowset.flows[i].offsetneed = (offsetneed/network.timeslot) + 1;
		

		flowset.flows[i].route = new Route[linknumber+10];

		int a, b;
		for(int j = 1; j <= linknumber; j++){
			fscanf(dp,"%d %d ",&a,&b);

			Link *e = network.devices[MAPNUM[a]].firstedge;					
			while(e != NULL && e->postdev != b){
				e = e->link;
			}
			
			int linkidnew = e->linkid;
		//	cout<<linkidnew<<endl;
			
			flowset.flows[i].route[j].preroute = j-1;//单播
		//	cout<<i<<" "<<j<<endl;
			flowset.flows[i].route[j].linkid = linkidnew;
		//	cout<<flowset.flows[i].route[j].linkid<<endl;
		//	cout<<flowset.flows[i].route[j].preroute<<endl;
		//	cout<<"==============="<<endl;			
		}

	}

	for (int i = 1; i <= flowset.fnumber; i++){
		for(int j = 1; j <= flowset.flows[i].linknumber; j++){
			int flag = 0;
			for(int k = 1; k <= NumPre; k++){
				if(num[k] == flowset.flows[i].route[j].linkid){
					flag = 1;
					break;
				}
			}
			if(flag == 0){			
				num[NumPre] = flowset.flows[i].route[j].linkid;
				NumPre++;
			}
		}
	}

	NumPre--;
//	PrintFlow();
}
//创建先序关系图
void createPreGraph(FlowSet& flowset){
	pregraph.vnumber = NumPre;//顶点的个数
	pregraph.schedfnumber = 0;//被调度的业务流数量，初始为0
	pregraph.vertex = new PreGVertex[pregraph.vnumber+100000];

	for(int i = 1;i <= NumPre; i++){
		pregraph.vertex[i].link = link[num[i]];
		pregraph.vertex[i].flowurgency = new FlowUrgency[100000];
		pregraph.vertex[i].firstedge = NULL;
		pregraph.vertex[i].flownumber = 0;
		pregraph.vertex[i].indegree = 0;
	}
	
	for(int i = 1; i <= flowset.fnumber; i++){
		for(int j = 1; j <= flowset.flows[i].linknumber; j++){
			for(int k = 1; k <= NumPre; k++){
				if(pregraph.vertex[k].link.linkid == flowset.flows[i].route[j].linkid){
					pregraph.vertex[k].flownumber++;
					pregraph.vertex[k].flownumberTT++;
					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].fid = i;
					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].accdelay = 0;
					//最小转发时延
					int devid = link[flowset.flows[i].route[j].linkid].predev;
					int mindelay=(network.devices[MAPNUM[devid]].rdelay + network.devices[MAPNUM[devid]].fdelay + network.devices[MAPNUM[devid]].pdelay + 
					link[flowset.flows[i].route[j].linkid].ldelay + network.timeaccuracy)/network.timeslot + 1;
					// cout<<network.devices[MAPNUM[devid]].rdelay<<endl;
					// cout<<network.devices[MAPNUM[devid]].fdelay<<endl;
					// cout<<network.devices[MAPNUM[devid]].pdelay<<endl;
					// cout<<link[flowset.flows[i].route[j].linkid].ldelay<<endl;
					// system("pause");
					//剩余最小传输时延
					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].remainmindelay = (flowset.flows[i].linknumber - j + 1) * mindelay;
					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].isscheduled = false;
					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].isbreakloop = false;
					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].isfeasible = true;
					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].isRCflow = false;
					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].offsetdelay = 0;
					PreGEdge *p,*e;

					if(j != flowset.flows[i].linknumber){
						if(pregraph.vertex[k].firstedge == NULL){
							p = new PreGEdge;
							p->flowid = i;
							p->prelinkid = flowset.flows[i].route[j].linkid;
							p->postlinkid = flowset.flows[i].route[j+1].linkid;
							p->edge = NULL;
							pregraph.vertex[k].firstedge = p;
						}
						else {
							PreGEdge *q = pregraph.vertex[k].firstedge;
							while(q->edge)//尾插法
								q = q->edge;
							e = new PreGEdge;
							e->flowid = i;
							e->prelinkid = flowset.flows[i].route[j].linkid;
							e->postlinkid = flowset.flows[i].route[j+1].linkid;
							e->edge = NULL;
							q->edge = e;
						}
					}
				}				
			}
		}
	}
	for(int i = 1; i <= NumPre; i++){
		PreGEdge *e = pregraph.vertex[i].firstedge;
		while (e){
			for(int j = 1; j <= NumPre; j++){
				if(pregraph.vertex[j].link.linkid == e->postlinkid)
				pregraph.vertex[j].indegree++;
			}
			e = e->edge;
		}
	}
//	PrintPreGraph();	
	// for(int i = 1; i <= NumPre; i++){
	// 	cout << pregraph.vertex[i].link.predev << "," << pregraph.vertex[i].link.postdev;
	// 	cout<<":"<<pregraph.vertex[i].indegree<<endl;
	// }
}