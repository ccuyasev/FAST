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
class EdgeNode { //ͼ���ڽӱ����洢�����Ǳ߱�ڵ�
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
	int weight; //ԭֵ�����ı�
	int weight2; //�����޸���ֵ������������󣩲�����
	EdgeNode* next;
};
class VertexNode {//�����ڵ�
public:
	VertexNode(int name = 'Z', EdgeNode* firstEdge = NULL) :name(name), firstEdge(firstEdge) {}
	int name;
	EdgeNode* firstEdge;
	~VertexNode() {}
};
VertexNode vn[MAXNODE];//ȫ�����ݽṹ
using namespace std;
int createGraph(int n, int m, int flag) { //����ͼ
	int from, to;
	int weight;
	for (int i = 1; i <= network.linknumber; i++) {
		//cin>>from>>to>>weight;
		from = link[i].predev;
		to = link[i].postdev;
		weight = 1;
		//���������ͼ
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
		//����ͼ
		else {
			if (vn[from].name == 'Z') vn[from].name = from;
			if (vn[to].name == 'Z') vn[to].name = to;//�����û�г��ߣ������Ҫ������������
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
pair<int, vector<int>> dijkstra(int from, int to, int* t) { //�����·��
	long max = 1e8;  //������ֵ
	int distance = 0;
	int parent[MAXNODE];//����ʱ��¼ÿ���ڵ�ĸ��ڵ㣬�����ҵ����·����ֻ�����������£��˽ڵ�ĸ��ڵ�Ż����
	for (int i = 0; i < MAXNODE; i++)
		parent[i] = 'Z';//��ʼ�����нڵ㸸�ڵ�Ϊ'Z'
	vector<int>vec;//�洢����T�е�ÿ���ڵ㣬�����·����
	vector<pair<int, int>>queue;//�������T����δ��չ�ߵļ��ϣ���vector ʵ��
	for (int i = 0; i < MAXNODE; i++) {
		if (vn[i].name != 'Z') {
			if (vn[i].name == from && t[vn[i].name] == 0)//�ų��㲻�����
				queue.push_back(make_pair(0, vn[i].name));//���ľ���ֵΪ0
			else if (t[vn[i].name] == 0)
				queue.push_back(make_pair(max, vn[i].name));//��ʼ��������ľ���Ϊmax
		}
	}
	vector<pair<int, int>>::iterator it = queue.begin();
	while (!queue.empty()) {
		sort(queue.begin(), queue.end());//������ֵ��С��������
		vector<pair<int, int>>::iterator tmp = queue.begin();
		if (queue.begin()->second == to) {
			distance = queue.begin()->first;//�������ڵ����յ�ʱ�������յ����̾���
			if (distance >= 1e8) {//�յ��·��ֵ���ڵ�������󣬴����·��������       
				distance = -1; //��ʾ�Ҳ���·�������þ���ֵΪ��ֵ
				break;
			}
		}
		EdgeNode* p = vn[tmp->second].firstEdge;
		int d = tmp->first;//�洢�˽ڵ���ӵľ���ֵ
		while (p != NULL) {
			for (it = queue.begin() + 1; it != queue.end(); it++) {
				if (p->to == it->second) {//��T���ҵ�Ҫ���µĵ�			
					if (d + p->weight2 < it->first) { //��Ҫ����,��ȵĻ����ٸ���
						it->first = d + p->weight2;
						parent[it->second] = tmp->second; //���¸��ڵ�
					}
					break;
				}
			}
			p = p->next;
		}
		queue.erase(queue.begin());//ɾ���׽ڵ�
	}

	int c = to;
	int i = 1;
	while (c != from && c != 'Z') {
		if (i > MAXNODE)//�������ڵ������˳�ѭ��
			break;
		vec.push_back(c);
		c = parent[c];
		i++;
	}
	if (c == from)
		vec.push_back(c);//vector �ǰ�������洢��ÿ���ڵ�
	return make_pair(distance, vec);
}
void reset() {//�޸ĺ󣬽�ͼ��λ
	for (int i = 0; i < MAXNODE; i++) {
		EdgeNode* p = vn[i].firstEdge;
		while (p != NULL) {
			p->weight2 = p->weight;
			p = p->next;
		}
	}
}

inline bool setinfinite(int start, int end) //���������
{
	int result = 0;
	EdgeNode* p = vn[start].firstEdge;//��vn���޸�
	while (p != NULL) {
		if (p->to == end) {
			p->weight2 = INFINITE;//��������weight2
			result = 1;
		}
		p = p->next;
	}
	p = vn[end].firstEdge;//����ͼ,Ҳ����������ͼ
	while (p != NULL) {
		if (p->to == start) {
			p->weight2 = INFINITE;
			result = 1;
		}
		p = p->next;
	}
	return result;
}

bool search(int node, vector<pair<int, vector<int>>>ListA) { //��ListA�н��ڵ�node��ʼ�����б���Ϊinfinite

	for (vector<pair<int, vector<int>>>::iterator it = ListA.begin(); it != ListA.end(); it++) {
		for (vector<int>::iterator it2 = it->second.end() - 1; it2 > it->second.begin(); it2--) { //����˳��������������	
			if (*it2 == node) {
				vector<int>::iterator it3 = it2 - 1;
				if (!setinfinite(*it2, *it3))
					return 0;
			}
		}
	}
	return 1;
}

bool exist(vector<pair<int, vector<int>>>List, vector<int>vec) {//�������Ƿ���ListB�г��ֹ�

	for (vector<pair<int, vector<int>>>::iterator it = List.begin(); it != List.end(); it++) {
		if (it->second == vec)
			return 1;
	}
	return 0;
}
void print(vector<pair<int, vector<int>>>List) { //��ӡListA��ListB��״̬

	//if(!List.empty())
	//for(vector<pair<int,vector<int>>>::iterator it = List.begin();it != List.end();it++){
	//    for(vector<int>::iterator it2 = it->second.end()-1;it2>=it->second.begin();it2--)
	   //     cout<<*it2<<"  ";
		//cout<<it->first<<endl;
   // }
}
//�����ƻ���ҵ����
int StackschedulingByUrgency_optimization(int previd, int fid, int index) {
	int flownumber = pregraph.vertex[previd].flownumber;//ҵ��������
	//cout<<pregraph.vertex[previd].link.predev<<" "<<pregraph.vertex[previd].link.postdev<<endl;
	//system("pause");

	int j = 0;
	//����ɵ��ȵ�ҵ����fid��previd��ʶ��������·�ϵķ���ʱ���
	int accdelay;
	int sfdelay = network.devices[MAPNUM[pregraph.vertex[previd].link.predev]].sfdelay;//��Сת��ʱ��
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
						accdelay = pregraph.vertex[k].flowurgency[p].offset;//ҵ��������ڵ�v_k��ʱ��
						break;
					}
					else if (pregraph.vertex[k].flowurgency[p].fid == fid && pregraph.vertex[k].flowurgency[p].isfeasible == 0) {
						accdelay = pregraph.vertex[k].flowurgency[p].offset;//ҵ��������ڵ�v_k��ʱ��
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
			//����ǻ�û���ȵ�ҵ���������������ƻ���ҵ���������߲��ɵ��ȵ�ҵ���������رȽ�
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
					pregraph.vertex[previd].flowurgency[j].isscheduled = true;//true:�Ѿ������ȣ�false:��δ������
					pregraph.vertex[previd].flowurgency[j].isfeasible = false;//true:���еĵ��ȣ�false:�޷�������
					flag = 0;
					unschedulenum++;
					ofstream dataFile;
					dataFile.open("output\\urgency_optimization.txt", ofstream::app);
					// ��TXT�ĵ���д������
					dataFile << "fid:" << pregraph.vertex[previd].flowurgency[j].fid << endl;
					dataFile << "StackschedulingByUrgency:offset>etedelay" << endl;
					dataFile << "periodj:" << periodj << endl;
					dataFile << "number:" << number << endl;
					dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
					dataFile << "===================================" << endl;
					// �ر��ĵ�
					dataFile.close();

					//cout<<"StackschedulingByUrgency:offset>etedelay"<<endl;
					//system("pause");
					return 1;
				}
				//��f_i^([v_k,v_j]).offset������f_iһ�������е�����ʱ���λ�󣬼�¼��ҵ�������ɵ���
				else if (number > periodj) {
					//����ҵ��������·�еĵ�����Ϣ
					pregraph.vertex[previd].flowurgency[j].offset = MAX;
					pregraph.vertex[previd].flowurgency[j].isscheduled = true;//true:�Ѿ������ȣ�false:��δ������
					pregraph.vertex[previd].flowurgency[j].isfeasible = false;//true:���еĵ��ȣ�false:�޷�������
					flag = 0;
					unschedulenum++;
					ofstream dataFile;
					dataFile.open("output\\urgency_optimization.txt", ofstream::app);
					// ��TXT�ĵ���д������
					dataFile << "fid:" << pregraph.vertex[previd].flowurgency[j].fid << endl;
					dataFile << "StackschedulingByUrgency:number>periodj" << endl;
					dataFile << "periodj:" << periodj << endl;
					dataFile << "number:" << number << endl;
					dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
					dataFile << "===================================" << endl;
					// �ر��ĵ�
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
			//����ǻ�û���ȵ�ҵ���������������ƻ���ҵ���������߲��ɵ��ȵ�ҵ���������رȽ�
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
	int flownumber = pregraph.vertex[previd].flownumber;//ҵ��������
	//cout<<"fid:?????"<<fid<<endl;
	//cout<<pregraph.vertex[previd].link.predev<<" "<<pregraph.vertex[previd].link.postdev<<endl;
	//system("pause");

	int j = 0;
	//����ɵ��ȵ�ҵ����fid��previd��ʶ��������·�ϵķ���ʱ���
	int accdelay;
	int sfdelay = network.devices[MAPNUM[pregraph.vertex[previd].link.predev]].sfdelay;//��Сת��ʱ��
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
						accdelay = pregraph.vertex[k].flowurgency[p].offset;//ҵ��������ڵ�v_k��ʱ��

						break;
					}
					else if (pregraph.vertex[k].flowurgency[p].fid == fid && pregraph.vertex[k].flowurgency[p].isfeasible == 0) {
						accdelay = pregraph.vertex[k].flowurgency[p].offset;//ҵ��������ڵ�v_k��ʱ��						
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
		   //����ǻ�û���ȵ�ҵ���������������ƻ���ҵ���������߲��ɵ��ȵ�ҵ���������رȽ�
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
					pregraph.vertex[previd].flowurgency[j].isscheduled = true;//true:�Ѿ������ȣ�false:��δ������
					pregraph.vertex[previd].flowurgency[j].isfeasible = false;//true:���еĵ��ȣ�false:�޷�������
					flag = 0;
					unschedulenum++;
					ofstream dataFile;
					dataFile.open("output\\urgency_optimization.txt", ofstream::app);
					// ��TXT�ĵ���д������
					dataFile << "fid:" << pregraph.vertex[previd].flowurgency[j].fid << endl;
					dataFile << "StackschedulingByUrgency:offset>etedelay" << endl;
					dataFile << "periodj:" << periodj << endl;
					dataFile << "number:" << number << endl;
					dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
					dataFile << "===================================" << endl;
					// �ر��ĵ�
					dataFile.close();

					//cout<<"StackschedulingByUrgency:offset>etedelay"<<endl;
					//system("pause");
					return 1;
				}
				//��f_i^([v_k,v_j]).offset������f_iһ�������е�����ʱ���λ�󣬼�¼��ҵ�������ɵ���
				else if (number > periodj) {
					//����ҵ��������·�еĵ�����Ϣ
					pregraph.vertex[previd].flowurgency[j].offset = MAX;
					pregraph.vertex[previd].flowurgency[j].isscheduled = true;//true:�Ѿ������ȣ�false:��δ������
					pregraph.vertex[previd].flowurgency[j].isfeasible = false;//true:���еĵ��ȣ�false:�޷�������
					flag = 0;
					unschedulenum++;
					ofstream dataFile;
					dataFile.open("output\\urgency_optimization.txt", ofstream::app);
					// ��TXT�ĵ���д������
					dataFile << "fid:" << pregraph.vertex[previd].flowurgency[j].fid << endl;
					dataFile << "StackschedulingByUrgency:number>periodj" << endl;
					dataFile << "periodj:" << periodj << endl;
					dataFile << "number:" << number << endl;
					dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
					dataFile << "===================================" << endl;
					// �ر��ĵ�
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
			//����ǻ�û���ȵ�ҵ���������������ƻ���ҵ���������߲��ɵ��ȵ�ҵ���������رȽ�
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
//�����ƻ���ҵ����
int StackschedulingByUrgency_optimization_RCflow(int previd, int fid, int index) {
	int flownumber = pregraph.vertex[previd].flownumber;//ҵ��������
	//cout<<pregraph.vertex[previd].link.predev<<" "<<pregraph.vertex[previd].link.postdev<<endl;

	//system("pause");


	int j = 0;
	//����ɵ��ȵ�ҵ����fid��previd��ʶ��������·�ϵķ���ʱ���
	int accdelay;
	int sfdelay = network.devices[MAPNUM[pregraph.vertex[previd].link.predev]].sfdelay;//��Сת��ʱ��
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
						accdelay = pregraph.vertex[k].flowurgency[p].offset;//ҵ��������ڵ�v_k��ʱ��
						break;
					}
					else if (pregraph.vertex[k].flowurgency[p].fid == fid && pregraph.vertex[k].flowurgency[p].isfeasible == 0) {
						accdelay = pregraph.vertex[k].flowurgency[p].offset;//ҵ��������ڵ�v_k��ʱ��
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
			//����ǻ�û���ȵ�ҵ���������������ƻ���ҵ���������߲��ɵ��ȵ�ҵ���������رȽ�
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
					pregraph.vertex[previd].flowurgency[j].isscheduled = true;//true:�Ѿ������ȣ�false:��δ������
					pregraph.vertex[previd].flowurgency[j].isfeasible = false;//true:���еĵ��ȣ�false:�޷�������
					flag = 0;
					// unschedulenum++;
					// ofstream dataFile;
					// dataFile.open("output\\urgency_optimization.txt", ofstream::app);
					// // ��TXT�ĵ���д������
					// dataFile << "fid:" << pregraph.vertex[previd].flowurgency[j].fid << endl;
					// dataFile << "StackschedulingByUrgency:offset>etedelay" << endl;
					// dataFile << "periodj:"<<periodj << endl;
					// dataFile << "number:"<<number << endl;
					// dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
					// dataFile << "===================================" << endl;
					// // �ر��ĵ�
					// dataFile.close();
					//cout<<"StackschedulingByUrgency:offset>etedelay"<<endl;
					//system("pause");
					return 1;
				}
				//��f_i^([v_k,v_j]).offset������f_iһ�������е�����ʱ���λ�󣬼�¼��ҵ�������ɵ���
				else if (number > periodj) {
					//����ҵ��������·�еĵ�����Ϣ
					pregraph.vertex[previd].flowurgency[j].offset = MAX;
					pregraph.vertex[previd].flowurgency[j].isscheduled = true;//true:�Ѿ������ȣ�false:��δ������
					pregraph.vertex[previd].flowurgency[j].isfeasible = false;//true:���еĵ��ȣ�false:�޷�������
					flag = 0;
					// unschedulenum++;		
					// ofstream dataFile;
					// dataFile.open("output\\urgency_optimization.txt", ofstream::app);
					// // ��TXT�ĵ���д������
					// dataFile << "fid:" << pregraph.vertex[previd].flowurgency[j].fid << endl;
					// dataFile << "StackschedulingByUrgency:number>periodj" << endl;
					// dataFile << "periodj:" << periodj << endl;
					// dataFile << "number:" << number << endl;
					// dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
					// dataFile << "===================================" << endl;
					// // �ر��ĵ�
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
			//����ǻ�û���ȵ�ҵ���������������ƻ���ҵ���������߲��ɵ��ȵ�ҵ���������رȽ�
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
		_mkdir(folderPath.c_str());   // ���� 0 ��ʾ�����ɹ���-1 ��ʾʧ��
	}
	clock_t start, end;//����clock_t����
	start = clock();//��ʼʱ��

	//��ʼ��������Ϣ����������������豸�������������˵�
	initializeNetwork(network);

	//��ʼ��ҵ������Ϣ������ҵ�����Ĳ�����·����Ϣ
	initializeFlows(flowset);

	//���������ϵͼ
	createPreGraph(flowset);

	//�����������е�ҵ��������
	int istrue_RC = topologyBasedScheduling(network, flowset, pregraph);


	cout << istrue_RC << endl;
	//system("pause");
	if (istrue_RC == 0) return 0;

	/*
		������ݵ�txt��
	*/
	ofstream dataFile;
	dataFile.open("output\\stack.txt", ofstream::app);
	// ��TXT�ĵ���д������
	end = clock();//����ʱ��

	dataFile << " time:" << double(end - start) / CLOCKS_PER_SEC << endl;
	dataFile << " �޷����ȵ�TTҵ��������:" << unschedulenum << endl;
	// �ر��ĵ�
	dataFile.close();

	dataFile.open("output\\print.txt", ofstream::out);
	dataFile << "======================print=================================" << endl;

	int number = flowset.fnumber + flowset.fnumberRC;

	for (int i = 1; i <= number; i++) {
		//�ҳ�ÿһ��ҵ�����Ĵ����Ƚڵ�
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
	dataFile << "============ÿһ������ϵ�offset================" << endl;
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

	//ÿһ����������ÿһ���豸�ϵ�offset
	dataFile << "============ÿһ����������ÿһ���豸�ϵ�offset================" << endl;
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
	// ��TXT�ĵ���д������
	// dataFile << " �޷����ȵ�RCҵ��������:" << unschedulenumRC << endl;
	dataFile << "==============================" << endl;
	// �ر��ĵ�
	dataFile.close();
	//��ILP�������м��offset�Ƿ���ȷ��ת��ΪILP�������������ʽ
	detect();

	int unnumTT = 0;//�޷����ȵ�TTҵ��������	
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



	end = clock();//����ʱ��
	cout << "time = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;//���ʱ�䣨��λ����
	system("pause");

	return 0;
}