#define SHMKEY (key_t)3000
#define PERMS 0666

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include "functs.h"
#include <time.h>

int main(int argc, char const *argv[])
{ 
     FILE *fp;
     int endflag=0,shmid;
     ssize_t sharedmemorysize;
     time_t start_t ,end_t,time_elapsed;
     sharedmemory_t *sharedmemory;
     int totalslots,freq;
     time_t mtime;
     start_t=time(NULL);
     vessel_t *array;
    
     for(int i=1; i<argc; i++){
        if(strcmp(argv[i],"-t")==0){                //frequency
            if(i+1<argc){
                freq=atoi(argv[i+1]);
            }
        }
         if(strcmp(argv[i],"-d")==0){                //monitor time
            if(i+1<argc){
                mtime=atoi(argv[i+1]);
            }
        }
        
         if(strcmp(argv[i],"-s")==0){                //shared memory size flag
            if(i+1<argc){
                sharedmemorysize=atoi(argv[i+1]);
            }
        }
         if(strcmp(argv[i],"-f")==0){                //shared memory size flag
            if(i+1<argc){
                totalslots=atoi(argv[i+1]);
            }
        }
		 	 
	}
     if ((shmid = shmget (SHMKEY,sharedmemorysize , PERMS )) < 0) {
		perror("shmget1");
		exit(1);
	 }
     if ( (sharedmemory = shmat(shmid, (sharedmemory_t *) 0, 0)) == (sharedmemory_t *) -1 ) {
		perror("shmat");
		exit(1);
	 }
     array=(vessel_t*)(char*)sharedmemory+sizeof(sharedmemory);
     fp = fopen ("monitorlog.txt","w");// create file to log history of the port
     if (fp== NULL) {
      	printf("Cannot create monitorlog.txt \n");
      	return 1;
   	 }
    
    while(endflag==0){
        sleep(freq);
        printport(array,fp,totalslots,sharedmemory);
        printstats(fp,sharedmemory);
        end_t=time(NULL);
        time_elapsed = difftime(end_t, start_t);
        if(time_elapsed>=mtime)
            endflag=1;
    }
    fclose(fp);
    return 0;
}
