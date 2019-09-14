#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "functs.h"

#define SHMKEY (key_t)3000
#define PERMS 0666

int main(int argc, char const *argv[])
{	
    config_t confignode;
	int counter=0,outvessels=0,cost,co1,co2,co3,i,totalvessels=0,currentvessels=0,value,sslots,mslots,lslots,totalslots,shmid, semid,traffic=0,numberovessels=0,j;
	FILE *fp,*fp2;
	vessel_t *sarray,*marray,*larray;
	 if(setupconfig(&confignode,(char*)argv[1])<0){
        printf("Config error\n");
        return -1;
    }
	co1=confignode.co1;         //s cost
    co2=confignode.co2;			//m cost
	co3=confignode.co3;			//l cost
	sslots=confignode.ca1;      //s size
    mslots=confignode.ca2;      //m size
    lslots=confignode.ca3;      //l size
	totalslots=mslots+sslots+lslots;		//total port slots
	ssize_t sharedmemorysize;
    sharedmemory_t *sharedmemory;
	sharedmemorysize=atoi(argv[2]);
    if ((shmid = shmget (SHMKEY, sharedmemorysize , PERMS )) < 0) {
		perror("shmget");
		exit(1);
	}
	fp = fopen ("porthistorylog.txt","w");// create file to log history of the port
    if (fp== NULL) {
      	printf("Cannot create porthistorylog.txt \n");
      	return 1;
   	}
	fp2 = fopen ("logs.txt","a");// create file to log history of the port
    if (fp2== NULL) {
      	printf("Cannot create logs.txt \n");
      	return 1;
   	}
	//printf("Accessing shared memory with ID: %d\n",shmid);
   if ( (sharedmemory = shmat(shmid, (sharedmemory_t *) 0, 0)) == (sharedmemory_t *) -1 ) {
		perror("shmat");
		exit(1);
	 }
	 sem_wait(&sharedmemory->sharedmemoryaccess); //lock shared memory
	 strcpy(sharedmemory->publicledger.logfile,"porthistorylog.txt");	// copy the log file name to the portledger
	  sarray=(vessel_t*)(char*)sharedmemory+sizeof(sharedmemory);
	  marray=sarray + sslots;
	  larray=marray + mslots;
	 
	 sem_post(&sharedmemory->sharedmemoryaccess); //unlock shared memory

	 
	 while(1){
		 //if port is empty then wait for someone to wake you up
		sem_wait(&sharedmemory->vessel);
		sem_wait(&sharedmemory->sharedmemoryaccess); //lock shared memory
		if(sharedmemory->portending==1){			//if it is time to close the port
			printf("Port needs to close\n");
			printf("Total vessels %d\n",totalvessels);
			sem_post(&sharedmemory->sharedmemoryaccess);	//unlock the shared memory
			break;
		}
		sem_post(&sharedmemory->sharedmemoryaccess);	//unlock the shared memory
		
		//Giving permission
		sem_post(&sharedmemory->portok); //give ok to start communicating with vessel
		sem_getvalue(&sharedmemory->vesselok,&value);
		sem_wait(&sharedmemory->vesselok); //wait for vessel to answer
		
		sem_wait(&sharedmemory->sharedmemoryaccess);
		if(sharedmemory->enterflag==1){
			
			if(sharedmemory->vesselinformation.type=='s'){
					for(i=0; i<sslots ;i++){
						if((sarray[i].status)==0){	//find the empty place in the port
							sharedmemory->position=i;
							break;
						}																	
					}
			}		
			if(sharedmemory->vesselinformation.type=='m'){
					for(i=0; i<mslots ;i++){
						if(marray[i].status==0){	//find the empty place in the port

							sharedmemory->position=i+sslots;
							break;
						}	
									
					}
			}
			if(sharedmemory->vesselinformation.type=='l'){
					for(i=0; i<lslots ;i++){
						if(larray[i].status==0){	//find the empty place in the port
							sharedmemory->position=i+mslots+sslots;
							break;
						}	
									
					}
			}
			
		}
		sem_post(&sharedmemory->sharedmemoryaccess);
		sem_post(&sharedmemory->portaccess);
		

		sem_wait(&sharedmemory->waitforcompletion);		//wait for the vessel to enter/leave
		//port traffic	
		sem_wait(&sharedmemory->sharedmemoryaccess);	//lock shared memory
		if(sharedmemory->enterflag==1){				//if vessel is entering
			counter++;
			fprintf(fp,"%s Port: %s type %c entered \n",sharedmemory->vesselinformation.actiontime,sharedmemory->vesselinformation.name,sharedmemory->vesselinformation.type);
			//printf("%s type %c entering,time:%s",sharedmemory->vesselinformation.name,sharedmemory->vesselinformation.type,sharedmemory->vesselinformation.actiontime);
			sharedmemory->enterflag=0;
			totalvessels++;
		}else if(sharedmemory->leaveflag==1){	//if vessel is leaving 
			
	
			
			if(sharedmemory->vesselinformation.type=='s'){ //if s type vessel
				cost=(sharedmemory->vesselinformation.duration)*co1;;		
				fprintf(fp,"Port: vessel %s leaving after %d minutes ,cost :%d $\n",sharedmemory->vesselinformation.name,sharedmemory->vesselinformation.duration,cost);
				sem_post(&sharedmemory->squeue); //open one slot

			}
			if(sharedmemory->vesselinformation.type=='m'){ //if m type vessel
				cost=(sharedmemory->vesselinformation.duration)*co2;
				fprintf(fp,"Port: vessel %s leaving after %d minutes ,cost :%d $\n",sharedmemory->vesselinformation.name,sharedmemory->vesselinformation.duration,cost);
				sem_post(&sharedmemory->mqueue); //open one slot

			}
			if(sharedmemory->vesselinformation.type=='l'){ //if l type vessel
				cost=(sharedmemory->vesselinformation.duration)*co3;	
				fprintf(fp,"Port: vessel %s leaving after %d minutes ,cost :%d $\n",sharedmemory->vesselinformation.name,sharedmemory->vesselinformation.duration,cost);
				sem_post(&sharedmemory->lqueue); //open one slot

			}
			outvessels++;
			sharedmemory->totalincome+=cost;
			sharedmemory->averageincome=sharedmemory->totalincome/outvessels;
			
		}
		sem_post(&sharedmemory->sharedmemoryaccess);	
		if(counter==5){	//print status of the port every 5 vessels
				printport(sarray,fp,totalslots,sharedmemory);
				counter=0;
		} 
		
	 }
	 fclose(fp);
	 fclose(fp2);
    return 0;
}
