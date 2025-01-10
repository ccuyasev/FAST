#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <fstream>
#include <time.h>
//计算结点previd上业务流i的紧急程度
int computeUrgencyLinkidRC(int previd, int i){

	int fid = pregraph.vertex[previd].flowurgency[i].fid;
    int sfdelay = network.devices[MAPNUM[pregraph.vertex[previd].link.predev]].sfdelay;
	int accdelay = pregraph.vertex[previd].flowurgency[i].accdelay;
	int remainmindelay = pregraph.vertex[previd].flowurgency[i].remainmindelay;
	int urgency = flowset.flows[fid].etedelay - sfdelay - accdelay - remainmindelay;
	if(urgency < 0){//说明该业务流无法调度
		return -1;

	}
	return urgency;
}
//查找需要破坏的业务流中紧急程度的最小值的索引，即urgency的最大值
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
//查找紧急程度的最小值
BreakLoopPoint findLowestUrgencyFlowsRC(){
	//对于每一条业务流
	int urgency[100001], Linkid[100001];
	int fnumberTT = flowset.fnumber + 1;
	int fnumberRC = flowset.fnumberRC;
	int number = flowset.fnumber + flowset.fnumberRC;
	int u = 1;
	for(int i = fnumberTT;i <= number;i++){
		//找出每一条RC业务流的待调度节点
		int flag = 0;	
		for(int j = 1; j <= flowset.flows[i].linknumber; j++){
			for(int k = 1;k <= NumPre; k++){
				if(num[k] == flowset.flows[i].route[j].linkid){
					for(int p = 1;p <= pregraph.vertex[k].flownumber; p++){
						if(pregraph.vertex[k].flowurgency[p].fid == i 
							&& pregraph.vertex[k].flowurgency[p].isscheduled == 0
							&& pregraph.vertex[k].flowurgency[p].isfeasible == 1 
							&& pregraph.vertex[k].flowurgency[p].isbreakloop == 0){
								//尚未被调度，可行的调度，正常业务流																
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
							&& pregraph.vertex[k].flowurgency[p].isbreakloop == 1)||//已经是破环的业务流

							(pregraph.vertex[k].flowurgency[p].fid == i 
							&& pregraph.vertex[k].flowurgency[p].isfeasible == 0)//不可调度的业务流
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
							&& pregraph.vertex[k].flowurgency[p].isbreakloop == 0){//全部调度结束的业务流								
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
//破环
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
					pregraph.vertex[k].indegree--;//入度减1
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
//调度破环的业务流
void StackschedulingByUrgencyRC (int previd,int fid,int index){
	int flownumber = pregraph.vertex[previd].flownumber;//业务流个数
	int j = 0;
	//计算可调度的业务流fid在previd标识的网络链路上的发送时间点
	int accdelay;
	int sfdelay = network.devices[MAPNUM[pregraph.vertex[previd].link.predev]].sfdelay;//最小转发时延
	int isfeasible = 1;
	if(index == 1){
		accdelay = 0;
	}
	else{
		for(int k = 1; k <= NumPre; k++){		
			if(pregraph.vertex[k].link.linkid == flowset.flows[fid].route[index-1].linkid)
				for(int p = 1;p <= pregraph.vertex[k].flownumber; p++){
					if(pregraph.vertex[k].flowurgency[p].fid == fid && pregraph.vertex[k].flowurgency[p].isfeasible != 0){
						accdelay = pregraph.vertex[k].flowurgency[p].offset;//业务流到达节点v_k的时间
						break;
					}
					else if(pregraph.vertex[k].flowurgency[p].fid == fid && pregraph.vertex[k].flowurgency[p].isfeasible == 0){
						accdelay = pregraph.vertex[k].flowurgency[p].offset;//业务流到达节点v_k的时间
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
			//如果是还没调度的业务流，或者用于破环的业务流，或者不可调度的业务流，不必比较
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
						pregraph.vertex[previd].flowurgency[j].isscheduled = true;//true:已经被调度；false:尚未被调度
						pregraph.vertex[previd].flowurgency[j].isfeasible = false;//true:可行的调度；false:无法被调度
						flag = 0;
						unschedulenumRC++;
						ofstream dataFile;
						dataFile.open("output\\urgencyRC.txt", ofstream::app);
						// 朝TXT文档中写入数据
						dataFile << "fid:" << pregraph.vertex[previd].flowurgency[j].fid << endl;
						dataFile << "StackschedulingByUrgency:offset>etedelay" << endl;
						dataFile << "periodj:"<<periodj << endl;
						dataFile << "number:"<<number << endl;
						dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
						dataFile << "===================================" << endl;
						// 关闭文档
						dataFile.close();
						break;
					}
					//当f_i^([v_k,v_j]).offset遍历完f_i一个周期中的所有时间槽位后，记录该业务流不可调度
					else if(number > periodj){
						//更新业务流在链路中的调度信息
						pregraph.vertex[previd].flowurgency[j].offset = MAX;
						pregraph.vertex[previd].flowurgency[j].isscheduled = true;//true:已经被调度；false:尚未被调度
						pregraph.vertex[previd].flowurgency[j].isfeasible = false;//true:可行的调度；false:无法被调度
						flag = 0;
						unschedulenumRC++;		
						ofstream dataFile;
						dataFile.open("output\\urgencyRC.txt", ofstream::app);
						// 朝TXT文档中写入数据S
						dataFile << "fid:" << pregraph.vertex[previd].flowurgency[j].fid << endl;
						dataFile << "StackschedulingByUrgency:number>periodj" << endl;
						dataFile << "periodj:" << periodj << endl;
						dataFile << "number:" << number << endl;
						dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
						dataFile << "===================================" << endl;
						// 关闭文档
						dataFile.close();		
						break;
					}
				}			
		}
		int num = 0;
		for(int k = 1; k <= flownumber; k++){
			if(j == k) continue;
			//如果是还没调度的业务流，或者用于破环的业务流，或者不可调度的业务流，不必比较
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