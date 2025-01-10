#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <fstream>

void detect(){
    //在ILP调度器中检查offset是否正确，转换为ILP调度器的输出格式
    ofstream dataFile;
	int terminalnum = 0;
	int unterminalnum = 0;
	for(int x = 1; x <= network.devnumber; x++){
		if(network.devices[x].isterminal == 1)
			terminalnum++;
		else if(network.devices[x].isterminal == 0)
			unterminalnum++;
	}

	dataFile.open("output\\output.txt", ofstream::app);
	//dataFile<<"============每一条数据流在每一个设备上的offset================"<<endl;
	dataFile<<"#n_flows n_devs"<<endl;
	int totalnumber = flowset.fnumber + flowset.fnumberRC;
	dataFile<<totalnumber<<" "<<network.devnumber<<endl;
	dataFile<<"#offset n_flows * n_devs"<<endl;
	int number = flowset.fnumber + flowset.fnumberRC;
	for(int i = 1;i <= number; i++){		
		for(int m =0 ;m < terminalnum; m++){
			int flag1 = 0;
			for(int j = 1;j <= flowset.flows[i].linknumber;j++){
				if(link[flowset.flows[i].route[j].linkid].predev == m){
					flag1 = 1;
					for(int k = 1;k <= NumPre; k++){
						if(pregraph.vertex[k].link.linkid == flowset.flows[i].route[j].linkid){
							for(int p = 1;p <= pregraph.vertex[k].flownumber; p++){
								if(pregraph.vertex[k].flowurgency[p].fid == flowset.flows[i].fid){
									dataFile << pregraph.vertex[k].flowurgency[p].offset * 20000<<" ";
									
								}
							}
						}
					}
				}
	
			}
			if(flag1 == 0)
				dataFile<<0<<" ";
		}

		int x = network.devices[terminalnum+1].devid;
		int z = x+unterminalnum;
		for(int n = x ;n < z; n++){
			int flag2 = 0;
			for(int j = 1;j <= flowset.flows[i].linknumber; j++){
				if(link[flowset.flows[i].route[j].linkid].predev == n){
					flag2 = 1;
					for(int k = 1;k <= NumPre; k++){
						if(pregraph.vertex[k].link.linkid == flowset.flows[i].route[j].linkid){//
							for(int p = 1 ;p <= pregraph.vertex[k].flownumber;p++){
								if(pregraph.vertex[k].flowurgency[p].fid == flowset.flows[i].fid){
									dataFile << pregraph.vertex[k].flowurgency[p].offset * 20000 << " ";								
								}
							}
						}
					}

				}
			}
			if(flag2 == 0)
				dataFile << 0 << " ";
			if(n == z-1)
				dataFile << endl;
		}
		dataFile << "#flow " << flowset.flows[i].fid << endl;
	}
	dataFile.close();
}