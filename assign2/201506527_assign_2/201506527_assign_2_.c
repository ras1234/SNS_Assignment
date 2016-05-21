#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <time.h>
#define n 10000/* total number of sensor nodes to be deployed */
#define l1 500
#define l2 500
#define rho 25.0

char DAT_FILE[100];/* locations of sensor nodes in the target field to be stored in a data file */
int KEYRING_SIZE; /* key ring size m for each sensor node */
int KEYPOOL_SIZE; /* key pool size M */
int d; /* average number of neighbor nodes for each sensor node */
double RANGE=25; /* communication range of sensor node */
double p; /* network connectivity during direct key establishment phase */
double pi; /* network connectivity during path key establishment phase using i hops in the path */
int storecoordinates[l1][l2]={0};

typedef struct{
	int x;/* x-coordinate of sensor node location in target field */
	int y;/* y-coordinate of sensor node location in target field */
	int *keyring;/* key ring */
	int phynbrsize; /* number of physical neighbors of a sensor node */
	int keynbrsize; /* number of key neighbors of a sensor node */
	int phynbr[200]; /* list of physical neighbors of a sensor node */
	int keynbr[200]; /* list of key neighbors of a sensor node */
	int id;
	int nonkeyneighbours[200];
	int nonkeysize;

}sensor;
sensor sensorNodes[n];

void calculateAverageDistance()
{
	double sum=0;
	int i,j;
	double xcoordinate;
	double ycoordinate;
	double tempsum=0;
	double totalnodes=0;
	double avg=0;
	for(i=0;i<n;i++)
	{
		for(j=i+1;j<n;j++)
		{
			xcoordinate=pow(sensorNodes[i].x-sensorNodes[j].x,2);
			ycoordinate=pow(sensorNodes[i].y-sensorNodes[j].y,2);
			tempsum=xcoordinate+ycoordinate;
			sum+=sqrt(tempsum);
		}
	}
	totalnodes=(n*(n-1))/2;
	avg=sum/totalnodes;
	printf("Average Distance=%lf m\n",avg);
	printf("Communication Range of sensor nodes=%lf\n",(double)rho);
}

void CalculatePhysicalNeighbours()
{
	printf("\nComputing physical neighbors...\n");
	int i,j,k,l;
	double tempvalue;
	int counterneighbours=0;
	int countkeyneighbour=0;
	int flag=0;
	double avgneignborsize=0;
	int totalneigh=0;
	//int count7=0;
	for(i=0;i<n;i++)
	{
		counterneighbours=0;
		countkeyneighbour=0;
		for(j=0;j<n;j++)
		{
			
			if(i!=j)
			{	
				tempvalue=sqrt(pow(sensorNodes[i].x-sensorNodes[j].x,2)+pow(sensorNodes[i].y-sensorNodes[j].y,2));
				if(tempvalue<=rho)
				{
					flag=0;
					sensorNodes[i].phynbr[counterneighbours]=sensorNodes[j].id;
					counterneighbours++;
					for(k=0;k<KEYRING_SIZE;k++)
					{
						for(l=0;l<KEYRING_SIZE;l++)
						{
							if(sensorNodes[i].keyring[k]==sensorNodes[j].keyring[l])
							{
								sensorNodes[i].keynbr[countkeyneighbour]=sensorNodes[j].id;
								flag=1;
								break;
							}	
						}
						if(flag==1)
						{
							countkeyneighbour++;
							break;
						}
					}
					
				}
			}

		}
		//count7+=countkeyneighbour;
		//printf("Counter Neighbours=%d\n",counterneighbours );
		sensorNodes[i].phynbrsize=counterneighbours;
		sensorNodes[i].keynbrsize=countkeyneighbour;
		//printf("key neig=%d %d\n",i,countkeyneighbour );
		totalneigh+=counterneighbours;
	}
	//printf("%d\n",totalneigh);
	//printf("%d\n",count7);
	avgneignborsize=totalneigh/n;
	d=avgneignborsize;
	printf("Average neighborhood size=%lf\n",avgneignborsize);
}

void calculateAverageNeighboursize()
{
	double A=l1*l2;
 	double d=((n*22*rho*rho)/(A*7))-1;
	printf("\ntheoretical value of Average Neighbour Size from formula=%.0lf \n",d);
}

void calulatedirectnetworkConn()
{
	int i=0;
	double num;
	double totalval=1;
	double M=KEYPOOL_SIZE;
	double m=KEYRING_SIZE;
	for(i=0;i<m;i++)
	{
		num=(M-m-i)/(M-i);
		totalval*=num;
	}
	totalval=1-totalval;
	double totalkeyneigh=0;
	double totalphysneigh=0;
	double connectivityvalue=0;
	for(i=0;i<n;i++)
	{
		totalkeyneigh+=sensorNodes[i].keynbrsize;
		totalphysneigh+=sensorNodes[i].phynbrsize;
	}
	connectivityvalue=totalkeyneigh/totalphysneigh;
	p=connectivityvalue;
	printf("Simulated average network connectivity=%lf\n",connectivityvalue);
	printf("Theoretical connectivity=%lf\n",totalval);
}

void calculatepathnetworkconn()
{
	printf("\nComputing Path Connectivity for hop1...\n");
	//calculating network connectivity for hop1
	double p1;
	p1=(1-p)*(pow((1-p*p),d));
	p1=1-p1;
	printf("Theoretical connectivity =%lf\n",p1);
	int k,i,j;
	int totaldirectkey=0;
	int counter=0;
	int totalcount=0;
	int totalphysicalkey=0;
	int nonkeyindex=0;
	int flag=0;
	for(k=0;k<n;k++)
	{
		flag=0;
		counter=0;
		nonkeyindex=0;
		for(i=0;i<sensorNodes[k].phynbrsize;i++)
		{
			flag=0;
			for(j=0;j<sensorNodes[k].keynbrsize;j++)
			{
				if(sensorNodes[k].phynbr[i]==sensorNodes[k].keynbr[j])
				{
					flag=1;
					break;
				}		
			}
			if(flag==0)
			{
				sensorNodes[k].nonkeyneighbours[nonkeyindex]=sensorNodes[k].phynbr[i];
				nonkeyindex++;
				counter++;
			}
		}
		sensorNodes[k].nonkeysize=counter;
		totalcount+=counter;	
	}
	//printf("counter=%d\n",totalcount);
	int counterhop1=0;
	int l,m;
	flag=0;
	for(k=0;k<n;k++)
	{
		totaldirectkey+=sensorNodes[k].keynbrsize;
		totalphysicalkey+=sensorNodes[k].phynbrsize;
		for(i=0;i<sensorNodes[k].nonkeysize;i++)
		{
			flag=0;
			for(j=0;j<sensorNodes[k].keynbrsize;j++)
			{
				sensor tempNode = sensorNodes[sensorNodes[k].keynbr[j]];
					//iterate over the keys of key neighbours of i
					for(l=0;l<tempNode.keynbrsize;l++)
					{
						if(sensorNodes[k].nonkeyneighbours[i]==tempNode.keynbr[l])
						{
							counterhop1++;
							flag=1;
							break;
						}
					}
					if(flag==1)
						break;
			}
		}
	}
	// printf("direct=%d\n",totaldirectkey );
	// printf("physical=%d\n",totalphysicalkey);
	// printf("counterhop1=%d\n",counterhop1);
	double pathnetworkconn;
	pathnetworkconn=((double)totaldirectkey+(double)counterhop1)/(double)totalphysicalkey;
	printf("Simulated avg network connectivity for path key=%lf\n",pathnetworkconn);

	printf("\nComputing Path Connectivity for hop2...\n");
	double p2;
	p2=(1-p1)*pow((1-p*p1),d);
	p2=1-p2;
	printf("Theoretical network connectivity for hop2=%lf\n",p2);
	
	//Calculating simulating network connectivity for hop2
	int counterhop2;
	int r;
	flag=0;
	for(k=0;k<n;k++)
	{
		counterhop2+=sensorNodes[k].keynbrsize;
		for(i=0;i<sensorNodes[k].nonkeysize;i++)
		{
			flag=0;
			for(j=0;j<sensorNodes[k].keynbrsize;j++)
			{
				//check if can be reached by 1 hop
					sensor tempNode = sensorNodes[sensorNodes[k].keynbr[j]];
					//iterate over the keys of key neighbours of i
					for(l=0;l<tempNode.keynbrsize;l++)
					{
						if(sensorNodes[k].nonkeyneighbours[i]==tempNode.keynbr[l])
						{
							counterhop2++;
							flag=1;
							break;
						}
					}

					if(flag==1)
						break;
			}
			if(flag==0)
			{
				for(j=0;j<sensorNodes[k].keynbrsize;j++)
					{
						sensor hop1Node = sensorNodes[sensorNodes[k].keynbr[j]];
						for(r=0;r<hop1Node.keynbrsize;r++)
						{
							sensor hop2Node = sensorNodes[hop1Node.keynbr[r]];

							//iterate over the keys of key neighbours of i
							for(l=0;l<hop2Node.keynbrsize;l++)
							{
								if(sensorNodes[k].nonkeyneighbours[i]==hop2Node.keynbr[l])
								{
									counterhop2++;
									flag=1;
									break;
								}
							}
							if(flag==1)
								break;
						}
						if(flag==1)
							break;
					}
			}
	
		}
	}
	//printf("counter=%d\n",counterhop2);
	double pathnetworkconn2;
	pathnetworkconn2=(double)counterhop2/(double)totalphysicalkey;
	printf("Simulated network connectivity for path key=%lf\n",pathnetworkconn2);
}

void printsensorNodes()
{
	int counter=0;
	int j,i;
	printf("%d\n",sensorNodes[i].id);
	for(i=0;i<n;i++)
	{
		printf("%d %d\n",sensorNodes[i].x,sensorNodes[i].y );
	}
	for(i=0;i<n;i++)
	{
		for(j=0;j<KEYRING_SIZE;j++)
		{
			printf("%d ",sensorNodes[i].keyring[j] );
		}
		printf("\n");
	}
	for(i=0;i<n;i++)
	{
		for(j=0;j<sensorNodes[i].phynbrsize;j++)
		{
			printf("%d ",sensorNodes[i].phynbr[j]);
		}
		printf("\n");
	}
	for(i=0;i<n;i++)
	{
		for(j=0;j<sensorNodes[i].keynbrsize;j++)
		{
			printf("%d ",sensorNodes[j].keynbr[j]);
		}
		printf("\n");
	}
}

int main(int argc,char *argv[])
{
	srand(time(0) );
	KEYRING_SIZE=atoi(argv[2]);
	KEYPOOL_SIZE=atoi(argv[3]);
	int counter=0;
	int x;
	int y;
	strcpy(DAT_FILE,argv[1]);
	//printf("%s\n",DAT_FILE );
	FILE *fp;
	FILE *fp1;
	int l,k;
	int flag=0;
	int temp;
	while(1)
	{
		if(counter<n)
		{
			
			x=rand()%l1;
			y=rand()%l2;
			if(storecoordinates[x][y]==0)
			{
				storecoordinates[x][y]=1;
				sensorNodes[counter].id=counter;
				sensorNodes[counter].x=x;
				sensorNodes[counter].y=y;
				sensorNodes[counter].keyring=malloc(KEYRING_SIZE *sizeof(int));
				k=0;
				while(k<KEYRING_SIZE)
				{
					flag=0;
					temp=rand()%KEYPOOL_SIZE;
					for(l=0;l<k;l++)
					{
						if(temp==sensorNodes[counter].keyring[l])
							flag=1;
					}
					if(flag==0)
					{
						sensorNodes[counter].keyring[k]=temp;
					//	printf("%d\n",temp );
						k++;
					}	
				}
				//printf("%d\n",counter );
				counter++;
			}
		}
		else if(counter==n)
			break;
	}
   //printf("%d\n",counter );
	char * commandsForGnuplot[] = {"set title \"Sensor Nodes\"","set term png","set output 'output'", "plot 'data.temp'"};

   	fp = fopen("data.temp","w"); 
   	fp1=fopen(DAT_FILE,"w");
   	if( fp == NULL )
   	{
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
   	}
   	fprintf(fp1, "%d\n",n );
   	printf("Reading Data File...\n");
	int i,j;
	for(i=0;i<l1;i++)
	{
		for(j=0;j<l2;j++)
		{
			if(storecoordinates[i][j]==1)
			{
				fprintf(fp, "%lf %lf \n",(double)i,(double)j);
				fprintf(fp1, "%lf %lf\n",(double)i,(double)j);
			}
		}

	}
	FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
    for (i=0; i < 4; i++)
    {
    	fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[i]); //Send commands to gnuplot one by one.
    }
	fclose(fp);
	fclose(fp1); 
	printf("Scaling communication range...\n");
	calculateAverageDistance();
	calculateAverageNeighboursize();
	CalculatePhysicalNeighbours();
	//printsensorNodes();
	printf("\nEG scheme\n");
	printf("Distributing keys...\n");
	printf("\nComputing key neighborhood in direct key establishment phase...\n");
	calulatedirectnetworkConn();
	//calculate path connectivity
	calculatepathnetworkconn();
	return 0;
}
