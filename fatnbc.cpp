# include <stdlib.h>
# include <unistd.h>
# include <iostream>
# include <math.h>
# define LEFT 0
# define RIGHT 1
# define ENABLE true
# define DISABLE false
using namespace std;


bool BUSY_STATE=DISABLE;

/*-----------------------------------INTERFACE DEFINITION---------------------------------*/

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

/*-----------------------------------NODE DEFINITION--------------------------------------*/

class node
{
	public:
	Interface LC,RC;
	int *array;
};

/*------------------------------------PACKET STRUCTURE------------------------------------*/

struct packet
{
	int source_address;
	int dest_address;
	int *data;
	int start_timer;
	int end_timer;
};

node** node_address;							//GLOBAL VARIABLE FOR STARTING ADDRESS OF NODE

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
nodes/=2;									//CREATION OF NODES
node **arr = (node **)malloc(nodes * sizeof(node *));
    for (int i=0; i<nodes; i++)
         arr[i] = (node *)malloc(stages * sizeof(node));

for (int i=0;i<=stages;i++)							//INITIALIZATION OF NODES
	for(int j=0;j<pow(2,i);j++)
		{
		arr[i][j].LC.REQ=false;
		arr[i][j].RC.REQ=false;
  		
		arr[i][j].LC.CHOKE=false;
                arr[i][j].RC.CHOKE=false;

  		arr[i][j].LC.CLK=false;
                arr[i][j].RC.CLK=false;

		arr[i][j].LC.Busy_Bit=false;
                arr[i][j].RC.Busy_Bit=false;

  		arr[i][j].LC.Transfer_Bit=false;
                arr[i][j].RC.Transfer_Bit=false;
   
  		arr[i][j].LC.Recieve_Bit=false;
                arr[i][j].RC.Recieve_Bit=false;
		}


int tempk=0;									//ASSIGNING NODES UNDER EACH INTERFACE
for(int i=stages;i>=0;i--)
{	
	tempk=0;
	for(int j=0;j<pow(2,i);j++)
	{ 
		arr[i][j].array=(int*)malloc((nodes/pow(2,i))*sizeof(int));
		for(int k=0;k<nodes*2/pow(2,i);k++)
		{
			arr[i][j].array[k]=tempk;
			tempk++;
		}
	}
}


for (int i=0;i<=stages;i++)
{
	cout<<"\n";
	for(int j=0;j<pow(2,i);j++)
	{
		cout<<"Node["<<i<<"]["<<j<<"]";
		cout<<"\t";
	}
}

if(BUSY_STATE)
{
	int temp_nodes=randomnum(nodes,0);
	int tempi,tempj;
	cout<<"\n\n"<<temp_nodes<<" nodes already under busy state";
	for (int i=0;i<=temp_nodes;i++)
	{
		tempi=randomnum(stages,1);
		tempj=randomnum(pow(2,tempi),1);
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
	arr[nstages][src].LC.Receive_Register=(int*)malloc(8*sizeof(int));
	arr[nstages][src].LC.Receive_Register=p.data;
}
else
{
	arr[nstages][src].RC.Receive_Register=(int*)malloc(8*sizeof(int));
	arr[nstages][src].RC.Receive_Register=p.data;
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
int hope=0;
void Routing_Algorithm (packet p,int nnodes)
{
float src,dest,start,end,nstages;
int i;
nstages=0;
for (i=0;pow(2,i)<nnodes;i++)
	nstages++;
cout<<"\nNo: Of Stages>>>"<<nstages;
node_address=create_topology(nnodes,nstages);
src=p.source_address;
dest=p.dest_address;
cout<<"\n";
int dir,pjs,pis,pjd,pid,nj,ni,tempsrc,tempdest,fromi,fromj;
tempsrc=src;
if(dest!=1)
	tempdest=dest;
else
	tempdest=dest+1;
cout<<"\nPacket Generated At Node["<<nstages<<"]["<<src<<"]";
set_node(nstages,src,p);
fromi=nstages;
fromj=src;
for(i=nstages;i>=0;i--)
{
	pjs=tempsrc/2;
	pjd=tempdest/2;
	pis=pid=i-1;
	if(pjs==pjd)
		break;
	else
	{
		if(pjs%2==0)
		{
			dir=LEFT;
		}
		else
		{
			dir=RIGHT;
		}
	sendpacket(fromi,fromj,pis,pjs,dir);
	cout<<"\nPacket Moved To Node["<<pis<<"]["<<pjs<<"]";
	hope++;
	fromi=pis;
	fromj=pjs;
	}
tempsrc=pjs;
tempdest=pjd;
}
if(pjs%2==0)
	sendpacket(fromi,fromj,pis,pjs,LEFT);
else
	sendpacket(fromi,fromj,pis,pjs,RIGHT);
cout<<"\nPacket Reached The Root Node ["<<pis<<"]["<<pjs<<"]";
hope++;
int flag;

fromi=pis;
fromj=pjs;
while(pis<nstages)
{
	flag=0;
	for(int j=0;j<pow(2,(nstages-pis))/2;j++)
	{
		if (node_address[pis][pjs].array[j]==dest)
		{
			pis=pis+1;
			pjs=pjs*2;
			flag=1;
			sendpacket(fromi,fromj,pis,pjs,LEFT);
			fromi=pis;
			fromj=pjs;
			cout<<"\nPacket Moved To Node["<<pis<<"]["<<pjs<<"]";
			hope++;
		}
	}
	if(flag==0)
	{
		pis=pis+1;
		pjs=pjs*2+1;
		sendpacket(fromi,fromj,pis,pjs,RIGHT);
		fromi=pis;
		fromj=pjs;
		cout<<"\nPacket Moved To Node["<<pis<<"]["<<pjs<<"]";
		hope++;
	}
}
}

int main ()
{
cout<<"\n\n\t\tFATTREE ARCHITECTURE\n\t\t********************";
int nnodes;
AGAIN:cout <<"\n\nEnter The No:Of Nodes >>>";
cin >>nnodes;
if(nnodes<8)
{
cout<<"\n\nFew Nodes... Build Cannot Be Done... Try Again...\n\n";
sleep(2);
goto AGAIN;
}
packet p;
int begin,end;
p.source_address=randomnum(nnodes,1);
cout<<"\n\nSource="<<p.source_address;
p.dest_address=randomnum(nnodes,0);
cout<<"\n\nDestination="<<p.dest_address;
begin=clock();
p.start_timer=begin;
cout<<"\n\nSimulation Begins.... \n\n";
Routing_Algorithm(p,nnodes);
end=clock();
p.end_timer=end;
float diff=end-begin;
cout<<"\n\nHops >>>"<<hope;
cout<<"\n\nProfiled Time>>>"<<diff/CLOCKS_PER_SEC<<"s\n\n";
}



