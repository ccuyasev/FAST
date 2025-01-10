#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <fstream>
#include <time.h>
//������previd��ҵ����i�Ľ����̶�
int computeUrgencyLinkidRC(int previd, int i){

	int fid = pregraph.vertex[previd].flowurgency[i].fid;
    int sfdelay = network.devices[MAPNUM[pregraph.vertex[previd].link.predev]].sfdelay;
	int accdelay = pregraph.vertex[previd].flowurgency[i].accdelay;
	int remainmindelay = pregraph.vertex[previd].flowurgency[i].remainmindelay;
	int urgency = flowset.flows[fid].etedelay - sfdelay - accdelay - remainmindelay;
	if(urgency < 0){//˵����ҵ�����޷�����
		return -1;

	}
	return urgency;
}
//������Ҫ�ƻ���ҵ�����н����̶ȵ���Сֵ����������urgency�����ֵ
int maxindexRC(int *array, int n){
	int max = array[1];
	int index = 1;
	for(int i = 2;i <= n ; i++){
		if(max < array[i]){
			max = array[i];
			index = i;
		}
	}
	return index;
}
//���ҽ����̶ȵ���Сֵ
BreakLoopPoint findLowestUrgencyFlowsRC(){
	//����ÿһ��ҵ����
	int urgency[100001], Linkid[100001];
	int fnumberTT = flowset.fnumber + 1;
	int fnumberRC = flowset.fnumberRC;
	int number = flowset.fnumber + flowset.fnumberRC;
	int u = 1;
	for(int i = fnumberTT;i <= number;i++){
		//�ҳ�ÿһ��RCҵ�����Ĵ����Ƚڵ�
		int flag = 0;	
		for(int j = 1; j <= flowset.flows[i].linknumber; j++){
			for(int k = 1;k <= NumPre; k++){
				if(num[k] == flowset.flows[i].route[j].linkid){
					for(int p = 1;p <= pregraph.vertex[k].flownumber; p++){
						if(pregraph.vertex[k].flowurgency[p].fid == i 
							&& pregraph.vertex[k].flowurgency[p].isscheduled == 0
							&& pregraph.vertex[k].flowurgency[p].isfeasible == 1 
							&& pregraph.vertex[k].flowurgency[p].isbreakloop == 0){
								//��δ�����ȣ����еĵ��ȣ�����ҵ����																
								urgency[u] = computeUrgencyLinkidRC(k,p);
								Linkid[u] = k;
								// cout<<"u:"<<u<<endl;
								// cout<<"urgency[u]:"<<urgency[u]<<endl;
								// cout<<"fid:"<<pregraph.vertex[k].flowurgency[p].fid<<endl;
								u++;
								
								flag = 1;
								break;								

						}
						else if((pregraph.vertex[k].flowurgency[p].fid == i 
							&& pregraph.vertex[k].flowurgency[p].isscheduled == 0
							&& pregraph.vertex[k].flowurgency[p].isfeasible == 1 
							&& pregraph.vertex[k].flowurgency[p].isbreakloop == 1)||//�Ѿ����ƻ���ҵ����

							(pregraph.vertex[k].flowurgency[p].fid == i 
							&& pregraph.vertex[k].flowurgency[p].isfeasible == 0)//���ɵ��ȵ�ҵ����
							){								
								urgency[u] = -1;
								Linkid[u] = k;
								flag = 1;
								// cout<<"u:"<<u<<endl;
								// cout<<"urgency[u]:"<<urgency[u]<<endl;
								// cout<<"fid:"<<pregraph.vertex[k].flowurgency[p].fid<<endl;
								u++;
								break;
						}
						
						else if(pregraph.vertex[k].flowurgency[p].fid == i 
							&& j == flowset.flows[i].linknumber
							&& pregraph.vertex[k].flowurgency[p].isscheduled == 1
							&& pregraph.vertex[k].flowurgency[p].isfeasible == 1 
							&& pregraph.vertex[k].flowurgency[p].isbreakloop == 0){//ȫ�����Ƚ�����ҵ����								
								urgency[u] = -1;
								Linkid[u] = k;
								// cout<<"u:"<<u<<endl;
								// cout<<"urgency[u]:"<<urgency[u]<<endl;
								// cout<<"fid:"<<pregraph.vertex[k].flowurgency[p].fid<<endl;
								u++;
								flag = 1;
								break;
						}
					}
				}
				if(flag == 1) break;
			}
		}
	}

	int Maxindex = maxindexRC(urgency, flowset.fnumberRC);
	BreakLoopPoint blp;
	blp.fid = Maxindex + flowset.fnumber;
	blp.linkid = Linkid[Maxindex];
	// cout<< "blp.fid " << blp.fid << endl;
	// cout<< "blp.linkid "<< blp.linkid << endl;
	return blp;
}
//�ƻ�
void breakLoopRC(PreGraph& pregraph , BreakLoopPoint& blp,queue <unsigned int> &fifo){

	int fid = blp.fid;
	int j = 0, m = 0;
	int linkid = blp.linkid;
	for(int i = 1;i <= flowset.flows[fid].linknumber; i++){
		if(flowset.flows[fid].route[i].linkid == num[linkid]){
			if(i == flowset.flows[fid].linknumber) 
				j = i;
			else{
				m = i + 1;
				break;
			}
		}
	}
	if(j == flowset.flows[fid].linknumber){
		for(int k = 1; k <= NumPre; k++){
			if(num[k] == flowset.flows[fid].route[j].linkid){
				for(int p = 1;p <= pregraph.vertex[k].flownumber; p++){
					if(pregraph.vertex[k].flowurgency[p].fid == fid){						
						pregraph.vertex[k].flowurgency[p].isbreakloop = true;
						break;
					}
				}
				break;
			}
		}
	}
	else {
		for(int k = 1;k <= NumPre; k++){
				if(num[k] == flowset.flows[fid].route[m-1].linkid){
					for(int p = 1;p <= pregraph.vertex[k].flownumber; p++){
						if(pregraph.vertex[k].flowurgency[p].fid == fid){							
							pregraph.vertex[k].flowurgency[p].isbreakloop = true;
							break;
						}
					}
					break;
				}
		}
		for(int i = m;i <= flowset.flows[fid].linknumber; i++){
			for(int k = 1;k <= NumPre; k++){
				if(num[k] == flowset.flows[fid].route[i].linkid){
					pregraph.vertex[k].indegree--;//��ȼ�1
					for(int p = 1;p <= pregraph.vertex[k].flownumber; p++){
						if(pregraph.vertex[k].flowurgency[p].fid == fid){
							pregraph.vertex[k].flowurgency[p].isbreakloop = true;
							break;
						}
					}
					break;
				}
			}
		}
	}
	for(int k = 1;k <= NumPre; k++){
		if(pregraph.vertex[k].indegree == 0)
			fifo.push(k);
	}

}
//�����ƻ���ҵ����
void StackschedulingByUrgencyRC (int previd,int fid,int index){
	int flownumber = pregraph.vertex[previd].flownumber;//ҵ��������
	int j = 0;
	//����ɵ��ȵ�ҵ����fid��previd��ʶ��������·�ϵķ���ʱ���
	int accdelay;
	int sfdelay = network.devices[MAPNUM[pregraph.vertex[previd].link.predev]].sfdelay;//��Сת��ʱ��
	int isfeasible = 1;
	if(index == 1){
		accdelay = 0;
	}
	else{
		for(int k = 1; k <= NumPre; k++){		
			if(pregraph.vertex[k].link.linkid == flowset.flows[fid].route[index-1].linkid)
				for(int p = 1;p <= pregraph.vertex[k].flownumber; p++){
					if(pregraph.vertex[k].flowurgency[p].fid == fid && pregraph.vertex[k].flowurgency[p].isfeasible != 0){
						accdelay = pregraph.vertex[k].flowurgency[p].offset;//ҵ��������ڵ�v_k��ʱ��
						break;
					}
					else if(pregraph.vertex[k].flowurgency[p].fid == fid && pregraph.vertex[k].flowurgency[p].isfeasible == 0){
						accdelay = pregraph.vertex[k].flowurgency[p].offset;//ҵ��������ڵ�v_k��ʱ��
						isfeasible = 0;
						break;
					}
				}
			}			
	}
	int offset;
	if(accdelay == MAX){
		offset = MAX;
	}
	else		
		offset = sfdelay + accdelay;
	for(int p = 1;p <= pregraph.vertex[previd].flownumber; p++){
		if(pregraph.vertex[previd].flowurgency[p].fid == fid && isfeasible == 1 && offset < flowset.flows[fid].etedelay){
			pregraph.vertex[previd].flowurgency[p].offset = offset;
			pregraph.vertex[previd].flowurgency[p].isscheduled = true;
			pregraph.vertex[previd].flowurgency[p].isfeasible = true;
			pregraph.vertex[previd].flowurgency[p].isbreakloop = false;
			j = p;
			break;
		}
		else if(pregraph.vertex[previd].flowurgency[p].fid == fid && isfeasible == 0){
			pregraph.vertex[previd].flowurgency[p].offset = MAX;
			pregraph.vertex[previd].flowurgency[p].isscheduled = true;
			pregraph.vertex[previd].flowurgency[p].isfeasible = false;
			pregraph.vertex[previd].flowurgency[p].isbreakloop = false;
			break;
		}
		else if(pregraph.vertex[previd].flowurgency[p].fid == fid && isfeasible==1 && offset>=flowset.flows[fid].etedelay){
			pregraph.vertex[previd].flowurgency[p].offset = MAX;
			pregraph.vertex[previd].flowurgency[p].isscheduled = true;
			pregraph.vertex[previd].flowurgency[p].isfeasible = false;
			pregraph.vertex[previd].flowurgency[p].isbreakloop = false;
			isfeasible = 0;
//			cout << isfeasible << endl;
			break;
		}

	}
	int flag = 1;
	int number = 0;
	while(flag == 1 && isfeasible == 1){
		for(int i = 1; i <= flownumber; i++){
			if(j == i) continue;
			//����ǻ�û���ȵ�ҵ���������������ƻ���ҵ���������߲��ɵ��ȵ�ҵ���������رȽ�
			if(!pregraph.vertex[previd].flowurgency[i].isscheduled 
				|| pregraph.vertex[previd].flowurgency[i].isbreakloop
				|| !pregraph.vertex[previd].flowurgency[i].isfeasible)continue; 
			
				if(isconflict(i, j ,previd)){
					int fidj = pregraph.vertex[previd].flowurgency[j].fid;
					int periodj = flowset.flows[fidj].period;	

					int chat = 0;
					while(pregraph.vertex[previd].flowurgency[j].offset < flowset.flows[fid].offsetneed){
						pregraph.vertex[previd].flowurgency[j].offset++;	
						chat = 1;
					}
					if(chat == 0)
						pregraph.vertex[previd].flowurgency[j].offset++;

					number++;
					pregraph.vertex[previd].flowurgency[j].offsetdelay++;
					if(pregraph.vertex[previd].flowurgency[j].offset >= flowset.flows[fid].etedelay){
						pregraph.vertex[previd].flowurgency[j].offset = MAX;
						pregraph.vertex[previd].flowurgency[j].isscheduled = true;//true:�Ѿ������ȣ�false:��δ������
						pregraph.vertex[previd].flowurgency[j].isfeasible = false;//true:���еĵ��ȣ�false:�޷�������
						flag = 0;
						unschedulenumRC++;
						ofstream dataFile;
						dataFile.open("output\\urgencyRC.txt", ofstream::app);
						// ��TXT�ĵ���д������
						dataFile << "fid:" << pregraph.vertex[previd].flowurgency[j].fid << endl;
						dataFile << "StackschedulingByUrgency:offset>etedelay" << endl;
						dataFile << "periodj:"<<periodj << endl;
						dataFile << "number:"<<number << endl;
						dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
						dataFile << "===================================" << endl;
						// �ر��ĵ�
						dataFile.close();
						break;
					}
					//��f_i^([v_k,v_j]).offset������f_iһ�������е�����ʱ���λ�󣬼�¼��ҵ�������ɵ���
					else if(number > periodj){
						//����ҵ��������·�еĵ�����Ϣ
						pregraph.vertex[previd].flowurgency[j].offset = MAX;
						pregraph.vertex[previd].flowurgency[j].isscheduled = true;//true:�Ѿ������ȣ�false:��δ������
						pregraph.vertex[previd].flowurgency[j].isfeasible = false;//true:���еĵ��ȣ�false:�޷�������
						flag = 0;
						unschedulenumRC++;		
						ofstream dataFile;
						dataFile.open("output\\urgencyRC.txt", ofstream::app);
						// ��TXT�ĵ���д������S
						dataFile << "fid:" << pregraph.vertex[previd].flowurgency[j].fid << endl;
						dataFile << "StackschedulingByUrgency:number>periodj" << endl;
						dataFile << "periodj:" << periodj << endl;
						dataFile << "number:" << number << endl;
						dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
						dataFile << "===================================" << endl;
						// �ر��ĵ�
						dataFile.close();		
						break;
					}
				}			
		}
		int num = 0;
		for(int k = 1; k <= flownumber; k++){
			if(j == k) continue;
			//����ǻ�û���ȵ�ҵ���������������ƻ���ҵ���������߲��ɵ��ȵ�ҵ���������رȽ�
			if(!pregraph.vertex[previd].flowurgency[k].isscheduled
				|| pregraph.vertex[previd].flowurgency[k].isbreakloop
				|| !pregraph.vertex[previd].flowurgency[k].isfeasible){
				continue;
			}
			if(isconflict(j, k ,previd)){
				num++;
				break;
			}
		}
		if(num == 0){
			flag = 0;
		}

	}	
}