#include <iostream>
using namespace std;
//��ӡ�ڽ�����
void PrintGraph(){
	cout << "�����������������ڽӱ���������ʾ��" << endl;
	for (int i = 1; i<= network.devnumber; i++){
		cout << network.devices[i].devid;             
		Link * e = network.devices[i].firstedge;
		while (e){                          
			cout << "-->" << e->postdev;
			e = e->link;
		}
		cout << endl;
	}
}
//��ӡ������
void PrintFlow(){
	cout << "����������������������ʾ��" << endl;
	for (int i = 1; i<= flowset.fnumber; i++){
		cout << "ҵ������ţ�"<<flowset.flows[i].fid<<endl;
		cout << "·��·��Ϊ��";
		for(int j = 1; j <= flowset.flows[i].linknumber; j++){
			cout << link[flowset.flows[i].route[j].linkid].predev << ",";
			cout << link[flowset.flows[i].route[j].linkid].postdev << " ";
		}
		cout << endl;
	}	
}
//��ӡ�����ϵͼ
void PrintPreGraph(){
	cout << "�������������ϵͼ��������ʾ��" << endl;
	cout << "NumPre: " << NumPre << endl;
	for(int k = 1; k <= NumPre; k++){
		int flownumber = pregraph.vertex[k].flownumber;
		cout << "k: " << k << endl;
		cout << "flownumber: " << pregraph.vertex[k].flownumber << endl;		
		cout << "indegree: " << pregraph.vertex[k].indegree << endl;	
		for(int i = 1;i <= pregraph.vertex[k].flownumber; i++){
			cout << "----------" << endl;
			cout << "fid: " << pregraph.vertex[k].flowurgency[i].fid << endl;
			cout << "accdelay: " << pregraph.vertex[k].flowurgency[i].accdelay << endl;
			cout << "remainmindelay: " << pregraph.vertex[k].flowurgency[i].remainmindelay << endl;
			cout << "isscheduled: " << pregraph.vertex[k].flowurgency[i].isscheduled << endl;
			cout << "isfeasible: " << pregraph.vertex[k].flowurgency[i].isfeasible << endl;
			cout << "isbreakloop: " << pregraph.vertex[k].flowurgency[i].isbreakloop << endl;
			cout << "isRCflow: " << pregraph.vertex[k].flowurgency[i].isRCflow << endl;
		}
		cout << pregraph.vertex[k].link.predev << "," << pregraph.vertex[k].link.postdev;
		PreGEdge *e = pregraph.vertex[k].firstedge;
		while (e != NULL){
			cout << "-->" << link[e->postlinkid].predev << "," << link[e->postlinkid].postdev;
			e = e->edge;
		}
		cout << endl;
		cout << "================================" << endl;
		
	}
}
