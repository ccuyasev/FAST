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
			cout << "调度未结束" << endl;
		
		// for(int k = 1;k <= NumPre; k++){
		// 	cout << pregraph.vertex[k].link.predev << "," << pregraph.vertex[k].link.postdev 
		// 	<< "入度为" << pregraph.vertex[k].indegree << endl;
		// }
		
			return false;
		}
	}
	cout << "开始调度破环的RC业务流" << endl;
	return true;
}
//计算节点previd上所有业务流的紧急程度
void computeUrgencyRC( int previd, Network& network, FlowSet& flowset, PreGraph& pregraph){

	   for(int i = 1; i <= pregraph.vertex[previd].flownumber; i++){
           //无法调度的或已经调度的或被选做破环的业务流，无需计算紧急程度
		   //或者是TT业务流
		   if(!pregraph.vertex[previd].flowurgency[i].isfeasible ||
			   pregraph.vertex[previd].flowurgency[i].isscheduled ||
			   pregraph.vertex[previd].flowurgency[i].isbreakloop ||
			   !pregraph.vertex[previd].flowurgency[i].isRCflow) continue;

		   int fid = pregraph.vertex[previd].flowurgency[i].fid;

		   //如果该结点的业务流的前一跳还没有被调度，说明程序出错
		   for(int j = 2; j <= flowset.flows[fid].linknumber; j++){
				////找出该节点
			   if(flowset.flows[fid].route[j].linkid == num[previd]){				   				   
				   for(int m = 1;m <= NumPre; m++){
						//找出该节点的前一跳节点
						if(pregraph.vertex[m].link.linkid == flowset.flows[fid].route[j-1].linkid){
							for(int k = 1;k <= pregraph.vertex[m].flownumber; k++){
								if(pregraph.vertex[m].flowurgency[k].fid == fid && pregraph.vertex[m].flowurgency[k].isscheduled == 0){									
									cout << pregraph.vertex[m].flowurgency[k].fid << endl;
									cout << pregraph.vertex[m].link.predev << "," << pregraph.vertex[m].link.postdev << endl;
									cout << "pregraph.vertex[m].flowurgency[k].fid: " << pregraph.vertex[m].flowurgency[k].fid<<endl;
									cout << "pregraph.vertex[m].flowurgency[k].isscheduled: " << pregraph.vertex[m].flowurgency[k].isscheduled << endl;
									cout << "pregraph.vertex[m].flowurgency[k].isfeasible: " << pregraph.vertex[m].flowurgency[k].isfeasible << endl;
									cout << "pregraph.vertex[m].flowurgency[k].isbreakloop: " << pregraph.vertex[m].flowurgency[k].isbreakloop << endl;					
									cout << "该结点的业务流的前一跳还没有被调度" << endl;
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
			   pregraph.vertex[previd].flowurgency[i].isscheduled = true;//已经被调度
			   pregraph.vertex[previd].flowurgency[i].isfeasible = false;//不可行的调度
			   pregraph.vertex[previd].flowurgency[i].urgency = MAX;
			   pregraph.vertex[previd].flowurgency[i].offset = MAX;
			   continue;
			}

			int remainmindelay = pregraph.vertex[previd].flowurgency[i].remainmindelay;
			int urgency = flowset.flows[fid].etedelay - sfdelay - accdelay - remainmindelay;
			pregraph.vertex[previd].flowurgency[i].urgency = urgency;

			if(urgency < 0){//说明该业务流无法调度
				//更新业务流在链路中的调度信息
				unschedulenumRC++;
				pregraph.vertex[previd].flowurgency[i].isscheduled = true;//已经被调度
				pregraph.vertex[previd].flowurgency[i].isfeasible = false;//不可行的调度
				pregraph.vertex[previd].flowurgency[i].offset = MAX;
				ofstream dataFile;
				dataFile.open("output\\urgencyRC.txt", ofstream::app);
				// 朝TXT文档中写入数据
				dataFile << "fid:" << pregraph.vertex[previd].flowurgency[i].fid << endl;
				dataFile << "urgency<0:" << endl;
				dataFile << "urgency:"<< urgency << endl;
				dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
				dataFile << "===================================" << endl;
				// 关闭文档
				dataFile.close();
			}
	   }
}
//对结点previd上业务流的紧急程度进行排序
void sortByUrgencyRC(int previd){
	int flownumber = pregraph.vertex[previd].flownumber;//业务流个数
	int flownumberRC = pregraph.vertex[previd].flownumberRC;
	int flownumberTT = pregraph.vertex[previd].flownumberTT;
	// cout<<"flownumberRC:"<<flownumberRC<<endl;
	// cout<<"flownumberTT:"<<flownumberTT<<endl;
	// cout<<"flownumber:"<<flownumber<<endl;
	// cout<<"==========sortByUrgencyRC=================="<<endl;
	//system("pause");
	for (int i = flownumberTT + 1; i <= flownumber; i++) { //控制n-1趟冒泡
		//无法调度的或被选做破环的业务流，无需计算紧急程度
		//或者不是RC业务流
		if( !pregraph.vertex[previd].flowurgency[i].isfeasible || pregraph.vertex[previd].flowurgency[i].isbreakloop||
			   !pregraph.vertex[previd].flowurgency[i].isRCflow ) continue; 
		for (int j = flownumberTT + 1; j <= flownumber - i; j++){
			if( !pregraph.vertex[previd].flowurgency[j].isfeasible || pregraph.vertex[previd].flowurgency[j].isbreakloop||
			   !pregraph.vertex[previd].flowurgency[j].isRCflow ) continue; 
			
			if(pregraph.vertex[previd].flowurgency[j].urgency > pregraph.vertex[previd].flowurgency[j+1].urgency) { //比较相邻的两条业务流紧急程度
				FlowUrgency tmp;//临时变量
				tmp = pregraph.vertex[previd].flowurgency[j]; //交换
				pregraph.vertex[previd].flowurgency[j] = pregraph.vertex[previd].flowurgency [j + 1];
				pregraph.vertex[previd].flowurgency [j + 1] = tmp;
			}
		}
	}
}
//判断两条业务流是否发生冲突
bool isconflictRC (int i, int j, int previd){

	int fidi = pregraph.vertex[previd].flowurgency[i].fid;
	int periodi = flowset.flows[fidi].period;
	int fidj = pregraph.vertex[previd].flowurgency[j].fid;
	int periodj = flowset.flows[fidj].period;
	int n = 1,Lcm = 0;

	//计算两条业务流周期的最小公倍数
	int max = (periodi > periodj)? periodi:periodj;
	do{
		Lcm = max * n;
		n++;
	}while(Lcm % periodi != 0 || Lcm % periodj != 0);

		int maxi = Lcm / periodi;
		int maxj = Lcm / periodj;
		//冲突判定
		for(int a = 0; a < maxi; a++){
			for(int b = 0; b < maxj; b++){
				if(pregraph.vertex[previd].flowurgency[i].offset + a * periodi == pregraph.vertex[previd].flowurgency[j].offset + b * periodj){
					return true;
				}
			}
		}
	return false;

}
//按照紧急程度确定发送时间
void schedulingByUrgencyRC (int previd){

	int flownumber = pregraph.vertex[previd].flownumber;//业务流个数
	int flownumberRC = pregraph.vertex[previd].flownumberRC;
	int flownumberTT = pregraph.vertex[previd].flownumberTT;
	// cout<<"flownumberRC:"<<flownumberRC<<endl;
	// cout<<"flownumberTT:"<<flownumberTT<<endl;
	// cout<<"flownumber:"<<flownumber<<endl;
	// cout<<"===================schedulingByUrgencyRC============="<<endl;
	//system("pause");
	for(int i = flownumberTT + 1; i <= flownumber; i++){//计算可调度的业务流在previd标识的网络链路上的发送时间点
		if( !pregraph.vertex[previd].flowurgency[i].isfeasible || pregraph.vertex[previd].flowurgency[i].isbreakloop||
			   !pregraph.vertex[previd].flowurgency[i].isRCflow) continue; 
		
		int sfdelay = network.devices[MAPNUM[pregraph.vertex[previd].link.predev]].sfdelay;//最小转发时延
		int accdelay = pregraph.vertex[previd].flowurgency[i].accdelay;//业务流到达节点v_k的时间
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
	//判断紧急程度比f_i高的业务流即下标小于i的业务流是否与f_i冲突
	for (int i = flownumberTT + 1; i <= flownumber; i++) {
		if( !pregraph.vertex[previd].flowurgency[i].isfeasible || pregraph.vertex[previd].flowurgency[i].isbreakloop||
			   !pregraph.vertex[previd].flowurgency[i].isRCflow) continue; 
		//紧急程度比f_i高的业务流，即下标小于i的业务流
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
				//若有冲突，高紧急程度业务流的发送时间不变，f_i的发送时间f_i^([v_k,v_j]).offset加1
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
						pregraph.vertex[previd].flowurgency[i].isscheduled = true;//true:已经被调度；false:尚未被调度
						pregraph.vertex[previd].flowurgency[i].isfeasible = false;//true:可行的调度；false:无法被调度
						flag = 0;
						unschedulenumRC++;
						ofstream dataFile;
						dataFile.open("output\\urgencyRC.txt", ofstream::app);
						// 朝TXT文档中写入数据
						dataFile << "fid:" << pregraph.vertex[previd].flowurgency[i].fid << endl;
						dataFile << "schedulingByUrgencyRC:" << endl;
						dataFile << "periodi:" << periodi << endl;
						dataFile << "number:" << number << endl;
						dataFile << pregraph.vertex[previd].link.predev << "," << pregraph.vertex[previd].link.postdev << endl;
						dataFile << "===================================" << endl;
						// 关闭文档
						dataFile.close();
						break;
					}
					//当f_i^([v_k,v_j]).offset遍历完f_i一个周期中的所有时间槽位后，记录该业务流不可调度
					else if(number > periodi){
						//更新业务流在链路中的调度信息
						pregraph.vertex[previd].flowurgency[i].offset = MAX;
						pregraph.vertex[previd].flowurgency[i].isscheduled = true;//true:已经被调度；false:尚未被调度
						pregraph.vertex[previd].flowurgency[i].isfeasible = false;//true:可行的调度；false:无法被调度
						flag = 0;
						unschedulenumRC++;
						ofstream dataFile;
						dataFile.open("output\\urgencyRC.txt", ofstream::app);
						// 朝TXT文档中写入数据
						dataFile << "fid:" << pregraph.vertex[previd].flowurgency[i].fid << endl;
						dataFile << "schedulingByUrgencyRC:" << endl;
						dataFile << "periodi:" << periodi << endl;
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

//更新先序节点为previd的顶点度数、业务流的accdelay、运行时状态
//将入度为0的节点入队列
void updatePreGVertexDegreeRC(int previd,queue <unsigned int> &fifo, stack<BreakLoopPoint>& Stack){
	//队列不为空，拓扑序列输出节点previd后，将先序关系图中previd结点的各个邻接点的入度减1
	PreGEdge *q = pregraph.vertex[previd].firstedge;
	while(q){
		for(int k = 1;k <= NumPre; k++){
			if(pregraph.vertex[k].link.linkid == q->postlinkid && pregraph.vertex[k].indegree != -1){//需要判断业务流编号
				for(int i = 1;i <= pregraph.vertex[k].flownumber; i++){
					if(pregraph.vertex[k].flowurgency[i].fid == q->flowid && pregraph.vertex[k].flowurgency[i].isbreakloop == 0
					&& pregraph.vertex[k].flowurgency[i].isRCflow == 1){
						pregraph.vertex[k].indegree--;//入度减1
						if(pregraph.vertex[k].indegree == 0){
							fifo.push(k); //将入度为0的节点入队列
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

	//更新下一跳的累积传输时延	
	//找出节点的每条可调度业务流的下一跳节点
	int flownumber = pregraph.vertex[previd].flownumber;//业务流个数
	int flownumberRC = pregraph.vertex[previd].flownumberRC;
	int flownumberTT = pregraph.vertex[previd].flownumberTT;

	for (int i = flownumberTT + 1; i <= flownumber; i++){//对于每一条业务流
		int flowid = pregraph.vertex[previd].flowurgency[i].fid;
		//注意j的范围
		for(int j = 1;j < flowset.flows[flowid].linknumber; j++){//找出业务流对应的链路id
			if(pregraph.vertex[previd].link.linkid == flowset.flows[flowid].route[j].linkid){
				for(int k = 1;k <= NumPre; k++){//找出业务流的下一跳
					if(pregraph.vertex[k].link.linkid == flowset.flows[flowid].route[j+1].linkid){
						for(int p = 1;p <= pregraph.vertex[k].flownumber; p++){//找p
							if(pregraph.vertex[k].flowurgency[p].fid == flowid){
								if(pregraph.vertex[k].flowurgency[p].isfeasible){//可行的调度才计算下一跳累积时延
									pregraph.vertex[k].flowurgency[p].accdelay = pregraph.vertex[previd].flowurgency[i].offset;
								}
								else  //不可行的调度，设置为最大值
									pregraph.vertex[k].flowurgency[p].accdelay = MAX;
							}
							
						}
					}
				}
			}

		}
	}
}