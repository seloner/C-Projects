#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>
#include <stdint.h>
#include "functs.h"
#define SHMKEY (key_t)3000
#define PERMS 0666
int main(int argc, char const *argv[])
{
    float averagewaiting;
	int exitflag=0,waitingtime=0,value,position,upgradeflag=0,mantime,parkperiod,sslots,mslots,totalslots,shmid,semid;
	ssize_t sharedmemorysize;
	pid_t pid;
	vessel_t *array;
    sharedmemory_t *sharedmemory;
	vessel_t vessel;
	FILE *fp;
	time_t current_t,start_t ,end_t,diff_t;
	config_t confignode;
	char pidtostring[50];
	
	for(int i=1; i<argc; i++){
        if(strcmp(argv[i],"-t")==0){                //type flag
            if(i+1<argc){
                if(strcmp(argv[i+1],"s")==0){		//if s vessel
					vessel.type='s';
				}
				 if(strcmp(argv[i+1],"m")==0){		//if m vessel
					vessel.type='m';
				}
				if(strcmp(argv[i+1],"l")==0){		//if l vessel
					vessel.type='l';
				}
            }
        }
		 if(strcmp(argv[i],"-p")==0){			//parking time flag
			 if(i+1<argc){
				 parkperiod=atoi(argv[i+1]);
			 }		 
		 }
		 if(strcmp(argv[i],"-m")==0){		//mantime flag
			 if(i+1<argc){
				 mantime=atoi(argv[i+1]);
			 }		 
		 }
		 if(strcmp(argv[i],"-u")==0){		//mantime flag
			 	upgradeflag=1;
			 		 
		 }	 
	}
	
	sharedmemorysize=atoi(argv[1]); //sharedmemorysize
	
	
	//creating name for vesel
	pid=getpid();			//get pid 
	sprintf(pidtostring,"%d",pid);
	strcpy(vessel.name,"vessel_");
	strcat(vessel.name,pidtostring);
	//
	
    if ((shmid = shmget (SHMKEY, sharedmemorysize , PERMS )) < 0) {
		perror("shmget");
		exit(1);
	}
	
   if ( (sharedmemory = shmat(shmid, (sharedmemory_t *) 0, 0)) == (sharedmemory_t *) -1 ) {
		perror("shmat");
		exit(1);
	 }
	 	array=(vessel_t*)(char*)sharedmemory+sizeof(sharedmemory); //array for port slots
	 
	 		sem_wait(&sharedmemory->sharedmemoryaccess);
			 if(sharedmemory->portending==1){
				 exitflag=1;
				 printf("Bad luck port closed\n");
			 }
			sem_post(&sharedmemory->sharedmemoryaccess);
			if(exitflag==1){		//if port is closed
				fclose(fp);
				return 0;

			} 
		  //check if slots available 
		  
		 if(vessel.type=='s'){		// s type
		 	
	     	sem_wait(&sharedmemory->squeue); 
			 
		 }
		 if(vessel.type=='m'){		// l type
	     	sem_wait(&sharedmemory->mqueue); 
		 }	
		 if(vessel.type=='l'){		// l type
	     	sem_wait(&sharedmemory->lqueue); 
		 } 
		 start_t=time(NULL);
		 //move on		
		 sem_post(&sharedmemory->vessel);	//notify port
		 sem_wait(&sharedmemory->portok);	//wait for ok to inform port master	
		 sem_wait(&sharedmemory->sharedmemoryaccess); //lock shared memory
		 fp = fopen ("logs.txt","a");// create file to log history of the port
   		 if (fp== NULL) {
      		printf("Cannot create logs.txt \n");
      		return 1;
   	   	}
		 current_t=time(NULL);
		 strcpy(vessel.actiontime,asctime(gmtime(&current_t)));
		 sharedmemory->vesselinformation=vessel;
		 sharedmemory->enterflag=1;
		 sem_post(&sharedmemory->sharedmemoryaccess);
		 sem_post(&sharedmemory->vesselok);
		 sem_wait(&sharedmemory->portaccess);		//wait to get permission to enter
		 sem_wait(&sharedmemory->sharedmemoryaccess); //lock shared memory
		 end_t=time(NULL);
		 diff_t=difftime(end_t, start_t);
		 waitingtime+=(int)diff_t;
		 	

		 //position of the vessel in port
		 position=sharedmemory->position;
		 array[position].status=1;
		 strcpy(array[position].name,vessel.name);
		 start_t=time(NULL);
		 fprintf(fp,"Vessel: %s type %c entering , position: %d,time: %s",vessel.name,vessel.type,position,asctime(gmtime(&start_t))); 
		 sem_post(&sharedmemory->sharedmemoryaccess); //unlock shared memoery
		//Entering	 
		 sleep(mantime);
		 sem_post(&sharedmemory->waitforcompletion); //notify port the you entered
		 //Staying in port
		 sleep(parkperiod);
		 //Leaving
		 start_t=time(NULL);
		 sem_post(&sharedmemory->vessel);
		 sem_wait(&sharedmemory->portok);	//wait for ok to inform port master
		 sem_wait(&sharedmemory->sharedmemoryaccess);
		 sharedmemory->leaveflag=1;
		 sem_post(&sharedmemory->sharedmemoryaccess);
		 sem_post(&sharedmemory->vesselok);
		 sem_post(&sharedmemory->portaccess);
		 end_t=time(NULL); 
		 strcpy(vessel.actiontime,asctime(gmtime(&end_t)));
		 sem_wait(&sharedmemory->sharedmemoryaccess);
		 diff_t = difftime(end_t, start_t);
		 waitingtime+=(int)diff_t;
		 if(vessel.type=='s'){		// s type
		    sharedmemory->svessels++; 
			sharedmemory->stwaiting+=waitingtime;
	     	sharedmemory->saverage=sharedmemory->stwaiting/sharedmemory->svessels;
		 }		 
		 if(vessel.type=='m'){		// m type
		 	sharedmemory->mvessels++; 
	     	sharedmemory->mtwaiting+=waitingtime;
	     	sharedmemory->maverage=sharedmemory->mtwaiting/sharedmemory->mvessels;
		 }	
		 if(vessel.type=='l'){		// l type
		 	 sharedmemory->lvessels++; 
		     sharedmemory->ltwaiting+=waitingtime;
	     	 sharedmemory->laverage=sharedmemory->ltwaiting/sharedmemory->lvessels;
	     }
		 sharedmemory->totalvessels++;
		 sharedmemory->twaiting+=waitingtime;
		 sharedmemory->taverage=sharedmemory->twaiting/sharedmemory->totalvessels;
		 vessel.duration=(int)diff_t+mantime;
		 sharedmemory->vesselinformation=vessel;
		 
		 sem_post(&sharedmemory->sharedmemoryaccess);
		 sleep(mantime);
		 sem_wait(&sharedmemory->sharedmemoryaccess);
		 end_t=time(NULL);
		 fprintf(fp,"Vessel: %s type %c leaving , position: %d,time: %s",vessel.name,vessel.type,position,asctime(gmtime(&end_t))); 
		 array[position].status=0;
		 sem_post(&sharedmemory->sharedmemoryaccess);
		 sem_post(&sharedmemory->waitforcompletion);
		 fclose(fp);
		
	 

    return 0;
}
