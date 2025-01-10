#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <vector>
#include <cstring>
#include <queue>
#include <stack>
#include <algorithm>
#include <fstream>
#include <time.h>
#include <direct.h>
#include "Datastruct.h"
#include "Initialdata.h"
#include "Scheduling.h"
#include "Detect.h"
#include <cstdio>
#include <cstdlib>
//#include <bits/stdc++.h>

#include <typeinfo>
// #include <corecrt_io.h>
using namespace std;
#define MAXNODE 10000
#define MAXEDGE 10000
#define INFINITE 1e8
class EdgeNode { //图用邻接表来存储，这是边表节点
public:
	EdgeNode(int to = 'Z', int weight = 1) :to(to), weight(weight), weight2(weight) { next = NULL; }
	EdgeNode(const EdgeNode& en) {
		this->to = en.to;
		this->weight = en.weight;
		this->weight2 = en.weight;
		this->next = en.next;
	}
	EdgeNode& operator=(const EdgeNode& en)
	{
		this->to = en.to;
		this->weight = en.weight;
		this->weight2 = en.weight;
		this->next = en.next;
	}
	~EdgeNode() {}
	int to;
	int weight; //原值，不改变
	int weight2; //用于修改数值（即设置无穷大）并重置
	EdgeNode* next;
};
class VertexNode {//顶点表节点
public:
	VertexNode(int name = 'Z', EdgeNode* firstEdge = NULL) :name(name), firstEdge(firstEdge) {}
	int name;
	EdgeNode* firstEdge;
	~VertexNode() {}
};
VertexNode vn[MAXNODE];//全局数据结构
using namespace std;
int createGraph(int n, int m, int flag) { //构造图
	int from, to;
	int weight;
	for (int i = 1; i <= network.linknumber; i++) {
		//cin>>from>>to>>weight;
		from = link[i].predev;
		to = link[i].postdev;
		weight = 1;
		//如果是无向图
		if (flag == 0) {
			if (vn[from].name == 'Z') {
				vn[from].name = from;
				//cout<<"=========="<<vn[from].name<<endl;
			}
			if (vn[to].name == 'Z') {
				vn[to].name = to;
				//cout<<"=========="<<vn[to].name<<endl;
			}

			// cout<<vn[from].name<<endl;
			// cout<<vn[to].name<<endl;
			// cout<<from<<endl;
			// cout<<to<<endl;
			// system("pause");

			EdgeNode* edge_new = new EdgeNode(to, weight);
			EdgeNode* p = vn[from].firstEdge;
			if (vn[from].firstEdge == NULL) {
				vn[from].firstEdge = edge_new;
			}
			else {
				while (p->next != NULL)
					p = p->next;
				p->next = edge_new;
			}
			EdgeNode* edge_new2 = new EdgeNode(from, weight);
			p = vn[to].firstEdge;
			if (vn[to].firstEdge == NULL)
				vn[to].firstEdge = edge_new2;
			else {
				while (p->next != NULL)
					p = p->next;
				p->next = edge_new2;
			}
		}
		//有向图
		else {
			if (vn[from].name == 'Z') vn[from].name = from;
			if (vn[to].name == 'Z') vn[to].name = to;//这个点没有出边，这个点要保存在数组里
			EdgeNode* edge_new = new EdgeNode(to, weight);
			EdgeNode* p = vn[from].firstEdge;
			if (vn[from].firstEdge == NULL)
				vn[from].firstEdge = edge_new;
			else {
				while (p->next != NULL)
					p = p->next;
				p->next = edge_new;
			}
		}
	}
	return 1;
}
pair<int, vector<int>> dijkstra(int from, int to, int* t) { //求最短路径
	long max = 1e8;  //无穷大的值
	int distance = 0;
	int parent[MAXNODE];//更新时记录每个节点的父节点，便于找到最短路径，只有这个距离更新，此节点的父节点才会更新
	for (int i = 0; i < MAXNODE; i++)
		parent[i] = 'Z';//初始化所有节点父节点为'Z'
	vector<int>vec;//存储的是T中的每个节点，即最短路径，
	vector<pair<int, int>>queue;//代表的是T，即未扩展边的集合，用vector 实现
	for (int i = 0; i < MAXNODE; i++) {
		if (vn[i].name != 'Z') {
			if (vn[i].name == from && t[vn[i].name] == 0)//排除点不再入队
				queue.push_back(make_pair(0, vn[i].name));//起点的距离值为0
			else if (t[vn[i].name] == 0)
				queue.push_back(make_pair(max, vn[i].name));//初始化其他点的距离为max
		}
	}
	vector<pair<int, int>>::iterator it = queue.begin();
	while (!queue.empty()) {
		sort(queue.begin(), queue.end());//按距离值从小到大排序
		vector<pair<int, int>>::iterator tmp = queue.begin();
		if (queue.begin()->second == to) {
			distance = queue.begin()->first;//当弹出节点是终点时，便是终点的最短距离
			if (distance >= 1e8) {//终点的路径值大于等于无穷大，代表此路径不存在       
				distance = -1; //显示找不到路径，设置距离值为负值
				break;
			}
		}
		EdgeNode* p = vn[tmp->second].firstEdge;
		int d = tmp->first;//存储此节点出队的距离值
		while (p != NULL) {
			for (it = queue.begin() + 1; it != queue.end(); it++) {
				if (p->to == it->second) {//在T中找到要更新的点			
					if (d + p->weight2 < it->first) { //需要更新,相等的话则不再更新
						it->first = d + p->weight2;
						parent[it->second] = tmp->second; //更新父节点
					}
					break;
				}
			}
			p = p->next;
		}
		queue.erase(queue.begin());//删除首节点
	}

	int c = to;
	int i = 1;
	while (c != from && c != 'Z') {
		if (i > MAXNODE)//大于最大节点数，退出循环
			break;
		vec.push_back(c);
		c = parent[c];
		i++;
	}
	if (c == from)
		vec.push_back(c);//vector 是按照逆序存储的每个节点
	return make_pair(distance, vec);
}
void reset() {//修改后，将图复位
	for (int i = 0; i < MAXNODE; i++) {
		EdgeNode* p = vn[i].firstEdge;
		while (p != NULL) {
			p->weight2 = p->weight;
			p = p->next;
		}
	}
}

inline bool setinfinite(int start, int end) //设置无穷大
{
	int result = 0;
	EdgeNode* p = vn[start].firstEdge;//在vn上修改
	while (p != NULL) {
		if (p->to == end) {
			p->weight2 = INFINITE;//操作的是weight2
			result = 1;
		}
		p = p->next;
	}
	p = vn[end].firstEdge;//无向图,也适用于有向图
	while (p != NULL) {
		if (p->to == start) {
			p->weight2 = INFINITE;
			result = 1;
		}
		p = p->next;
	}
	return result;
}

bool search(int node, vector<pair<int, vector<int>>>ListA) { //在ListA中将节点node开始的所有边设为infinite

	for (vector<pair<int, vector<int>>>::iterator it = ListA.begin(); it != ListA.end(); it++) {
		for (vector<int>::iterator it2 = it->second.end() - 1; it2 > it->second.begin(); it2--) { //按照顺序逐个设置无穷大	
			if (*it2 == node) {
				vector<int>::iterator it3 = it2 - 1;
				if (!setinfinite(*it2, *it3))
					return 0;
			}
		}
	}
	return 1;
}

bool exist(vector<pair<int, vector<int>>>List, vector<int>vec) {//新序列是否在ListB中出现过

	for (vector<pair<int, vector<int>>>::iterator it = List.begin(); it != List.end(); it++) {
		if (it->second == vec)
			return 1;
	}
	return 0;
}
void print(vector<pair<int, vector<int>>>List) { //打印ListA、ListB的状态

	//if(!List.empty())
	//for(vector<pair<int,vector<int>>>::iterator it = List.begin();it != List.end();it++){
	//    for(vector<int>::iterator it2 = it->second.end()-1;it2>=it->second.begin();it2--)
	   //     cout<<*it2<<"  ";
		//cout<<it->first<<endl;
   // }
}
//调度破环的业务流
int StackschedulingByUrgency_optimization(int previd, int fid, int index) {
	int flownumber = pregraph.vertex[previd].flownumber;//业务流个数
	//cout<<pregraph.vertex[previd].link.predev<<" "<<pregraph.vertex[previd].link.postdev<<endl;
	//system("pause");

	int j = 0;
	//计算可调度的业务流fid在previd标识的网络链路上的发送时间点
	int accdelay;
	int sfdelay = network.devices[MAPNUM[pregraph.vertex[previd].link.predev]].sfdelay;//最小转发时延
	//cout<<"sfdelay:"<<sfdelay<<endl;
	int isfeasible = 1;
	if (index == 1) {
		accdelay = 0;
	}
	else {
		for (int k = 1; k <= NumPre; k++) {
			if (pregraph.vertex[k].link.linkid == flowset.flows[fid].route_change[index - 1].linkid)
				for (int p = 1; p <= pregraph.vertex[k].flownumber; p++) {
					if (pregraph.vertex[k].flowurgency[p].fid == fid && pregraph.vertex[k].flowurgency[p].isfeasible != 0) {
						accdelay = pregraph.vertex[k].flowurgency[p].offset;//业务流到达节点v_k的时间
						break;
					}
					else if (pregraph.vertex[k].flowurgency[p].fid == fid && pregraph.vertex[k].flowurgency[p].isfeasible == 0) {
						accdelay = pregraph.vertex[k].flowurgency[p].offset;//业务流到达节点v_k的时间
						isfeasible = 0;
						break;
					}
				}
		}
	}
	int offset;
	if (accdelay == MAX) {
		offset = MAX;
	}
	else
		offset = sfdelay + accdelay;

	//cout<<"offset:"<<offset<<endl;
	//system("pause");

	// cout<<"j:"<<j<<endl;
	// cout<<"pregraph.vertex[previd].flownumber:"<<pregraph.vertex[previd].flownumber<<endl;
	for (int p = 1; p <= pregraph.vertex[previd].flownumber; p++) {
		// cout<<"p:"<<p<<endl;
		// cout<<"fid:"<<fid<<endl;
		// cout<<"pregraph.vertex[previd].flowurgency[p].fid:"<<pregraph.vertex[previd].flowurgency[p].fid<<endl;
		if (pregraph.vertex[previd].flowurgency[p].fid == fid && isfeasible == 1 && offset < flowset.flows[fid].etedelay) {
			pregraph.vertex[previd].flowurgency[p].offset = offset;
			pregraph.vertex[previd].flowurgency[p].isscheduled = true;
			pregraph.vertex[previd].flowurgency[p].isfeasible = true;
			pregraph.vertex[previd].flowurgency[p].isbreakloop = false;
			j = p;
			break;
		}
		else if (pregraph.vertex[previd].flowurgency[p].fid == fid && isfeasible == 0) {
			pregraph.vertex[previd].flowurgency[p].offset = MAX;
			pregraph.vertex[previd].flowurgency[p].isscheduled = true;
			pregraph.vertex[previd].flowurgency[p].isfeasible = false;
			pregraph.vertex[previd].flowurgency[p].isbreakloop = false;
			//cout<<"isfeasible == 0"<<endl;
			//system("pause");
			return 1;
		}
		else if (pregraph.vertex[previd].flowurgency[p].fid == fid && isfeasible == 1 && offset >= flowset.flows[fid].etedelay) {
			pregraph.vertex[previd].flowurgency[p].offset = MAX;
			pregraph.vertex[previd].flowurgency[p].isscheduled = true;
			pregraph.vertex[previd].flowurgency[p].isfeasible = false;
			pregraph.vertex[previd].flowurgency[p].isbreakloop = false;
			isfeasible = 0;
			//cout << isfeasible << endl;
			//cout<<"offset >= flowset.flows[fid].etedelay"<<endl;
			//system("pause");
			return 1;
		}
		//system("pause");
	}
	int flag = 1;
	int number = 0;
	while (flag == 1 && isfeasible == 1) {
		for (int i = 1; i <= flownumber; i++) {
			if (j == i) continue;
			// cout<<"j:"<<j<<endl;
			// cout<<"i:"<<i<<endl;
			//如果是还没调度的业务流，或者用于破环的业务流，或者不可调度的业务流，不必比较
			if (!pregraph.vertex[previd].flowurgency[i].isscheduled
				|| pregraph.vertex[previd].flowurgency[i].isbreakloop
				|| !pregraph.vertex[previd].flowurgency[i].isfeasible)continue;

			if (isconflict(i, j, previd)) {
				int fidj = pregraph.vertex[previd].flowurgency[j].fid;
				int periodj = flowset.flows[fidj].period;

				int chat = 0;
				while (pregraph.vertex[previd].flowurgency[j].offset < flowset.flows[fid].offsetneed) {
					pregraph.vertex[previd].flowurgency[j].offset++;
					chat = 1;
				}
				if (chat == 0)
					pregraph.vertex[previd].flowurgency[j].offset++;

				number++;
				pregraph.vertex[previd].flowurgency[j].offsetdelay++;

				if (pregraph.vertex[previd].flowurgency[j].offset >= flowset.flows[fid].etedelay) {
					pregraph.vertex[previd].flowurgency[j].offset = MAX;
					pregraph.vertex[previd].flowurgency[j].isscheduled = true;//true:已经被调度；false:尚未被调度
					pregraph.vertex[previd].flowurgency[j].isfeasible = false;//true:可行的调度；false:无法被调度
					flag = 0;
					unschedulenum++;
					ofstream dataFile;
					dataFile.open("output\\urgency_optimization.txt", ofstream::app);
					// 朝TXT文档中写入数据
					dataFile << "fid:" << pregraph.vertex[previd].flowurgency[j].fid << endl;
					dataFile << "StackschedulingByUrgency:offset>etedelay" << endl;
					dataFile << "periodj:" << periodj << endl;
					dataFile << "number:" << number << endl;
					dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
					dataFile << "===================================" << endl;
					// 关闭文档
					dataFile.close();

					//cout<<"StackschedulingByUrgency:offset>etedelay"<<endl;
					//system("pause");
					return 1;
				}
				//当f_i^([v_k,v_j]).offset遍历完f_i一个周期中的所有时间槽位后，记录该业务流不可调度
				else if (number > periodj) {
					//更新业务流在链路中的调度信息
					pregraph.vertex[previd].flowurgency[j].offset = MAX;
					pregraph.vertex[previd].flowurgency[j].isscheduled = true;//true:已经被调度；false:尚未被调度
					pregraph.vertex[previd].flowurgency[j].isfeasible = false;//true:可行的调度；false:无法被调度
					flag = 0;
					unschedulenum++;
					ofstream dataFile;
					dataFile.open("output\\urgency_optimization.txt", ofstream::app);
					// 朝TXT文档中写入数据
					dataFile << "fid:" << pregraph.vertex[previd].flowurgency[j].fid << endl;
					dataFile << "StackschedulingByUrgency:number>periodj" << endl;
					dataFile << "periodj:" << periodj << endl;
					dataFile << "number:" << number << endl;
					dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
					dataFile << "===================================" << endl;
					// 关闭文档
					dataFile.close();
					//cout<<"StackschedulingByUrgency:number>periodj"<<endl;
					//system("pause");

					return 1;
				}
			}
		}
		int num = 0;
		//cout<<"++++++++++++j:"<<j<<endl;
		for (int k = 1; k <= flownumber; k++) {
			if (j == k) continue;
			//如果是还没调度的业务流，或者用于破环的业务流，或者不可调度的业务流，不必比较
			if (!pregraph.vertex[previd].flowurgency[k].isscheduled
				|| pregraph.vertex[previd].flowurgency[k].isbreakloop
				|| !pregraph.vertex[previd].flowurgency[k].isfeasible) {
				continue;
			}
			if (isconflict(j, k, previd)) {
				num++;
				break;
			}
		}
		if (num == 0) {
			flag = 0;
		}
		//cout<<"==========j:"<<j<<endl;

	}
	//cout<<"=========end=================="<<endl;
	//cout<<"offset:"<<pregraph.vertex[previd].flowurgency[j].offset<<endl;
	//system("pause");
	return 0;
}
int StackschedulingByUrgency_optimization_RC(int previd, int fid, int index, int length) {
	int flownumber = pregraph.vertex[previd].flownumber;//业务流个数
	//cout<<"fid:?????"<<fid<<endl;
	//cout<<pregraph.vertex[previd].link.predev<<" "<<pregraph.vertex[previd].link.postdev<<endl;
	//system("pause");

	int j = 0;
	//计算可调度的业务流fid在previd标识的网络链路上的发送时间点
	int accdelay;
	int sfdelay = network.devices[MAPNUM[pregraph.vertex[previd].link.predev]].sfdelay;//最小转发时延
	//cout<<"sfdelay:"<<sfdelay<<endl;
	int isfeasible = 1;
	if (index == 1) {
		accdelay = 0;
		//system("pause");
	}
	else {
		for (int k = 1; k <= NumPre; k++) {
			if (pregraph.vertex[k].link.linkid == flowset.flows[fid].route[index - 1].linkid)
				for (int p = 1; p <= pregraph.vertex[k].flownumber; p++) {
					if (pregraph.vertex[k].flowurgency[p].fid == fid && pregraph.vertex[k].flowurgency[p].isfeasible != 0) {
						accdelay = pregraph.vertex[k].flowurgency[p].offset;//业务流到达节点v_k的时间

						break;
					}
					else if (pregraph.vertex[k].flowurgency[p].fid == fid && pregraph.vertex[k].flowurgency[p].isfeasible == 0) {
						accdelay = pregraph.vertex[k].flowurgency[p].offset;//业务流到达节点v_k的时间						
						isfeasible = 0;

						break;
					}
				}
		}
	}

	int offset;
	if (accdelay == MAX) {
		offset = MAX;
	}
	else
		offset = sfdelay + accdelay;

	//cout<<"offset:"<<offset<<endl;
	//system("pause");

	//cout<<"j:"<<j<<endl;
	//cout<<"pregraph.vertex[previd].flownumber:"<<pregraph.vertex[previd].flownumber<<endl;
	for (int p = 1; p <= pregraph.vertex[previd].flownumber; p++) {
		//cout<<"p:"<<p<<endl;
		//cout<<"fid:"<<fid<<endl;
		//cout<<"pregraph.vertex[previd].flowurgency[p].fid:"<<pregraph.vertex[previd].flowurgency[p].fid<<endl;

		if (pregraph.vertex[previd].flowurgency[p].fid == fid && isfeasible == 1 && offset < flowset.flows[fid].etedelay) {
			pregraph.vertex[previd].flowurgency[p].offset = offset;
			pregraph.vertex[previd].flowurgency[p].isscheduled = true;
			pregraph.vertex[previd].flowurgency[p].isfeasible = true;
			pregraph.vertex[previd].flowurgency[p].isbreakloop = false;
			j = p;
			break;
		}
		else if (pregraph.vertex[previd].flowurgency[p].fid == fid && isfeasible == 0) {
			pregraph.vertex[previd].flowurgency[p].offset = MAX;
			pregraph.vertex[previd].flowurgency[p].isscheduled = true;
			pregraph.vertex[previd].flowurgency[p].isfeasible = false;
			pregraph.vertex[previd].flowurgency[p].isbreakloop = false;
			//cout<<"isfeasible == 0"<<endl;
			//system("pause");
			return 1;
		}
		else if (pregraph.vertex[previd].flowurgency[p].fid == fid && isfeasible == 1 && offset >= flowset.flows[fid].etedelay) {
			pregraph.vertex[previd].flowurgency[p].offset = MAX;
			pregraph.vertex[previd].flowurgency[p].isscheduled = true;
			pregraph.vertex[previd].flowurgency[p].isfeasible = false;
			pregraph.vertex[previd].flowurgency[p].isbreakloop = false;
			isfeasible = 0;
			//cout << isfeasible << endl;
			//cout<<"offset >= flowset.flows[fid].etedelay"<<endl;
			//system("pause");
			return 1;
		}
		//system("pause");
	}
	int flag = 1;
	int number = 0;
	while (flag == 1 && isfeasible == 1) {
		for (int i = 1; i <= flownumber; i++) {
			if (j == i) continue;
			//cout<<"j:"<<j<<endl;
			//cout<<"i:"<<i<<endl;
		   //如果是还没调度的业务流，或者用于破环的业务流，或者不可调度的业务流，不必比较
			if (!pregraph.vertex[previd].flowurgency[i].isscheduled
				|| pregraph.vertex[previd].flowurgency[i].isbreakloop
				|| !pregraph.vertex[previd].flowurgency[i].isfeasible)continue;

			if (isconflict(i, j, previd)) {
				int fidj = pregraph.vertex[previd].flowurgency[j].fid;
				int periodj = flowset.flows[fidj].period;

				int chat = 0;
				while (pregraph.vertex[previd].flowurgency[j].offset < flowset.flows[fid].offsetneed) {
					pregraph.vertex[previd].flowurgency[j].offset++;
					chat = 1;
				}
				if (chat == 0)
					pregraph.vertex[previd].flowurgency[j].offset = pregraph.vertex[previd].flowurgency[j].offset + length;

				number = number + length;
				pregraph.vertex[previd].flowurgency[j].offsetdelay++;
				if (pregraph.vertex[previd].flowurgency[j].offset >= flowset.flows[fid].etedelay) {
					pregraph.vertex[previd].flowurgency[j].offset = MAX;
					pregraph.vertex[previd].flowurgency[j].isscheduled = true;//true:已经被调度；false:尚未被调度
					pregraph.vertex[previd].flowurgency[j].isfeasible = false;//true:可行的调度；false:无法被调度
					flag = 0;
					unschedulenum++;
					ofstream dataFile;
					dataFile.open("output\\urgency_optimization.txt", ofstream::app);
					// 朝TXT文档中写入数据
					dataFile << "fid:" << pregraph.vertex[previd].flowurgency[j].fid << endl;
					dataFile << "StackschedulingByUrgency:offset>etedelay" << endl;
					dataFile << "periodj:" << periodj << endl;
					dataFile << "number:" << number << endl;
					dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
					dataFile << "===================================" << endl;
					// 关闭文档
					dataFile.close();

					//cout<<"StackschedulingByUrgency:offset>etedelay"<<endl;
					//system("pause");
					return 1;
				}
				//当f_i^([v_k,v_j]).offset遍历完f_i一个周期中的所有时间槽位后，记录该业务流不可调度
				else if (number > periodj) {
					//更新业务流在链路中的调度信息
					pregraph.vertex[previd].flowurgency[j].offset = MAX;
					pregraph.vertex[previd].flowurgency[j].isscheduled = true;//true:已经被调度；false:尚未被调度
					pregraph.vertex[previd].flowurgency[j].isfeasible = false;//true:可行的调度；false:无法被调度
					flag = 0;
					unschedulenum++;
					ofstream dataFile;
					dataFile.open("output\\urgency_optimization.txt", ofstream::app);
					// 朝TXT文档中写入数据
					dataFile << "fid:" << pregraph.vertex[previd].flowurgency[j].fid << endl;
					dataFile << "StackschedulingByUrgency:number>periodj" << endl;
					dataFile << "periodj:" << periodj << endl;
					dataFile << "number:" << number << endl;
					dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
					dataFile << "===================================" << endl;
					// 关闭文档
					dataFile.close();
					//cout<<"StackschedulingByUrgency:number>periodj"<<endl;
					//system("pause");

					return 1;
				}
			}
		}
		int num = 0;
		//cout<<"++++++++++++j:"<<j<<endl;
		for (int k = 1; k <= flownumber; k++) {
			if (j == k) continue;
			//如果是还没调度的业务流，或者用于破环的业务流，或者不可调度的业务流，不必比较
			if (!pregraph.vertex[previd].flowurgency[k].isscheduled
				|| pregraph.vertex[previd].flowurgency[k].isbreakloop
				|| !pregraph.vertex[previd].flowurgency[k].isfeasible) {
				continue;
			}
			if (isconflict(j, k, previd)) {
				num++;
				break;
			}
		}
		if (num == 0) {
			flag = 0;
		}
		//cout<<"==========j:"<<j<<endl;

	}
	//cout<<"=========end=================="<<endl;
	//cout<<"offset:"<<pregraph.vertex[previd].flowurgency[j].offset<<endl;
	//system("pause");
	return 0;
}
//调度破环的业务流
int StackschedulingByUrgency_optimization_RCflow(int previd, int fid, int index) {
	int flownumber = pregraph.vertex[previd].flownumber;//业务流个数
	//cout<<pregraph.vertex[previd].link.predev<<" "<<pregraph.vertex[previd].link.postdev<<endl;

	//system("pause");


	int j = 0;
	//计算可调度的业务流fid在previd标识的网络链路上的发送时间点
	int accdelay;
	int sfdelay = network.devices[MAPNUM[pregraph.vertex[previd].link.predev]].sfdelay;//最小转发时延
	//cout<<"sfdelay:"<<sfdelay<<endl;
	int isfeasible = 1;
	if (index == 1) {
		accdelay = 0;
	}
	else {
		for (int k = 1; k <= NumPre; k++) {
			if (pregraph.vertex[k].link.linkid == flowset.flows[fid].route[index - 1].linkid)
				for (int p = 1; p <= pregraph.vertex[k].flownumber; p++) {
					if (pregraph.vertex[k].flowurgency[p].fid == fid && pregraph.vertex[k].flowurgency[p].isfeasible != 0) {
						accdelay = pregraph.vertex[k].flowurgency[p].offset;//业务流到达节点v_k的时间
						break;
					}
					else if (pregraph.vertex[k].flowurgency[p].fid == fid && pregraph.vertex[k].flowurgency[p].isfeasible == 0) {
						accdelay = pregraph.vertex[k].flowurgency[p].offset;//业务流到达节点v_k的时间
						isfeasible = 0;
						break;
					}
				}
		}
	}
	int offset;
	if (accdelay == MAX) {
		offset = MAX;
	}
	else
		offset = sfdelay + accdelay;
	//cout<<"offset:"<<offset<<endl;
	//system("pause");

	for (int p = 1; p <= pregraph.vertex[previd].flownumber; p++) {

		if (pregraph.vertex[previd].flowurgency[p].fid == fid && isfeasible == 1 && offset < flowset.flows[fid].etedelay) {
			pregraph.vertex[previd].flowurgency[p].offset = offset;
			pregraph.vertex[previd].flowurgency[p].isscheduled = true;
			pregraph.vertex[previd].flowurgency[p].isfeasible = true;
			pregraph.vertex[previd].flowurgency[p].isbreakloop = false;
			j = p;
			break;
		}
		else if (pregraph.vertex[previd].flowurgency[p].fid == fid && isfeasible == 0) {
			pregraph.vertex[previd].flowurgency[p].offset = MAX;
			pregraph.vertex[previd].flowurgency[p].isscheduled = true;
			pregraph.vertex[previd].flowurgency[p].isfeasible = false;
			pregraph.vertex[previd].flowurgency[p].isbreakloop = false;
			//cout<<"isfeasible == 0"<<endl;
			//system("pause");

			return 1;
		}
		else if (pregraph.vertex[previd].flowurgency[p].fid == fid && isfeasible == 1 && offset >= flowset.flows[fid].etedelay) {
			pregraph.vertex[previd].flowurgency[p].offset = MAX;
			pregraph.vertex[previd].flowurgency[p].isscheduled = true;
			pregraph.vertex[previd].flowurgency[p].isfeasible = false;
			pregraph.vertex[previd].flowurgency[p].isbreakloop = false;
			isfeasible = 0;
			//cout << isfeasible << endl;
			//cout<<"offset >= flowset.flows[fid].etedelay"<<endl;
			//system("pause");
			return 1;
		}
		//system("pause");
	}

	int flag = 1;
	int number = 0;
	while (flag == 1 && isfeasible == 1) {
		for (int i = 1; i <= flownumber; i++) {
			if (j == i) continue;
			// cout<<"j:"<<j<<endl;
			// cout<<"i:"<<i<<endl;
			//如果是还没调度的业务流，或者用于破环的业务流，或者不可调度的业务流，不必比较
			if (!pregraph.vertex[previd].flowurgency[i].isscheduled
				|| pregraph.vertex[previd].flowurgency[i].isbreakloop
				|| !pregraph.vertex[previd].flowurgency[i].isfeasible)continue;

			if (isconflict(i, j, previd)) {
				int fidj = pregraph.vertex[previd].flowurgency[j].fid;
				int periodj = flowset.flows[fidj].period;

				int chat = 0;
				while (pregraph.vertex[previd].flowurgency[j].offset < flowset.flows[fid].offsetneed) {
					pregraph.vertex[previd].flowurgency[j].offset++;
					chat = 1;
				}
				if (chat == 0)
					pregraph.vertex[previd].flowurgency[j].offset++;

				number++;
				pregraph.vertex[previd].flowurgency[j].offsetdelay++;
				if (pregraph.vertex[previd].flowurgency[j].offset >= flowset.flows[fid].etedelay) {
					pregraph.vertex[previd].flowurgency[j].offset = MAX;
					pregraph.vertex[previd].flowurgency[j].isscheduled = true;//true:已经被调度；false:尚未被调度
					pregraph.vertex[previd].flowurgency[j].isfeasible = false;//true:可行的调度；false:无法被调度
					flag = 0;
					// unschedulenum++;
					// ofstream dataFile;
					// dataFile.open("output\\urgency_optimization.txt", ofstream::app);
					// // 朝TXT文档中写入数据
					// dataFile << "fid:" << pregraph.vertex[previd].flowurgency[j].fid << endl;
					// dataFile << "StackschedulingByUrgency:offset>etedelay" << endl;
					// dataFile << "periodj:"<<periodj << endl;
					// dataFile << "number:"<<number << endl;
					// dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
					// dataFile << "===================================" << endl;
					// // 关闭文档
					// dataFile.close();
					//cout<<"StackschedulingByUrgency:offset>etedelay"<<endl;
					//system("pause");
					return 1;
				}
				//当f_i^([v_k,v_j]).offset遍历完f_i一个周期中的所有时间槽位后，记录该业务流不可调度
				else if (number > periodj) {
					//更新业务流在链路中的调度信息
					pregraph.vertex[previd].flowurgency[j].offset = MAX;
					pregraph.vertex[previd].flowurgency[j].isscheduled = true;//true:已经被调度；false:尚未被调度
					pregraph.vertex[previd].flowurgency[j].isfeasible = false;//true:可行的调度；false:无法被调度
					flag = 0;
					// unschedulenum++;		
					// ofstream dataFile;
					// dataFile.open("output\\urgency_optimization.txt", ofstream::app);
					// // 朝TXT文档中写入数据
					// dataFile << "fid:" << pregraph.vertex[previd].flowurgency[j].fid << endl;
					// dataFile << "StackschedulingByUrgency:number>periodj" << endl;
					// dataFile << "periodj:" << periodj << endl;
					// dataFile << "number:" << number << endl;
					// dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
					// dataFile << "===================================" << endl;
					// // 关闭文档
					// dataFile.close();
					//cout<<"StackschedulingByUrgency:number>periodj"<<endl;
					//system("pause");

					return 1;
				}
			}
		}
		int num = 0;
		//cout<<"++++++++++++j:"<<j<<endl;
		for (int k = 1; k <= flownumber; k++) {
			if (j == k) continue;
			//如果是还没调度的业务流，或者用于破环的业务流，或者不可调度的业务流，不必比较
			if (!pregraph.vertex[previd].flowurgency[k].isscheduled
				|| pregraph.vertex[previd].flowurgency[k].isbreakloop
				|| !pregraph.vertex[previd].flowurgency[k].isfeasible) {
				continue;
			}
			if (isconflict(j, k, previd)) {
				num++;
				break;
			}
		}
		if (num == 0) {
			flag = 0;
		}
	}
	//cout<<"=========end=================="<<endl;
	//cout<<"offset:"<<pregraph.vertex[previd].flowurgency[j].offset<<endl;
	//system("pause");

	return 0;
}
int main() {
	string folderPath = ".\\output";

	if (0 != _access(folderPath.c_str(), 0))
	{
		// if this folder not exist, create a new one.
		_mkdir(folderPath.c_str());   // 返回 0 表示创建成功，-1 表示失败
	}
	clock_t start, end;//定义clock_t变量
	start = clock();//开始时间

	//初始化网络信息，包括网络参数、设备参数、网络拓扑等
	initializeNetwork(network);

	//初始化业务流信息，包括业务流的参数和路由信息
	initializeFlows(flowset);

	//创建先序关系图
	createPreGraph(flowset);

	//基于拓扑序列的业务流调度
	int istrue_RC = topologyBasedScheduling(network, flowset, pregraph);


	cout << istrue_RC << endl;
	//system("pause");
	if (istrue_RC == 0) return 0;

	/*
		输出数据到txt中
	*/
	ofstream dataFile;
	dataFile.open("output\\stack.txt", ofstream::app);
	// 朝TXT文档中写入数据
	end = clock();//结束时间

	dataFile << " time:" << double(end - start) / CLOCKS_PER_SEC << endl;
	dataFile << " 无法调度的TT业务流个数:" << unschedulenum << endl;
	// 关闭文档
	dataFile.close();

	dataFile.open("output\\print.txt", ofstream::out);
	dataFile << "======================print=================================" << endl;

	int number = flowset.fnumber + flowset.fnumberRC;

	for (int i = 1; i <= number; i++) {
		//找出每一条业务流的待调度节点
		for (int j = 1; j <= flowset.flows[i].linknumber; j++) {
			for (int k = 1; k <= NumPre; k++) {
				if (pregraph.vertex[k].link.linkid == flowset.flows[i].route[j].linkid) {
					for (int p = 1; p <= pregraph.vertex[k].flownumber; p++) {
						if (pregraph.vertex[k].flowurgency[p].fid == flowset.flows[i].fid) {
							dataFile << "fid: " << pregraph.vertex[k].flowurgency[p].fid << endl;
							dataFile << pregraph.vertex[k].link.predev << "," << pregraph.vertex[k].link.postdev << endl;
							dataFile << "isscheduled: " << pregraph.vertex[k].flowurgency[p].isscheduled << endl;
							dataFile << "isfeasible: " << pregraph.vertex[k].flowurgency[p].isfeasible << endl;
							dataFile << "isbreakloop: " << pregraph.vertex[k].flowurgency[p].isbreakloop << endl;
							dataFile << "====================" << endl;
						}
					}
				}
			}
		}
	}
	dataFile.close();

	dataFile.open("output\\linkresult.txt", ofstream::out);
	dataFile << "============每一个结点上的offset================" << endl;
	for (int k = 1; k <= NumPre; k++) {
		dataFile << pregraph.vertex[k].link.predev << "," << pregraph.vertex[k].link.postdev << endl;
		for (int i = 1; i <= pregraph.vertex[k].flownumber; i++) {
			dataFile << "fid: " << pregraph.vertex[k].flowurgency[i].fid << endl;
			dataFile << "offset: " << pregraph.vertex[k].flowurgency[i].offset << endl;
		}
		dataFile << "==================================" << endl;
	}
	dataFile.close();
	dataFile.open("output\\offsetresult.txt", ofstream::out);

	//每一条数据流在每一个设备上的offset
	dataFile << "============每一条数据流在每一个设备上的offset================" << endl;
	for (int i = 1; i <= number; i++) {
		dataFile << "fid:" << flowset.flows[i].fid << endl;
		for (int j = 1; j <= flowset.flows[i].linknumber; j++) {
			for (int k = 1; k <= NumPre; k++) {
				if (pregraph.vertex[k].link.linkid == flowset.flows[i].route[j].linkid) {
					for (int p = 1; p <= pregraph.vertex[k].flownumber; p++) {
						if (pregraph.vertex[k].flowurgency[p].fid == flowset.flows[i].fid) {
							dataFile << pregraph.vertex[k].link.predev << "," << pregraph.vertex[k].link.postdev;
							dataFile << ":" << pregraph.vertex[k].flowurgency[p].offset << endl;
						}
					}
				}
			}
		}
		dataFile << "===========================" << endl;
	}
	dataFile.close();
	dataFile.open("output\\stack.txt", ofstream::app);
	// 朝TXT文档中写入数据
	// dataFile << " 无法调度的RC业务流个数:" << unschedulenumRC << endl;
	dataFile << "==============================" << endl;
	// 关闭文档
	dataFile.close();
	//在ILP调度器中检查offset是否正确，转换为ILP调度器的输出格式
	detect();

	int unnumTT = 0;//无法调度的TT业务流数量	
	for (int i = 1; i <= flowset.fnumber; i++) {
		//dataFile << "fid:" << flowset.flows[i].fid << endl;
		for (int j = 1; j <= flowset.flows[i].linknumber; j++) {
			int flag2 = 0;
			for (int k = 1; k <= NumPre; k++) {
				int flag1 = 0;
				if (pregraph.vertex[k].link.linkid == flowset.flows[i].route[j].linkid) {
					for (int p = 1; p <= pregraph.vertex[k].flownumber; p++) {
						if (pregraph.vertex[k].flowurgency[p].fid == flowset.flows[i].fid) {
							if (pregraph.vertex[k].flowurgency[p].offset == MAX) {
								cout << flowset.flows[i].fid << " ";
								unnumTT++;
								flag1 = 1;
								break;
							}
						}
					}
				}
				if (flag1 == 1) {
					flag2 = 1;
					break;
				}
			}
			if (flag2 == 1) {
				break;
			}
		}
		//cout<<"==========================="<<endl;
	}
	//system("pause");

	cout << "unschedulenum: " << unschedulenum << endl;


	//==================================================================================================================



	end = clock();//结束时间
	cout << "time = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;//输出时间（单位：ｓ）
	system("pause");

	return 0;
}