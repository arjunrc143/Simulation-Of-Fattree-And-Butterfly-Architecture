# include <iostream>
# include <math.h>
# include <stdlib.h>
# include <unistd.h>
# include <pthread.h>
# include <omp.h>
# define LEFT 0
# define RIGHT 1
# define ENABLE true
# define DISABLE false

using namespace std;

bool BUSY_STATE=DISABLE;

//int iCPU=omp_get_num_procs();
//omp_set_num_threads(iCPU);

class Interface
{
	public:
	bool REQ,ACK,DATA,CLK,CHOKE;
	int *Receive_Register;
	int *Send_Register;
	int *Receive_Buffer;
	int *Send_Buffer;
	bool Busy_Bit, Recieve_Bit, Transfer_Bit;
};

class node
{
	public:
	Interface LC,RC;
	int li,lj,ri,rj;
};

struct packet
{
	int source_address;
	int dest_address;
	int *data;
	int start_timer;
	int end_timer;
};

node** node_address;

int randomnum (int a,int b)
{
	int r;
	if(b==1)
		srand(time(NULL));
	r = rand()%a;
	return r;
}

node** create_topology(int nodes,int stages)
{
	nodes/=2;
	node **arr = (node **)malloc(nodes * sizeof(node *));
    	for (int i=0; i<nodes; i++)
       		arr[i] = (node *)malloc(stages * sizeof(node));
	int k;
	
//	#pragma omp parallel for	

	for (int i=0;i<nodes;i++)
		for(int j=0/*,k=stages-1*/;j<stages;j++/*,k--*/)
		{
			arr[i][j].LC.REQ=0;
			arr[i][j].RC.REQ=0;

			arr[i][j].LC.ACK=0;
                	arr[i][j].RC.ACK=0;
  		
			arr[i][j].LC.CHOKE=0;
                	arr[i][j].RC.CHOKE=0;

  			arr[i][j].LC.CLK=0;
                	arr[i][j].RC.CLK=0;

			arr[i][j].LC.Busy_Bit=0;
                	arr[i][j].RC.Busy_Bit=0;

  			arr[i][j].LC.Transfer_Bit=0;
                	arr[i][j].RC.Transfer_Bit=0;

  			arr[i][j].LC.Recieve_Bit=0;
                	arr[i][j].RC.Recieve_Bit=0;
		}
	int temp1,temp2;
	
	k=stages-1;

//	#pragma omp parallel for

	for(int j=0/*,k=stages-1*/;j<stages;j++/*,k--*/)
	{
		temp1=pow(2,k)/2;
		temp2=temp1;
		for(int i=0;i<nodes;i++)
		{
			if(i<temp1)
			{
				arr[i][j].li=i;
				arr[i][j].lj=j+1;
				arr[i][j].ri=i+(pow(2,k)/2);
				arr[i][j].rj=j+1;
			}
			else
			{
				arr[i][j].li=i-(pow(2,k)/2);
				arr[i][j].lj=j+1;
				arr[i][j].ri=i;
				arr[i][j].rj=j+1;
			}
			if(i+1==temp1+temp2)
				temp1=temp1+temp2*2;
		}
	k--;
	}	
	
	k=0;
	
//	#pragma omp parallel for

	for(int i=0/*,k=0*/;i<nodes;i++/*,k=k+2*/)
	{
		arr[i][stages-1].li=0;
		arr[i][stages-1].ri=0;
		arr[i][stages-1].lj=k;
		arr[i][stages-1].rj=k+1;
		k=k+2;
	}

//	#pragma omp parallel for 

	for (int i=0;i<nodes;i++)
	{
//		cout<<"\n";
		for(int j=0;j<stages;j++)
		{
//			cout<<"Node["<<i<<"]["<<j<<"]";
//			cout<<"\t";
		}
	}

	if(BUSY_STATE)
	{
		int temp_nodes=randomnum(nodes*2,0);
		int tempi,tempj;
		cout<<"\n\n"<<temp_nodes<<" nodes already under busy state";
		for (int i=0;i<=temp_nodes;i++)
		{
			tempi=randomnum(nodes,1);
			tempj=randomnum(stages,1);
			arr[tempi][tempj].LC.Busy_Bit=true;
			arr[tempi][tempj].RC.Busy_Bit=true;
		}		
	}	
	return arr;
}


void set_node(int nstages,int src,packet p)
{
	node** arr=node_address;
	p.data=(int*)malloc(8*sizeof(int));
	for(int i=0;i<8;i++)
		if(i==0)
			p.data[i]=randomnum(10,1);
		else
			p.data[i]=randomnum(10,0);
	cout<<"\nGenerated Packet is ";
	for(int i=0;i<8;i++)
		cout<<p.data[i];
	if(src%2==0)
	{
		arr[src/2][0].LC.Receive_Register=(int*)malloc(8*sizeof(int));
		arr[src/2][0].LC.Receive_Register=p.data;
	}	
	else
	{
		arr[src/2][0].RC.Receive_Register=(int*)malloc(8*sizeof(int));
		arr[src/2][0].RC.Receive_Register=p.data;
	}
	free(p.data);
}



bool request(int pis,int pjs)
{
	node** arr=node_address;
	if(BUSY_STATE)
	{
		if (arr[pis][pjs].LC.Busy_Bit==true)
		{
			int BC_Release_Time=randomnum(10,0);
			for(int i=0;i<10;i++)
			{
				if(i==BC_Release_Time)
				{
					arr[pis][pjs].LC.Busy_Bit=false;
					arr[pis][pjs].RC.Busy_Bit=false;
					break;
				}
				else
				{
					cout<<"\nWaiting For Acknowledgemnet...";
				}
			}
			if(pjs%2==0)
                		node_address[pis][pjs].LC.ACK=true;
                	else
                	        node_address[pis][pjs].RC.ACK=true;
        		cout<<"\nAcknowledgement at Node["<<pis<<"]["<<pjs<<"] Within Timeout...";
        		return true;
		}	
		else
		{
			if(pjs%2==0)
				node_address[pis][pjs].LC.ACK=true;
			else
				node_address[pis][pjs].RC.ACK=true;
		cout<<"\nAcknowledgement at Node["<<pis<<"]["<<pjs<<"] Within Timeout...";
		return true;
		}
	}	
	else
	{
		if(pjs%2==0)
		node_address[pis][pjs].LC.ACK=true;
		else
		node_address[pis][pjs].RC.ACK=true;
		cout<<"\nAcknowledgement at Node["<<pis<<"]["<<pjs<<"] Within Timeout...";
		return true;
	}
}

void sendpacket(int fromi,int fromj,int pis,int pjs,int dir)
{
	node** arr=node_address;
	if(dir==LEFT)
	{
		arr[fromi][fromj].LC.REQ=true;
		arr[fromi][fromj].LC.Send_Buffer=(int*)malloc(8*sizeof(int));
		arr[fromi][fromj].LC.Send_Buffer=arr[fromi][fromj].LC.Receive_Register;
		cout<<"\nInitiating Request at Node ["<<fromi<<"]["<<fromj<<"]...";
		if(request(pis,pjs))
		{
			arr[pis][pjs].LC.Receive_Buffer=(int*)malloc(8*sizeof(int));
			arr[pis][pjs].LC.Receive_Buffer=arr[fromi][fromj].LC.Send_Buffer;
			arr[pis][pjs].LC.Receive_Register=(int*)malloc(8*sizeof(int));
			arr[pis][pjs].LC.Receive_Register=arr[pis][pjs].LC.Receive_Buffer;
		}
	}
	else
	{
		arr[fromi][fromj].RC.REQ=true;
		arr[fromi][fromj].RC.Send_Buffer=(int*)malloc(8*sizeof(int));
		arr[fromi][fromj].RC.Send_Buffer=arr[fromi][fromj].RC.Receive_Register;
		cout<<"\nInitiating Request at Node ["<<fromi<<"]["<<fromj<<"]...";
		if(request(pis,pjs))
		{
			arr[pis][pjs].RC.Receive_Buffer=(int*)malloc(8*sizeof(int));
			arr[pis][pjs].RC.Receive_Buffer=arr[fromi][fromj].RC.Send_Buffer;
			arr[pis][pjs].RC.Receive_Register=(int*)malloc(8*sizeof(int));
			arr[pis][pjs].RC.Receive_Register=arr[pis][pjs].RC.Receive_Buffer;;
		}
	}
}
	

void Routing_Algorithm (packet p,int nnodes)
{
	float src,dest,start,end,nstages;
	int mid;
	int loc,grp,dir;
	int k,i;
	nstages=0;
	nnodes/=2;
	for (i=0;pow(2,i)<nnodes;i++)
		nstages++;
	cout<<"\nNo: Of Stages="<<nstages;
	node_address=create_topology(nnodes,nstages);
/*	src=p.source_address;
	dest=p.dest_address;
	grp=1;
	start=0;
	end=nnodes-1;
	mid=(start+end)/2;
	cout<<"\n";
	set_node(nstages,src,p);
	int fromi,fromj,pis,pjs;
	fromi=src/2;
	fromj=0;
	cout<<"\nPacket Generated At Node["<<fromi<<"]["<<fromj<<"]";
	for(i=0,k=nstages;grp<nnodes;i++,k--)
	{
		if(grp*2==nnodes)
		{
			if((start<=mid)&&(dest<=mid))
				loc=0;
			if ((start<=mid)&&(dest>mid))
				loc=1;
			if ((start>=mid)&&(dest<mid))
				loc=2;
			if ((start>=mid)&&(dest>mid))
				loc=3;
		}
		else
		{
			if ((start<mid)&&(dest<=mid))
				loc=0;
			if ((start<mid)&&(dest>mid))
 				loc=1;
			if ((start>mid)&&(dest<mid))
 				loc=2;
			if ((start>mid)&&(dest>mid))
 				loc=3;
		}
		switch(loc)
		{ 
			case 0:	dir=LEFT;
			       	end=mid;
       				pis=node_address[fromi][fromj].li;
				pjs=node_address[fromi][fromj].lj;
				sendpacket(fromi,fromj,pis,pjs,dir);
				if(grp*2!=nnodes)
					cout<<"\nPacket Reached Node["<<pis<<"]["<<pjs<<"]";
				else
					cout<<"\nPacket Reached Destination "<<pjs;
				break;
			case 1:	dir=RIGHT;
				start=mid+1;
 				pis=node_address[fromi][fromj].ri;
        			pjs=node_address[fromi][fromj].rj;
				sendpacket(fromi,fromj,pis,pjs,dir);
				if(grp*2!=nnodes)
        				cout<<"\nPacket Reached Node["<<pis<<"]["<<pjs<<"]";
				else
					cout<<"\nPacket Reached Destination "<<pjs;
       				break;
			case 2:	dir=LEFT;
       				end=mid;
 				pis=node_address[fromi][fromj].li;
        			pjs=node_address[fromi][fromj].lj;
				sendpacket(fromi,fromj,pis,pjs,dir);
				if(grp*2!=nnodes)
        				cout<<"\nPacket Reached Node["<<pis<<"]["<<pjs<<"]";
 				else
                                        cout<<"\nPacket Reached Destination "<<pjs;
       				break;
			case 3:	dir=RIGHT;
       				start=mid+1;
 				pis=node_address[fromi][fromj].ri;
        			pjs=node_address[fromi][fromj].rj;
				sendpacket(fromi,fromj,pis,pjs,dir);
				if(grp*2!=nnodes)
        				cout<<"\nPacket Reached Node["<<pis<<"]["<<pjs<<"]";
       				else
                                        cout<<"\nPacket Reached Destination "<<pjs;
				break;
		}
		grp=grp*2;
		mid=(start+end)/2;
		fromi=pis;
		fromj=pjs;
	}
	cout<<"\nHops>>>"<<nstages;	*/
}
 
int main ()
{
	cout<<"\n\n\t\tButterfly Architecture\n\t\t**********************";
	int nnodes;
	AGAIN:cout <<"\n \nEnter The No:Of Nodes >>";
	cin >>nnodes;
	if (nnodes<4)
	{
		cout<<"\nFew Nodes... Build Cannot Be Done... Try Again...";
		sleep(2);
		goto AGAIN;
	}
	packet p;
	int begin,end;
	p.data=0;
	//p.source_address=randomnum(nnodes/2,1);
	cout<<"\nEnter The Source>>";
	cin>>p.source_address;
	//cout<<"\n\nSource="<<p.source_address;
	//p.dest_address=randomnum(nnodes/2,0);
	cout<<"\nEnter The Destination>>";
	cin>>p.dest_address;
	//cout<<"\n\nDestination="<<p.dest_address;
	begin=clock();
	p.start_timer=begin;
	Routing_Algorithm(p,nnodes);
	end=clock();
	p.end_timer=end;
	float diff=end-begin;
	cout<<"\nProfiled Time>>>"<<diff/CLOCKS_PER_SEC<<"s\n\n";
}



