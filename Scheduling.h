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
//��ʼ����ɣ���ʼ����ҵ����
int topologyBasedScheduling(Network& network, FlowSet& flowset, PreGraph& pregraph) {
	queue<unsigned int> fifo;//��������ʱ���ڴ洢���Ϊ0����·���
	stack<BreakLoopPoint> Stack;//���ڴ洢�ƻ���ҵ������ź��ƻ����

	for (int i = 1; i <= pregraph.vnumber; i++) {//�����Ϊ0 ����·�����
		if (pregraph.vertex[i].indegree == 0) {
			fifo.push(i);
		}
	}
	while (!areAllPreGVertexScheduled(pregraph)) {//��������
		while (fifo.empty()) {//�ƻ���ֱ���������Ϊ0�Ķ���
			//�ҳ������̶���͵�ҵ���������ƻ�
			BreakLoopPoint blp = findLowestUrgencyFlows();

			Stack.push(blp);//�洢�ƻ���ҵ������Ϣ

			//�������̶���͵�ҵ�����ӻ���ȥ�������Ϊ0�Ľڵ������
			breakLoop(pregraph, blp, fifo);
		}
		while (!fifo.empty()) {
			int previd = fifo.front();//������������ڵ�previd	
			//			cout << "fifo���������Ϊ0�Ľ�����:" << fifo.size() << endl;			
			fifo.pop();
			//����ڵ�previd������ҵ�����Ľ����̶�
			computeUrgency(previd, network, flowset, pregraph);

			//���ս����̶Ƚ�������
			sortByUrgency(previd);

			//���ս����̶�ȷ������ʱ��
			schedulingByUrgency(previd);

			pregraph.vertex[previd].indegree = -1;//��Ǹýڵ��ѱ�����

			//��������ڵ�Ϊprevid�Ķ��������ҵ������accdelay������ʱ״̬
			//�����Ϊ0�Ľڵ������
			updatePreGVertexDegree(previd, fifo, Stack);
		}
	}

	ofstream dataFile;
	dataFile.open("output\\stack.txt", ofstream::app);
	if (!dataFile) {
		cout << "���ļ�stack.txtʧ��" << endl;
		system("pause");
	}
	// ��TXT�ĵ���д������
	dataFile << " StacksizeTT:" << Stack.size() << endl;
	// �ر��ĵ�
	dataFile.close();

	//�����ƻ���TTҵ����
	while (!Stack.empty()) {
		//���������ƻ���ҵ����
		int previd = Stack.top().linkid;//������������ڵ�previd
		int fid = Stack.top().fid;
		//		cout << "---------------------Ŀǰ���ȵĽڵ�Ϊ:---------------------" << endl;
		//		cout << link[num[previd]].predev<<","<<link[num[previd]].postdev << endl;
		//		cout << "fidΪ:" << fid << endl;
		//		cout << "------------------------------------------------------------" << endl;
		ofstream dataFile;
		dataFile.open("output\\breakloop.txt", ofstream::app);
		// ��TXT�ĵ���д������
		dataFile << "fid:" << fid << endl;
		dataFile << link[num[previd]].predev << "," << link[num[previd]].postdev << endl;
		dataFile << "========================" << endl;
		// �ر��ĵ�
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
			//���ս����̶�ȷ������ʱ��
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

	//	//����RCҵ����
	//	//�ȶ�ȡRCҵ����
	//	FILE* fp;
	//	FILE* dp;
	//	fp = fopen("4flowRC.txt", "r");
	//	if (NULL == fp)  //������������˵���ļ��򿪳ɹ�
	//		cout << "4flowRC.txt doesn't exist" << endl;

	//	dp = fopen("5flowlinkRC.txt", "r");
	//	if (NULL == dp)  //������������˵���ļ��򿪳ɹ�
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
	//		flowset.flows[i].fid = fid + flowset.fnumber;//ҵ�������
	//		flowset.flows[i].length = length;//���ĵĳ���
	//		flowset.flows[i].period = period / network.timeslot;//ҵ����������
	//		flowset.flows[i].etedelay = etedelay / network.timeslot;//�˵���ʱ��
	//		flowset.flows[i].rdevnumber = rdevnumber;//�����ն˵�����
	//		flowset.flows[i].linknumber = linknumber;//·������·������
	//		flowset.flows[i].schedlinknumber = schedlinknumber;//·���б����ȵ���·����

	//		if (offsetneed % network.timeslot == 0)
	//			flowset.flows[i].offsetneed = offsetneed / network.timeslot;//�涨��ʼ�ķ���ʱ��㣬����õ��Ľ���������ڸ�ʱ���
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
	//			flowset.flows[i].route[j].preroute = j - 1;//����
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


	//	//	cout << "����������������������ʾ��" << endl;

	//		// for (int i = 1; i<= number; i++){
	//		// 	cout << "ҵ������ţ�"<<flowset.flows[i].fid<<endl;
	//		// 	cout << "·��·��Ϊ��";
	//		// 	for(int j = 1; j <= flowset.flows[i].linknumber; j++){
	//		// 		cout << link[flowset.flows[i].route[j].linkid].predev << ",";
	//		// 		cout << link[flowset.flows[i].route[j].linkid].postdev << " ";
	//		// 	}
	//		// 	cout << endl;
	//		// }

	//		//��RC�������������ϵͼ��
	//	pregraph.vnumber = NumPre;//����ĸ���

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
	//	// //��Сת��ʱ��
	//	// int mindelay=(15000 + 0 + 800 + 40 + network.timeaccuracy)/network.timeslot + 1;

	//	for (int i = fnumberTT; i <= number; i++) {
	//		for (int j = 1; j <= flowset.flows[i].linknumber; j++) {
	//			for (int k = 1; k <= NumPre; k++) {
	//				if (pregraph.vertex[k].link.linkid == flowset.flows[i].route[j].linkid) {
	//					pregraph.vertex[k].flownumber++;
	//					pregraph.vertex[k].flownumberRC++;
	//					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].fid = i;
	//					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].accdelay = 0;
	//					//��Сת��ʱ��
	//					int devid = link[flowset.flows[i].route[j].linkid].predev;
	//					int mindelay = (network.devices[MAPNUM[devid]].rdelay + network.devices[MAPNUM[devid]].fdelay + network.devices[MAPNUM[devid]].pdelay +
	//						link[flowset.flows[i].route[j].linkid].ldelay + network.timeaccuracy) / network.timeslot + 1;
	//					// cout<<network.devices[MAPNUM[devid]].rdelay<<endl;
	//					// cout<<network.devices[MAPNUM[devid]].fdelay<<endl;
	//					// cout<<network.devices[MAPNUM[devid]].pdelay<<endl;
	//					// cout<<link[flowset.flows[i].route[j].linkid].ldelay<<endl;
	//					// system("pause");
	//					//ʣ����С����ʱ��
	//					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].remainmindelay = (flowset.flows[i].linknumber - j + 1) * mindelay;
	//					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].isscheduled = false;
	//					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].isbreakloop = false;
	//					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].isfeasible = true;
	//					pregraph.vertex[k].flowurgency[pregraph.vertex[k].flownumber].isRCflow = true;//RCҵ����
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
	//							while (q->edge)//β�巨
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
	//					//�ҵ�����ҵ����
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
	//					//�ҵ�����ҵ����
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
	//	queue<unsigned int> fifoRC;//��������ʱ���ڴ洢���Ϊ0����·���
	//	stack<BreakLoopPoint> StackRC;//���ڴ洢�ƻ���ҵ������ź��ƻ����

	//	for (int i = 1; i <= pregraph.vnumber; i++) {//�����Ϊ0 ����·�����
	//		if (pregraph.vertex[i].indegree == 0) {
	//			fifoRC.push(i);
	//		}
	//	}
	//	// for(int i = 1; i <= NumPre; i++){
	//	// 	cout << pregraph.vertex[i].link.predev << "," << pregraph.vertex[i].link.postdev;
	//	// 	cout<<":"<<pregraph.vertex[i].indegree<<endl;
	//	// }
	//	//system("pause");

	//	while (!areAllPreGVertexScheduledRC(pregraph)) {//��������
	//		while (fifoRC.empty()) {//�ƻ���ֱ���������Ϊ0�Ķ���
	//			//			cout<<"breakloopRC"<<endl;
	//						//system("pause");
	//						//�ҳ������̶���͵�ҵ���������ƻ�
	//			BreakLoopPoint blpRC = findLowestUrgencyFlowsRC();

	//			StackRC.push(blpRC);//�洢�ƻ���ҵ������Ϣ

	//			//�������̶���͵�ҵ�����ӻ���ȥ�������Ϊ0�Ľڵ������
	//			breakLoopRC(pregraph, blpRC, fifoRC);
	//		}
	//		while (!fifoRC.empty()) {
	//			// for(int i = 1; i <= NumPre; i++){
	//			// 	cout << pregraph.vertex[i].link.predev << "," << pregraph.vertex[i].link.postdev;
	//			// 	cout<<":"<<pregraph.vertex[i].indegree<<endl;
	//			// 	cout<<"========================================="<<endl;							
	//			// }
	//			// cout<<"========================================="<<endl;				
	//			int previdRC = fifoRC.front();//������������ڵ�previd	
	//			//system("pause");
	//			//cout << "fifoRC���������Ϊ0�Ľ�����:" << fifoRC.size() << endl;
	//			//cout<<"��ǰ���ȵĽڵ�Ϊ:"<<pregraph.vertex[previdRC].link.predev<< "," << pregraph.vertex[previdRC].link.postdev<<endl;

	//			// for(int i = 1; i <= NumPre; i++){
	//			// 	cout << pregraph.vertex[i].link.predev << "," << pregraph.vertex[i].link.postdev;
	//			// 	cout<<":"<<pregraph.vertex[i].indegree<<endl;

	//			// }
	//			//system("pause");

	//			fifoRC.pop();
	//			//����ڵ�previd������ҵ�����Ľ����̶�
	//			computeUrgencyRC(previdRC, network, flowset, pregraph);

	//			//���ս����̶Ƚ�������
	//			sortByUrgencyRC(previdRC);

	//			//���ս����̶�ȷ������ʱ��
	//			schedulingByUrgencyRC(previdRC);

	//			pregraph.vertex[previdRC].indegree = -1;//��Ǹýڵ��ѱ�����

	//			//��������ڵ�Ϊprevid�Ķ��������ҵ������accdelay������ʱ״̬
	//			//�����Ϊ0�Ľڵ������
	//			updatePreGVertexDegreeRC(previdRC, fifoRC, StackRC);

	//		}
	//	}

	//	ofstream dataFileRC;
	//	dataFileRC.open("output\\stack.txt", ofstream::app);
	//	if (!dataFileRC) cout << "���ļ�ʧ��" << endl;
	//	// ��TXT�ĵ���д������
	//	dataFileRC << " StacksizeRC:" << StackRC.size() << endl;
	//	// �ر��ĵ�
	//	dataFileRC.close();

	//	//�����ƻ���RCҵ����
	//	while (!StackRC.empty()) {
	//		//���������ƻ���ҵ����
	//		int previd = StackRC.top().linkid;//������������ڵ�previd
	//		int fid = StackRC.top().fid;
	//		//		cout << "---------------------Ŀǰ���ȵ��ƻ��ڵ�ΪRC:---------------------" << endl;
	//		//		cout << link[num[previd]].predev<<","<<link[num[previd]].postdev << endl;
	//		//		cout << "fidΪ:" << fid << endl;
	//		//		cout << "------------------------------------------------------------" << endl;
	//		ofstream dataFile;
	//		dataFile.open("output\\breakloopRC.txt", ofstream::app);
	//		// ��TXT�ĵ���д������
	//		dataFile << "fid:" << fid << endl;
	//		dataFile << link[num[previd]].predev << "," << link[num[previd]].postdev << endl;
	//		dataFile << "========================" << endl;
	//		// �ر��ĵ�
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
	//			//���ս����̶�ȷ������ʱ��
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