#include <iostream>
#include "Print.h"
using namespace std;
/*
 *��ʼ����������
*/
void initializeNetwork(Network& network){
	FILE* fp;
    fp = fopen("1device.txt", "r");
	if (NULL == fp)  //������������˵���ļ��򿪳ɹ�
        cout << "1device.txt doesn't exist" << endl;

	int timescope, timeaccuracy, timeslot, maxframe, maxline;
	int linerate, devnumber, linknumber;
	fscanf(fp,"%d %d %d %d %d %d %d %d",&timescope,&timeaccuracy,&timeslot,&maxframe,&maxline,&linerate,&devnumber,&linknumber);
	network.timescope = timescope;//ʱ������ 1ns
	network.timeaccuracy = timeaccuracy;//ʱ��ͬ���ľ���1000ns
    network.timeslot = timeslot;//ʱ��� 20000ns 
    network.maxframe = maxframe;//����ĳ��� 1518 Bytes
    network.maxline = maxline;//����߳� 100m
    network.linerate = linerate;//�������� 1000 Mbps
    network.devnumber = devnumber;//�豸����
    network.linknumber = linknumber;//��·����
	network.devices = new Device[1000000];
	int devid, rdelay, fdelay, pdelay, sfdelay;
	bool isterminal;
	for(int i = 1; i <= network.devnumber; i++){
		fscanf(fp,"%d %d %d %d %d %d",&devid,&rdelay,&fdelay,&pdelay,&sfdelay,&isterminal);
		MAPNUM[devid] = i;
		network.devices[i].devid = devid;//�豸���
		network.devices[i].rdelay = rdelay;//����ʱ��
		network.devices[i].fdelay = fdelay;//ת��ʱ��
		network.devices[i].pdelay = pdelay;//����ʱ��
		network.devices[i].sfdelay = sfdelay/network.timeslot + 1;//�洢��store��ת����forward��ʱ��
		network.devices[i].isterminal = isterminal;//true:�նˣ�false:������
		network.devices[i].firstedge = NULL;//�豸����·��ָ��
	}

	int linkid,predev,postdev,ldelay;
	bool isdirect = 0;
	Link *e;
	for(int i = 1;i <= network.linknumber; i++){
		fscanf(fp,"%d %d %d %d",&linkid,&predev,&postdev,&ldelay);
		link[i].linkid = linkid;//��·���
		link[i].isdirect = 1;//true������ߣ�false:�����
		link[i].predev = predev;//ǰ��ڵ�ı��
		link[i].postdev = postdev;//�����ڵ�ı��
		link[i].ldelay = ldelay;//��·ʱ��
		if(network.devices[MAPNUM[predev]].firstedge == NULL){//��ǰ����predev����û�ж���
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
			while(p->link)//β�巨
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
//��ʼ��ҵ����
void initializeFlows(FlowSet& flowset){
	FILE* fp;
	FILE* dp;
    fp = fopen("2flowTT.txt", "r");


	if (NULL == fp)  //������������˵���ļ��򿪳ɹ�
        cout << "2flowTT.txt doesn't exist" << endl;


	dp = fopen("3flowlinkTT.txt", "r");
	if (NULL == dp)  //������������˵���ļ��򿪳ɹ�
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

		flowset.flows[i].fid = fid;//ҵ�������
		flowset.flows[i].length = length;//���ĵĳ���
		flowset.flows[i].period = period/network.timeslot ;//ҵ����������
		flowset.flows[i].etedelay = etedelay/network.timeslot ;//�˵���ʱ��
		flowset.flows[i].rdevnumber = rdevnumber;//�����ն˵�����
		flowset.flows[i].linknumber = linknumber;//·������·������
		flowset.flows[i].schedlinknumber = schedlinknumber;//·���б����ȵ���·����
		if(offsetneed % network.timeslot == 0)
			flowset.flows[i].offsetneed = offsetneed/network.timeslot;//�涨��ʼ�ķ���ʱ��㣬����õ��Ľ���������ڸ�ʱ���
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
			
			flowset.flows[i].route[j].preroute = j-1;//����
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
//���������ϵͼ
void createPreGraph(FlowSet& flowset){
	pregraph.vnumber = NumPre;//����ĸ���
	pregraph.schedfnumber = 0;//�����ȵ�ҵ������������ʼΪ0
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
					//��Сת��ʱ��
					int devid = link[flowset.flows[i].route[j].linkid].predev;
					int mindelay=(network.devices[MAPNUM[devid]].rdelay + network.devices[MAPNUM[devid]].fdelay + network.devices[MAPNUM[devid]].pdelay + 
					link[flowset.flows[i].route[j].linkid].ldelay + network.timeaccuracy)/network.timeslot + 1;
					// cout<<network.devices[MAPNUM[devid]].rdelay<<endl;
					// cout<<network.devices[MAPNUM[devid]].fdelay<<endl;
					// cout<<network.devices[MAPNUM[devid]].pdelay<<endl;
					// cout<<link[flowset.flows[i].route[j].linkid].ldelay<<endl;
					// system("pause");
					//ʣ����С����ʱ��
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
							while(q->edge)//β�巨
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