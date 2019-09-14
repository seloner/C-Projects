#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "myport.h"
#include "functs.h"


#define SHMKEY (key_t)3000
#define PERMS 0666|IPC_CREAT

int main(int argc, char const *argv[])
{   
    FILE *fp;
    publicledger_t publicledger;
    pid_t pid,wpid;
    vessel_t *array;
    char sharedsizestring[50],totalslotstring[50],filename[50];
    config_t confignode;
    int portime,val,shmid, semid ,sslots,lslots,mslots,status=0;
    ssize_t sharedmemorysize;
    for(int i=1; i<argc; i++){
        if(strcmp(argv[i],"-l")==0){                //config file name
            if(i+1<argc){
                strcpy(filename,argv[i+1]);
            }
        }
         if(strcmp(argv[i],"-t")==0){                //port time
            if(i+1<argc){
                portime=atoi(argv[i+1]);
            }
        }
       }
    if(setupconfig(&confignode,filename)<0){
        printf("Config error\n");
        return -1;
    }
     fp = fopen ("logs.txt","w");// create file to log history of the port
   		 if (fp== NULL) {
      		printf("Cannot create logs.txt \n");
      		return 1;
   	};
    sslots=confignode.ca1;      //s size
    mslots=confignode.ca2;      //m size
    lslots=confignode.ca3;      //l size
    sprintf(totalslotstring,"%d",sslots+mslots+lslots);

    sharedmemorysize=sizeof(sharedmemory_t)+(sslots+mslots+lslots)*sizeof(vessel_t); //allocate shared memory size + vessels for port
    //printf("Shared memory size %ld\n",sharedmemorysize);
    sharedmemory_t *sharedmemory;
     if ((shmid = shmget (SHMKEY,sharedmemorysize , PERMS )) < 0) {
		perror("shmget");
		exit(1);
	}
    if ( (sharedmemory = shmat(shmid, (sharedmemory_t *) 0, 0)) == (sharedmemory_t *) -1 ) {
		perror("shmat");
		exit(1);
	 }
       
     //init semaphores values
     sem_init(&(sharedmemory->portaccess),1,0);
     sem_init(&sharedmemory->vessel,1,0);  
     sem_init(&(sharedmemory->vesselok),1,0);  
     sem_init(&sharedmemory->portok,1,0);              
     sem_init(&sharedmemory->sharedmemoryaccess,1,1);
     sem_init(&sharedmemory->porttraffic,1,0);
     sem_init(&sharedmemory->waitforcompletion,1,0);
     sem_init(&sharedmemory->squeue ,1,sslots);   //s slots (for port)
     sem_init(&sharedmemory->mqueue ,1,mslots);   //m slots (for port)
     sem_init(&sharedmemory->lqueue ,1,lslots);   //l slots (for port)
     sem_wait(&sharedmemory->sharedmemoryaccess);
     sharedmemory->enterflag=0;
     sharedmemory->leaveflag=0;
     sharedmemory->portending=0;
     sharedmemory->svessels=0;
     sharedmemory->mvessels=0;
     sharedmemory->lvessels=0;
     sharedmemory->totalvessels=0;
     sharedmemory->saverage=0;
     sharedmemory->maverage=0;
     sharedmemory->laverage=0;
     sharedmemory->taverage=0;
     sharedmemory->twaiting=0;
     sharedmemory->stwaiting=0;
     sharedmemory->mtwaiting=0;
     sharedmemory->ltwaiting=0;
     sharedmemory->totalincome=0;
     sharedmemory->averageincome=0;
     //init shared memory list about vessels
      strcpy(sharedmemory->vesselinformation.name,"\0");
      array=(vessel_t*)(char*)sharedmemory+sizeof(sharedmemory);    //offset calculation for array of vessels
     for(int i=0;i<confignode.ca1;i++){         //for s type
         array[i].type='s';
         array[i].status=0;     
         
     }
     sem_post(&sharedmemory->sharedmemoryaccess);
     for(int i=0;i<confignode.ca2;i++){         //for m type 
         
         array[(confignode.ca1)+i].type='m';
         array[(confignode.ca1)+i].status=0;
         
     }
     for(int i=0;i<confignode.ca3;i++){         //for l type
        
         array[(confignode.ca2+confignode.ca1)+i].type='l';
         array[(confignode.ca2+confignode.ca1)+i].status=0;
         
     }

     if ((pid = fork()) < 0){
                perror("fork failed\n");
    }
    else if(pid==0){
                sprintf(sharedsizestring,"%ld",sharedmemorysize);
                char *args[]={"./port-master",filename,sharedsizestring,NULL}; //pass the args needed and execute port master
                execvp(args[0],args);

    }

    if ((pid = fork()) < 0){
                perror("fork failed\n");
    }
    else if(pid==0){
                sprintf(sharedsizestring,"%ld",sharedmemorysize);
                char *args[]={"./monitor","-s",sharedsizestring,"-f",totalslotstring,"-d","120","-t","20",NULL}; //pass the args needed and execute port master
                execvp(args[0],args);

    }
    for(int i=0; i<5; i++){
        if ((pid = fork()) < 0){
                    perror("fork failed\n");
        }
        else if(pid==0){
                    sprintf(sharedsizestring,"%ld",sharedmemorysize);
                    char *args[]={"./vessel",sharedsizestring,"-t","s","-m","2","-p","4",NULL}; //pass the args needed and execute vessel
                    execvp(args[0],args);

        }
    }
    for(int i=0; i<5; i++){
        if ((pid = fork()) < 0){
                    perror("fork failed\n");
        }
        else if(pid==0){
                    sprintf(sharedsizestring,"%ld",sharedmemorysize);
                    char *args[]={"./vessel",sharedsizestring,"-t","m","-m","2","-p","3",NULL}; //pass the args needed and execute vessel
                    execvp(args[0],args);

        }
    }
     for(int i=0; i<5; i++){
        if ((pid = fork()) < 0){
                    perror("fork failed\n");
        }
        else if(pid==0){
                    
                    sprintf(sharedsizestring,"%ld",sharedmemorysize);
                    char *args[]={"./vessel",sharedsizestring,"-t","l","-m","3","-p","6",NULL}; //pass the args needed and execute vessel
                  
                    execvp(args[0],args);
                    

        }
    }
    sleep(5);
    for(int i=0; i<2; i++){
        if ((pid = fork()) < 0){
                    perror("fork failed\n");
        }
        else if(pid==0){
                    sprintf(sharedsizestring,"%ld",sharedmemorysize);
                    char *args[]={"./vessel",sharedsizestring,"-t","s","-m","2","-p","4",NULL}; //pass the args needed and execute vessel
                    execvp(args[0],args);

        }
    }
    for(int i=0; i<3; i++){
        if ((pid = fork()) < 0){
                    perror("fork failed\n");
        }
        else if(pid==0){
                    sprintf(sharedsizestring,"%ld",sharedmemorysize);
                    char *args[]={"./vessel",sharedsizestring,"-t","m","-m","2","-p","3",NULL}; //pass the args needed and execute vessel
                    execvp(args[0],args);

        }
    }
    sleep(5);
      for(int i=0; i<5; i++){
        if ((pid = fork()) < 0){
                    perror("fork failed\n");
        }
        else if(pid==0){
                    
                    sprintf(sharedsizestring,"%ld",sharedmemorysize);
                    char *args[]={"./vessel",sharedsizestring,"-t","l","-m","3","-p","5",NULL}; //pass the args needed and execute vessel
                  
                    execvp(args[0],args);
                    

        }
    }
    sleep(120);

    printf("Port now should close\n");
    
    while(1){
        sem_wait(&sharedmemory->sharedmemoryaccess);
        sem_getvalue(&sharedmemory->vessel,&val);
        if(val==0)
            break;
        else
            sem_post(&sharedmemory->sharedmemoryaccess);
    }
    sharedmemory->portending=1;
    sem_post(&sharedmemory->sharedmemoryaccess);
    sem_post(&sharedmemory->vessel);
   while ((wpid = wait(&status)) > 0);         //wait for children

     shmdt(sharedmemory);
    if (shmctl(shmid, IPC_RMID, (struct shmid_ds *)0 ) <0) {
		perror("semctl");
		exit(1);
	} 
    return 0;
}
