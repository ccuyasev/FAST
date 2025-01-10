#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <fstream>
#include <time.h>
#include "Fast.h"
#include "FastRC.h"
#include "BreakLoop.h"
#include "BreakLoopRC.h"
//初始化完成，开始调度业务流
int topologyBasedScheduling(Network& network, FlowSet& flowset, PreGraph& pregraph) {
	queue<unsigned int> fifo;//拓扑排序时用于存储入度为0的链路编号
	stack<BreakLoopPoint> Stack;//用于存储破环的业务流编号和破环起点

	for (int i = 1; i <= pregraph.vnumber; i++) {//将入度为0 的链路入队列
		if (pregraph.vertex[i].indegree == 0) {
			fifo.push(i);
		}
	}
	while (!areAllPreGVertexScheduled(pregraph)) {//继续调度
		while (fifo.empty()) {//破环，直到出现入度为0的顶点
			//找出紧急程度最低的业务流进行破环
			BreakLoopPoint blp = findLowestUrgencyFlows();

			Stack.push(blp);//存储破环的业务流信息

			//将紧急程度最低的业务流从环中去掉，入度为0的节点入队列
			breakLoop(pregraph, blp, fifo);
		}
		while (!fifo.empty()) {
			int previd = fifo.front();//拓扑序列输出节点previd	
			//			cout << "fifo队列中入度为0的结点个数:" << fifo.size() << endl;			
			fifo.pop();
			//计算节点previd上所有业务流的紧急程度
			computeUrgency(previd, network, flowset, pregraph);

			//按照紧急程度进行排序
			sortByUrgency(previd);

			//按照紧急程度确定发送时间
			schedulingByUrgency(previd);

			pregraph.vertex[previd].indegree = -1;//标记该节点已被调度

			//更新先序节点为previd的顶点度数、业务流的accdelay、运行时状态
			//将入度为0的节点入队列
			updatePreGVertexDegree(previd, fifo, Stack);
		}
	}

	ofstream dataFile;
	dataFile.open("output\\stack.txt", ofstream::app);
	if (!dataFile) {
		cout << "打开文件stack.txt失败" << endl;
		system("pause");
	}
	// 朝TXT文档中写入数据
	dataFile << " StacksizeTT:" << Stack.size() << endl;
	// 关闭文档
	dataFile.close();

	//调度破环的TT业务流
	while (!Stack.empty()) {
		//处理用于破环的业务流
		int previd = Stack.top().linkid;//拓扑序列输出节点previd
		int fid = Stack.top().fid;
		//		cout << "---------------------目前调度的节点为:---------------------" << endl;
		//		cout << link[num[previd]].predev<<","<<link[num[previd]].postdev << endl;
		//		cout << "fid为:" << fid << endl;
		//		cout << "------------------------------------------------------------" << endl;
		ofstream dataFile;
		dataFile.open("output\\breakloop.txt", ofstream::app);
		// 朝TXT文档中写入数据
		dataFile << "fid:" << fid << endl;
		dataFile << link[num[previd]].predev << "," << link[num[previd]].postdev << endl;
		dataFile << "========================" << endl;
		// 关闭文档
		dataFile.close();

		Stack.pop();
		//		cout << "Stack.size():" << Stack.size() << endl;;
		int j = 0;
		for (int i = 1; i <= flowset.flows[fid].linknumber; i++) {
			if (num[previd] == flowset.flows[fid].route[i].linkid) {
				j = i;
				break;
			}
		}
		while (j != flowset.flows[fid].linknumber + 1) {
			//按照紧急程度确定发送时间
			StackschedulingByUrgency(previd, fid, j);
			j++;
			for (int k = 1; k <= NumPre; k++) {
				if (num[k] == flowset.flows[fid].route[j].linkid) {
					previd = k;
					break;
				}
			}

		}
	}



	// for(int i = 1; i <= NumPre; i++){
	// 	cout << pregraph.vertex[i].link.predev << "," << pregraph.vertex[i].link.postdev;
	// 	cout<<":"<<pregraph.vertex[i].indegree<<endl;
	// }
	//system("pause");

	{

	//	//调度RC业务流
	//	//先读取RC业务流
	//	FILE* fp;
	//	FILE* dp;
	//	fp = fopen("4flowRC.txt", "r");
	//	if (NULL == fp)  //条件成立，则说明文件打开成功
	//		cout << "4flowRC.txt doesn't exist" << endl;

	//	dp = fopen("5flowlinkRC.txt", "r");
	//	if (NULL == dp)  //条件成立，则说明文件打开成功
	//		cout << "5flowlinkRC.txt doesn't exist" << endl;

	//	int fnumberTT = flowset.fnumber + 1;

	//	int fnumberRC;
	//	fscanf(fp, "%d", &fnumberRC);
	//	if (fnumberRC == 0) return 0;
	//	flowset.fnumberRC = fnumberRC;

	//	//flowset.flows = new Flow[fnumber+100000];

	//	int fid, length, period, etedelay;
	//	int rdevnumber, linknumber, schedlinknumber;
	//	int offsetneed;

	//	int number = fnumberTT + fnumberRC - 1;

	//	for (int i = fnumberTT; i <= number; i++) {
	//		fscanf(fp, "%d %d %d %d %d %d %d %d", &fid, &length, &period, &etedelay, &rdevnumber, &linknumber, &schedlinknumber, &offsetneed);
	//		flowset.flows[i].fid = fid + flowset.fnumber;//业务流编号
	//		flowset.flows[i].length = length;//报文的长度
	//		flowset.flows[i].period = period / network.timeslot;//业务流的周期
	//		flowset.flows[i].etedelay = etedelay / network.timeslot;//端到端时延
	//		flowset.flows[i].rdevnumber = rdevnumber;//接收终端的数量
	//		flowset.flows[i].linknumber = linknumber;//路由中链路的数量
	//		flowset.flows[i].schedlinknumber = schedlinknumber;//路由中被调度的链路数量

	//		if (offsetneed % network.timeslot == 0)
	//			flowset.flows[i].offsetneed = offsetneed / network.timeslot;//规定初始的发送时间点，计算得到的结果不能早于该时间点
	//		else
	//			flowset.flows[i].offsetneed = (offsetneed / network.timeslot) + 1;


	//		flowset.flows[i].route = new Route[linknumber + 10];

	//		int a, b;
	//		for (int j = 1; j <= linknumber; j++) {
	//			fscanf(dp, "%d %d ", &a, &b);
	//			Link* e = network.devices[MAPNUM[a]].firstedge;
	//			while (e != NULL && e->postdev != b) {
	//				e = e->link;
	//			}

	//			int linkidnew = e->linkid;
	//			flowset.flows[i].route[j].preroute = j - 1;//单播
	//			flowset.flows[i].route[j].linkid = linkidnew;
	//		}
	//	}
	//	int NumberTT = NumPre;
	//	NumPre++;
	//	for (int i = fnumberTT; i <= number; i++) {
	//		for (int j = 1; j <= flowset.flows[i].linknumber; j++) {
	//			int flag = 0;
	//			for (int k = 1; k <= NumPre; k++) {
	//				if (num[k] == flowset.flows[i].route[j].linkid) {
	//					flag = 1;
	//					break;
	//				}
	//			}
	//			if (flag == 0) {
	//				num[NumPre] = flowset.flows[i].route[j].linkid;
	//				NumPre++;
	//			}
	//		}
	//	}
	//	NumPre--;


	//	//	cout << "所建立的数据流如以下所示：" << endl;

	//		// for (int i = 1; i<= number; i++){
	//		// 	cout << "业务流编号："<<flowset.flows[i].fid<<endl;
	//		// 	cout << "路由路径为：";
	//		// 	for(int j = 1; j <= flowset.flows[i].linknumber; j++){
	//		// 		cout << link[flowset.flows[i].route[j].linkid].predev << ",";
	//		// 		cout << link[flowset.flows[i].route[j].linkid].postdev << " ";
	//		// 	}
	//		// 	cout << endl;
	//		// }

	//		//将RC流量加入先序关系图中
	//	pregraph.vnumber = NumPre;//顶点的个数

	//	//pregraph.vertex = new PreGVertex[pregraph.vnumber+100000];
	////	cout<<NumberTT<<endl;
	////	cout<<NumPre<<endl;

	//	for (int i = NumberTT + 1; i <= NumPre; i++) {
	//		pregraph.vertex[i].link = link[num[i]];
	//		pregraph.vertex[i].flowurgency = new FlowUrgency[100000];
	//		pregraph.vertex[i].firstedge = NULL;
	//		pregraph.vertex[i].flownumber = 0;
	//		pregraph.vertex[i].indegree = 0;
	//	}
	//	// //最小转发时延
	//	// int mindelay=(15000 + 0 + 800 + 40 + network.timeaccuracy)/network.timeslot + 1;

	//	for (int i = fnumberTT; i <= number; i++) {
	//		for (int j = 1; j <= flowset.flows[i].linknumber; j++) {
	//			for (int k = 1; k <= NumPre; k++) {
	//				if (pregraph.vertex[k].link.linkid == flowset.flows[i].route[j].linkid) {
	//					pregraph.vertex[k].flownumber++;
	//					pregraph.vertex[k].flownumberRC++;
	//					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].fid = i;
	//					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].accdelay = 0;
	//					//最小转发时延
	//					int devid = link[flowset.flows[i].route[j].linkid].predev;
	//					int mindelay = (network.devices[MAPNUM[devid]].rdelay + network.devices[MAPNUM[devid]].fdelay + network.devices[MAPNUM[devid]].pdelay +
	//						link[flowset.flows[i].route[j].linkid].ldelay + network.timeaccuracy) / network.timeslot + 1;
	//					// cout<<network.devices[MAPNUM[devid]].rdelay<<endl;
	//					// cout<<network.devices[MAPNUM[devid]].fdelay<<endl;
	//					// cout<<network.devices[MAPNUM[devid]].pdelay<<endl;
	//					// cout<<link[flowset.flows[i].route[j].linkid].ldelay<<endl;
	//					// system("pause");
	//					//剩余最小传输时延
	//					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].remainmindelay = (flowset.flows[i].linknumber - j + 1) * mindelay;
	//					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].isscheduled = false;
	//					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].isbreakloop = false;
	//					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].isfeasible = true;
	//					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].isRCflow = true;//RC业务流
	//					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].offsetdelay = 0;

	//					PreGEdge* p, * e;
	//					if (j != flowset.flows[i].linknumber) {
	//						if (pregraph.vertex[k].firstedge == NULL) {
	//							p = new PreGEdge;
	//							p->flowid = i;
	//							p->prelinkid = flowset.flows[i].route[j].linkid;
	//							p->postlinkid = flowset.flows[i].route[j + 1].linkid;
	//							p->edge = NULL;
	//							pregraph.vertex[k].firstedge = p;
	//						}
	//						else {
	//							PreGEdge* q = pregraph.vertex[k].firstedge;
	//							while (q->edge)//尾插法
	//								q = q->edge;
	//							e = new PreGEdge;
	//							e->flowid = i;
	//							e->prelinkid = flowset.flows[i].route[j].linkid;
	//							e->postlinkid = flowset.flows[i].route[j + 1].linkid;
	//							e->edge = NULL;
	//							q->edge = e;
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//	// PrintPreGraph();
	//	// cout<<"==========================="<<endl;
	//	// system("pause");
	//	for (int i = 1; i <= NumPre; i++) {
	//		PreGEdge* e = pregraph.vertex[i].firstedge;

	//		while (e) {
	//			for (int j = 1; j <= NumPre; j++) {
	//				if (pregraph.vertex[j].link.linkid == e->postlinkid) {
	//					//找到这条业务流
	//					int number = pregraph.vertex[j].flownumberRC + pregraph.vertex[j].flownumberTT;
	//					for (int p = 1; p <= number; p++) {
	//						if (pregraph.vertex[j].flowurgency[p].fid == e->flowid
	//							&& pregraph.vertex[j].flowurgency[p].isRCflow == true) {
	//							// cout << pregraph.vertex[j].link.predev << "," << pregraph.vertex[j].link.postdev;
	//							// cout<<"pregraph.vertex[j].indegree:"<<pregraph.vertex[j].indegree<<endl;
	//							// system("pause");
	//							if (pregraph.vertex[j].indegree == -1)
	//								pregraph.vertex[j].indegree = 1;
	//							else
	//								pregraph.vertex[j].indegree++;
	//						}
	//					}

	//				}
	//				if (pregraph.vertex[j].link.linkid == e->prelinkid) {
	//					//找到这条业务流
	//					int number = pregraph.vertex[j].flownumberRC + pregraph.vertex[j].flownumberTT;
	//					for (int p = 1; p <= number; p++) {
	//						if (pregraph.vertex[j].flowurgency[p].fid == e->flowid
	//							&& pregraph.vertex[j].flowurgency[p].isRCflow == true) {
	//							// cout << pregraph.vertex[j].link.predev << "," << pregraph.vertex[j].link.postdev;
	//							// cout<<"pregraph.vertex[j].indegree:"<<pregraph.vertex[j].indegree<<endl;
	//							// system("pause");
	//							if (e->prelinkid == flowset.flows[e->flowid].route[1].linkid)
	//								pregraph.vertex[j].indegree = 0;
	//							// cout << pregraph.vertex[j].link.predev << "," << pregraph.vertex[j].link.postdev;
	//							// cout<<"======pregraph.vertex[j].indegree======:"<<pregraph.vertex[j].indegree<<endl;
	//							// system("pause");
	//						}
	//					}
	//				}

	//			}
	//			e = e->edge;
	//		}
	//	}
	//	// PrintPreGraph();
	//	// for(int i = 1; i <= NumPre; i++){
	//	// 	cout << pregraph.vertex[i].link.predev << "," << pregraph.vertex[i].link.postdev;
	//	// 	cout<<":"<<pregraph.vertex[i].indegree<<endl;
	//	// }
	//	// system("pause");

	////============================================================================
	//	queue<unsigned int> fifoRC;//拓扑排序时用于存储入度为0的链路编号
	//	stack<BreakLoopPoint> StackRC;//用于存储破环的业务流编号和破环起点

	//	for (int i = 1; i <= pregraph.vnumber; i++) {//将入度为0 的链路入队列
	//		if (pregraph.vertex[i].indegree == 0) {
	//			fifoRC.push(i);
	//		}
	//	}
	//	// for(int i = 1; i <= NumPre; i++){
	//	// 	cout << pregraph.vertex[i].link.predev << "," << pregraph.vertex[i].link.postdev;
	//	// 	cout<<":"<<pregraph.vertex[i].indegree<<endl;
	//	// }
	//	//system("pause");

	//	while (!areAllPreGVertexScheduledRC(pregraph)) {//继续调度
	//		while (fifoRC.empty()) {//破环，直到出现入度为0的顶点
	//			//			cout<<"breakloopRC"<<endl;
	//						//system("pause");
	//						//找出紧急程度最低的业务流进行破环
	//			BreakLoopPoint blpRC = findLowestUrgencyFlowsRC();

	//			StackRC.push(blpRC);//存储破环的业务流信息

	//			//将紧急程度最低的业务流从环中去掉，入度为0的节点入队列
	//			breakLoopRC(pregraph, blpRC, fifoRC);
	//		}
	//		while (!fifoRC.empty()) {
	//			// for(int i = 1; i <= NumPre; i++){
	//			// 	cout << pregraph.vertex[i].link.predev << "," << pregraph.vertex[i].link.postdev;
	//			// 	cout<<":"<<pregraph.vertex[i].indegree<<endl;
	//			// 	cout<<"========================================="<<endl;							
	//			// }
	//			// cout<<"========================================="<<endl;				
	//			int previdRC = fifoRC.front();//拓扑序列输出节点previd	
	//			//system("pause");
	//			//cout << "fifoRC队列中入度为0的结点个数:" << fifoRC.size() << endl;
	//			//cout<<"当前调度的节点为:"<<pregraph.vertex[previdRC].link.predev<< "," << pregraph.vertex[previdRC].link.postdev<<endl;

	//			// for(int i = 1; i <= NumPre; i++){
	//			// 	cout << pregraph.vertex[i].link.predev << "," << pregraph.vertex[i].link.postdev;
	//			// 	cout<<":"<<pregraph.vertex[i].indegree<<endl;

	//			// }
	//			//system("pause");

	//			fifoRC.pop();
	//			//计算节点previd上所有业务流的紧急程度
	//			computeUrgencyRC(previdRC, network, flowset, pregraph);

	//			//按照紧急程度进行排序
	//			sortByUrgencyRC(previdRC);

	//			//按照紧急程度确定发送时间
	//			schedulingByUrgencyRC(previdRC);

	//			pregraph.vertex[previdRC].indegree = -1;//标记该节点已被调度

	//			//更新先序节点为previd的顶点度数、业务流的accdelay、运行时状态
	//			//将入度为0的节点入队列
	//			updatePreGVertexDegreeRC(previdRC, fifoRC, StackRC);

	//		}
	//	}

	//	ofstream dataFileRC;
	//	dataFileRC.open("output\\stack.txt", ofstream::app);
	//	if (!dataFileRC) cout << "打开文件失败" << endl;
	//	// 朝TXT文档中写入数据
	//	dataFileRC << " StacksizeRC:" << StackRC.size() << endl;
	//	// 关闭文档
	//	dataFileRC.close();

	//	//调度破环的RC业务流
	//	while (!StackRC.empty()) {
	//		//处理用于破环的业务流
	//		int previd = StackRC.top().linkid;//拓扑序列输出节点previd
	//		int fid = StackRC.top().fid;
	//		//		cout << "---------------------目前调度的破环节点为RC:---------------------" << endl;
	//		//		cout << link[num[previd]].predev<<","<<link[num[previd]].postdev << endl;
	//		//		cout << "fid为:" << fid << endl;
	//		//		cout << "------------------------------------------------------------" << endl;
	//		ofstream dataFile;
	//		dataFile.open("output\\breakloopRC.txt", ofstream::app);
	//		// 朝TXT文档中写入数据
	//		dataFile << "fid:" << fid << endl;
	//		dataFile << link[num[previd]].predev << "," << link[num[previd]].postdev << endl;
	//		dataFile << "========================" << endl;
	//		// 关闭文档
	//		dataFile.close();

	//		StackRC.pop();
	//		//cout << "StackRC.size():" << StackRC.size() << endl;;
	//		int j = 0;
	//		for (int i = 1; i <= flowset.flows[fid].linknumber; i++) {
	//			if (num[previd] == flowset.flows[fid].route[i].linkid) {
	//				j = i;
	//				break;
	//			}
	//		}
	//		while (j != flowset.flows[fid].linknumber + 1) {
	//			//按照紧急程度确定发送时间
	//			StackschedulingByUrgencyRC(previd, fid, j);
	//			j++;
	//			for (int k = 1; k <= NumPre; k++) {
	//				if (num[k] == flowset.flows[fid].route[j].linkid) {
	//					previd = k;
	//					break;
	//				}
	//			}

	//		}
	//	}
	}
	return 1;
	// for(int i = 1; i <= NumPre; i++){
	// 	cout << pregraph.vertex[i].link.predev << "," << pregraph.vertex[i].link.postdev;
	// 	cout<<":"<<pregraph.vertex[i].indegree<<endl;
	// }
	//system("pause");	
}