#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <fstream>
#include <time.h>
bool areAllPreGVertexScheduledRC(PreGraph& pregraph){
	for(int i = 1; i <= pregraph.vnumber; i++){
		if(pregraph.vertex[i].indegree != -1){
			cout << "����δ����" << endl;
		
		// for(int k = 1;k <= NumPre; k++){
		// 	cout << pregraph.vertex[k].link.predev << "," << pregraph.vertex[k].link.postdev 
		// 	<< "���Ϊ" << pregraph.vertex[k].indegree << endl;
		// }
		
			return false;
		}
	}
	cout << "��ʼ�����ƻ���RCҵ����" << endl;
	return true;
}
//����ڵ�previd������ҵ�����Ľ����̶�
void computeUrgencyRC( int previd, Network& network, FlowSet& flowset, PreGraph& pregraph){

	   for(int i = 1; i <= pregraph.vertex[previd].flownumber; i++){
           //�޷����ȵĻ��Ѿ����ȵĻ�ѡ���ƻ���ҵ�����������������̶�
		   //������TTҵ����
		   if(!pregraph.vertex[previd].flowurgency[i].isfeasible ||
			   pregraph.vertex[previd].flowurgency[i].isscheduled ||
			   pregraph.vertex[previd].flowurgency[i].isbreakloop ||
			   !pregraph.vertex[previd].flowurgency[i].isRCflow) continue;

		   int fid = pregraph.vertex[previd].flowurgency[i].fid;

		   //����ý���ҵ������ǰһ����û�б����ȣ�˵���������
		   for(int j = 2; j <= flowset.flows[fid].linknumber; j++){
				////�ҳ��ýڵ�
			   if(flowset.flows[fid].route[j].linkid == num[previd]){				   				   
				   for(int m = 1;m <= NumPre; m++){
						//�ҳ��ýڵ��ǰһ���ڵ�
						if(pregraph.vertex[m].link.linkid == flowset.flows[fid].route[j-1].linkid){
							for(int k = 1;k <= pregraph.vertex[m].flownumber; k++){
								if(pregraph.vertex[m].flowurgency[k].fid == fid && pregraph.vertex[m].flowurgency[k].isscheduled == 0){									
									cout << pregraph.vertex[m].flowurgency[k].fid << endl;
									cout << pregraph.vertex[m].link.predev << "," << pregraph.vertex[m].link.postdev << endl;
									cout << "pregraph.vertex[m].flowurgency[k].fid: " << pregraph.vertex[m].flowurgency[k].fid<<endl;
									cout << "pregraph.vertex[m].flowurgency[k].isscheduled: " << pregraph.vertex[m].flowurgency[k].isscheduled << endl;
									cout << "pregraph.vertex[m].flowurgency[k].isfeasible: " << pregraph.vertex[m].flowurgency[k].isfeasible << endl;
									cout << "pregraph.vertex[m].flowurgency[k].isbreakloop: " << pregraph.vertex[m].flowurgency[k].isbreakloop << endl;					
									cout << "�ý���ҵ������ǰһ����û�б�����" << endl;
									system("pause");
									break;
								}
							}
						}
				   }
			   }
		   }

		   int sfdelay=network.devices[MAPNUM[pregraph.vertex[previd].link.predev]].sfdelay;
           int accdelay=pregraph.vertex[previd].flowurgency[i].accdelay;
		   if(accdelay == MAX){
			   pregraph.vertex[previd].flowurgency[i].isscheduled = true;//�Ѿ�������
			   pregraph.vertex[previd].flowurgency[i].isfeasible = false;//�����еĵ���
			   pregraph.vertex[previd].flowurgency[i].urgency = MAX;
			   pregraph.vertex[previd].flowurgency[i].offset = MAX;
			   continue;
			}

			int remainmindelay = pregraph.vertex[previd].flowurgency[i].remainmindelay;
			int urgency = flowset.flows[fid].etedelay - sfdelay - accdelay - remainmindelay;
			pregraph.vertex[previd].flowurgency[i].urgency = urgency;

			if(urgency < 0){//˵����ҵ�����޷�����
				//����ҵ��������·�еĵ�����Ϣ
				unschedulenumRC++;
				pregraph.vertex[previd].flowurgency[i].isscheduled = true;//�Ѿ�������
				pregraph.vertex[previd].flowurgency[i].isfeasible = false;//�����еĵ���
				pregraph.vertex[previd].flowurgency[i].offset = MAX;
				ofstream dataFile;
				dataFile.open("output\\urgencyRC.txt", ofstream::app);
				// ��TXT�ĵ���д������
				dataFile << "fid:" << pregraph.vertex[previd].flowurgency[i].fid << endl;
				dataFile << "urgency<0:" << endl;
				dataFile << "urgency:"<< urgency << endl;
				dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
				dataFile << "===================================" << endl;
				// �ر��ĵ�
				dataFile.close();
			}
	   }
}
//�Խ��previd��ҵ�����Ľ����̶Ƚ�������
void sortByUrgencyRC(int previd){
	int flownumber = pregraph.vertex[previd].flownumber;//ҵ��������
	int flownumberRC = pregraph.vertex[previd].flownumberRC;
	int flownumberTT = pregraph.vertex[previd].flownumberTT;
	// cout<<"flownumberRC:"<<flownumberRC<<endl;
	// cout<<"flownumberTT:"<<flownumberTT<<endl;
	// cout<<"flownumber:"<<flownumber<<endl;
	// cout<<"==========sortByUrgencyRC=================="<<endl;
	//system("pause");
	for (int i = flownumberTT + 1; i <= flownumber; i++) { //����n-1��ð��
		//�޷����ȵĻ�ѡ���ƻ���ҵ�����������������̶�
		//���߲���RCҵ����
		if( !pregraph.vertex[previd].flowurgency[i].isfeasible || pregraph.vertex[previd].flowurgency[i].isbreakloop||
			   !pregraph.vertex[previd].flowurgency[i].isRCflow ) continue; 
		for (int j = flownumberTT + 1; j <= flownumber - i; j++){
			if( !pregraph.vertex[previd].flowurgency[j].isfeasible || pregraph.vertex[previd].flowurgency[j].isbreakloop||
			   !pregraph.vertex[previd].flowurgency[j].isRCflow ) continue; 
			
			if(pregraph.vertex[previd].flowurgency[j].urgency > pregraph.vertex[previd].flowurgency[j+1].urgency) { //�Ƚ����ڵ�����ҵ���������̶�
				FlowUrgency tmp;//��ʱ����
				tmp = pregraph.vertex[previd].flowurgency[j]; //����
				pregraph.vertex[previd].flowurgency[j] = pregraph.vertex[previd].flowurgency [j + 1];
				pregraph.vertex[previd].flowurgency [j + 1] = tmp;
			}
		}
	}
}
//�ж�����ҵ�����Ƿ�����ͻ
bool isconflictRC (int i, int j, int previd){

	int fidi = pregraph.vertex[previd].flowurgency[i].fid;
	int periodi = flowset.flows[fidi].period;
	int fidj = pregraph.vertex[previd].flowurgency[j].fid;
	int periodj = flowset.flows[fidj].period;
	int n = 1,Lcm = 0;

	//��������ҵ�������ڵ���С������
	int max = (periodi > periodj)? periodi:periodj;
	do{
		Lcm = max * n;
		n++;
	}while(Lcm % periodi != 0 || Lcm % periodj != 0);

		int maxi = Lcm / periodi;
		int maxj = Lcm / periodj;
		//��ͻ�ж�
		for(int a = 0; a < maxi; a++){
			for(int b = 0; b < maxj; b++){
				if(pregraph.vertex[previd].flowurgency[i].offset + a * periodi == pregraph.vertex[previd].flowurgency[j].offset + b * periodj){
					return true;
				}
			}
		}
	return false;

}
//���ս����̶�ȷ������ʱ��
void schedulingByUrgencyRC (int previd){

	int flownumber = pregraph.vertex[previd].flownumber;//ҵ��������
	int flownumberRC = pregraph.vertex[previd].flownumberRC;
	int flownumberTT = pregraph.vertex[previd].flownumberTT;
	// cout<<"flownumberRC:"<<flownumberRC<<endl;
	// cout<<"flownumberTT:"<<flownumberTT<<endl;
	// cout<<"flownumber:"<<flownumber<<endl;
	// cout<<"===================schedulingByUrgencyRC============="<<endl;
	//system("pause");
	for(int i = flownumberTT + 1; i <= flownumber; i++){//����ɵ��ȵ�ҵ������previd��ʶ��������·�ϵķ���ʱ���
		if( !pregraph.vertex[previd].flowurgency[i].isfeasible || pregraph.vertex[previd].flowurgency[i].isbreakloop||
			   !pregraph.vertex[previd].flowurgency[i].isRCflow) continue; 
		
		int sfdelay = network.devices[MAPNUM[pregraph.vertex[previd].link.predev]].sfdelay;//��Сת��ʱ��
		int accdelay = pregraph.vertex[previd].flowurgency[i].accdelay;//ҵ��������ڵ�v_k��ʱ��
		int offset = sfdelay +accdelay;

		pregraph.vertex[previd].flowurgency[i].offset = offset;
		pregraph.vertex[previd].flowurgency[i].isscheduled = true;
		pregraph.vertex[previd].flowurgency[i].isfeasible = true;
	}

	// if(flownumberRC == 1 && pregraph.vertex[previd].flowurgency[flownumberTT + 1].isRCflow == true){
	// 	int fidi = pregraph.vertex[previd].flowurgency[flownumberTT + 1].fid;
	// 	while(pregraph.vertex[previd].flowurgency[flownumberTT + 1].offset < flowset.flows[fidi].offsetneed
	// 	&& pregraph.vertex[previd].flowurgency[flownumberTT + 1].offset < flowset.flows[fidi].etedelay
	// 	){
	// 		pregraph.vertex[previd].flowurgency[flownumberTT + 1].offset++;
	// 	}
	// 	cout<<pregraph.vertex[previd].flowurgency[flownumberTT + 1].offset<<endl;
	// 	system("pause");
	// }
	// cout<<"flownumberRC:"<<flownumberRC<<endl;
	// cout<<"flownumberTT:"<<flownumberTT<<endl;
	// cout<<"flownumber:"<<flownumber<<endl;
	// system("pause");
	//�жϽ����̶ȱ�f_i�ߵ�ҵ�������±�С��i��ҵ�����Ƿ���f_i��ͻ
	for (int i = flownumberTT + 1; i <= flownumber; i++) {
		if( !pregraph.vertex[previd].flowurgency[i].isfeasible || pregraph.vertex[previd].flowurgency[i].isbreakloop||
			   !pregraph.vertex[previd].flowurgency[i].isRCflow) continue; 
		//�����̶ȱ�f_i�ߵ�ҵ���������±�С��i��ҵ����
		int number = 0;
		int flag = 1;
		// cout<<"i:"<<i<<endl;
		// cout<<"flownumber:"<<flownumber<<endl;
		// system("pause");

		while(flag == 1){
			for(int j = 1; j < i; j++){
				// cout<<"i:"<<i<<endl;
				// cout<<"j:"<<j<<endl;
				
				if(!pregraph.vertex[previd].flowurgency[j].isfeasible || 
				pregraph.vertex[previd].flowurgency[j].isbreakloop) continue; 
				// cout<<"j+++:"<<j<<endl;
				// cout<<"i:"<<i<<endl;
				// system("pause");
				//���г�ͻ���߽����̶�ҵ�����ķ���ʱ�䲻�䣬f_i�ķ���ʱ��f_i^([v_k,v_j]).offset��1
				if(isconflictRC(i, j ,previd)){
					
					int fidi = pregraph.vertex[previd].flowurgency[i].fid;
					int periodi = flowset.flows[fidi].period;
					

					int chat = 0;
					while(pregraph.vertex[previd].flowurgency[i].offset < flowset.flows[fidi].offsetneed){
						pregraph.vertex[previd].flowurgency[i].offset++;
						//cout<<pregraph.vertex[previd].flowurgency[i].offset<<endl;
						chat = 1;
					}
					if(chat == 0)
						pregraph.vertex[previd].flowurgency[i].offset++;

					number++;
					pregraph.vertex[previd].flowurgency[i].offsetdelay++;
					//cout<<"number:"<<number<<endl;				
					if(pregraph.vertex[previd].flowurgency[i].offset >= flowset.flows[fidi].etedelay){
						pregraph.vertex[previd].flowurgency[i].offset = MAX;
						pregraph.vertex[previd].flowurgency[i].isscheduled = true;//true:�Ѿ������ȣ�false:��δ������
						pregraph.vertex[previd].flowurgency[i].isfeasible = false;//true:���еĵ��ȣ�false:�޷�������
						flag = 0;
						unschedulenumRC++;
						ofstream dataFile;
						dataFile.open("output\\urgencyRC.txt", ofstream::app);
						// ��TXT�ĵ���д������
						dataFile << "fid:" << pregraph.vertex[previd].flowurgency[i].fid << endl;
						dataFile << "schedulingByUrgencyRC:" << endl;
						dataFile << "periodi:" << periodi << endl;
						dataFile << "number:" << number << endl;
						dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
						dataFile << "===================================" << endl;
						// �ر��ĵ�
						dataFile.close();
						break;
					}
					//��f_i^([v_k,v_j]).offset������f_iһ�������е�����ʱ���λ�󣬼�¼��ҵ�������ɵ���
					else if(number > periodi){
						//����ҵ��������·�еĵ�����Ϣ
						pregraph.vertex[previd].flowurgency[i].offset = MAX;
						pregraph.vertex[previd].flowurgency[i].isscheduled = true;//true:�Ѿ������ȣ�false:��δ������
						pregraph.vertex[previd].flowurgency[i].isfeasible = false;//true:���еĵ��ȣ�false:�޷�������
						flag = 0;
						unschedulenumRC++;
						ofstream dataFile;
						dataFile.open("output\\urgencyRC.txt", ofstream::app);
						// ��TXT�ĵ���д������
						dataFile << "fid:" << pregraph.vertex[previd].flowurgency[i].fid << endl;
						dataFile << "schedulingByUrgencyRC:" << endl;
						dataFile << "periodi:" << periodi << endl;
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
			for(int k = 1; k < i; k++){
				if(!pregraph.vertex[previd].flowurgency[k].isfeasible || 
				pregraph.vertex[previd].flowurgency[k].isbreakloop) continue; 
				if(isconflictRC(i, k,previd)){
					num++;
					// cout<<"i:"<<i<<endl;
					// cout<<"k:"<<k<<endl;
					// cout<<"num:"<<num<<endl;
					break;
				}	
			}
			if(num == 0)
				flag = 0;
		}

	}
}

//��������ڵ�Ϊprevid�Ķ��������ҵ������accdelay������ʱ״̬
//�����Ϊ0�Ľڵ������
void updatePreGVertexDegreeRC(int previd,queue <unsigned int> &fifo, stack<BreakLoopPoint>& Stack){
	//���в�Ϊ�գ�������������ڵ�previd�󣬽������ϵͼ��previd���ĸ����ڽӵ����ȼ�1
	PreGEdge *q = pregraph.vertex[previd].firstedge;
	while(q){
		for(int k = 1;k <= NumPre; k++){
			if(pregraph.vertex[k].link.linkid == q->postlinkid && pregraph.vertex[k].indegree != -1){//��Ҫ�ж�ҵ�������
				for(int i = 1;i <= pregraph.vertex[k].flownumber; i++){
					if(pregraph.vertex[k].flowurgency[i].fid == q->flowid && pregraph.vertex[k].flowurgency[i].isbreakloop == 0
					&& pregraph.vertex[k].flowurgency[i].isRCflow == 1){
						pregraph.vertex[k].indegree--;//��ȼ�1
						if(pregraph.vertex[k].indegree == 0){
							fifo.push(k); //�����Ϊ0�Ľڵ������
						}
						// cout<<"pregraph.vertex[k].flowurgency[i].fid: "<<pregraph.vertex[k].flowurgency[i].fid<<endl;
						// cout<<"pregraph.vertex[k].indegree:" <<pregraph.vertex[k].indegree<<endl;
						// cout<<pregraph.vertex[k].link.predev<<pregraph.vertex[k].link.postdev<<endl;
					}
				
				 }
			}		
		}
		q = q->edge;
	}

	//������һ�����ۻ�����ʱ��	
	//�ҳ��ڵ��ÿ���ɵ���ҵ��������һ���ڵ�
	int flownumber = pregraph.vertex[previd].flownumber;//ҵ��������
	int flownumberRC = pregraph.vertex[previd].flownumberRC;
	int flownumberTT = pregraph.vertex[previd].flownumberTT;

	for (int i = flownumberTT + 1; i <= flownumber; i++){//����ÿһ��ҵ����
		int flowid = pregraph.vertex[previd].flowurgency[i].fid;
		//ע��j�ķ�Χ
		for(int j = 1;j < flowset.flows[flowid].linknumber; j++){//�ҳ�ҵ������Ӧ����·id
			if(pregraph.vertex[previd].link.linkid == flowset.flows[flowid].route[j].linkid){
				for(int k = 1;k <= NumPre; k++){//�ҳ�ҵ��������һ��
					if(pregraph.vertex[k].link.linkid == flowset.flows[flowid].route[j+1].linkid){
						for(int p = 1;p <= pregraph.vertex[k].flownumber; p++){//��p
							if(pregraph.vertex[k].flowurgency[p].fid == flowid){
								if(pregraph.vertex[k].flowurgency[p].isfeasible){//���еĵ��Ȳż�����һ���ۻ�ʱ��
									pregraph.vertex[k].flowurgency[p].accdelay = pregraph.vertex[previd].flowurgency[i].offset;
								}
								else  //�����еĵ��ȣ�����Ϊ���ֵ
									pregraph.vertex[k].flowurgency[p].accdelay = MAX;
							}
							
						}
					}
				}
			}

		}
	}
}