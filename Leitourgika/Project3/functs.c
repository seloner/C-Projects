#define _GNU_SOURCE
#include "functs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int setupconfig(config_t *config,char *filename){
    FILE *fpa;
    char *line=NULL;
    size_t len = 0;
    ssize_t read;
    char *token;
    //init config
    config->S=0;
    config->M=0;
    config->L=0;
    config->ca1=0;
    config->ca1=0;
    config->ca1=0;
    config->co1=0;
    config->co2=0;
    config->co3=0;
    fpa = fopen (filename,"r");     //open file
     if (fpa== NULL) {
      	printf("Cannot access config file %s\n",filename);
      	return -1;
   	}
      while ((read = getline(&line, &len, fpa)) != -1) {
        token = strtok(line," \n");
         while( token != NULL ) {
             //printf("%s\n",token);
             if(strcmp(token,"\n")!=0){
                 if(strcmp(token,"-t1")==0){
                     config->S=1;
                 }
                 if(strcmp(token,"-t2")==0){
                     config->M=1;
                 }
                 if(strcmp(token,"-t3")==0){
                     config->L=1;
                 }
                 if(strcmp(token,"-ca1")==0){
                    token = strtok(NULL," \n");
                    config->ca1=atoi(token);
                 }
                 if(strcmp(token,"-ca2")==0){
                    token = strtok(NULL," \n");
                    config->ca2=atoi(token);
                 }
                 if(strcmp(token,"-ca3")==0){
                    token = strtok(NULL," \n");
                    config->ca3=atoi(token);
                 }
                if(strcmp(token,"-co1")==0){
                    token = strtok(NULL," \n");
                    config->co1=atoi(token);
                 }
                if(strcmp(token,"-co2")==0){
                    token = strtok(NULL," \n");
                    config->co2=atoi(token);
                 }
                if(strcmp(token,"-co3")==0){
                    token = strtok(NULL," \n");
                    config->co3=atoi(token);
                }
                
             }
             if(token)
                token=strtok(NULL," \n");
             
             
         }
    }
     if(line)
        free(line);
   
     fclose(fpa);
    return 0;
}


void printport(vessel_t *vesselsarray,FILE *fp,int totalslots,sharedmemory_t *sharedmemory){
    sem_wait(&sharedmemory->sharedmemoryaccess);
    fprintf(fp,"\n<--------------Port status---------------->\n");
    for (int i=0; i<totalslots; i++){
        if(vesselsarray[i].status==0){
            fprintf(fp,"Type %c position %d currently empty\n",vesselsarray[i].type,i);
        }else{
            fprintf(fp,"Type %c position %d vessel %s\n",vesselsarray[i].type,i,vesselsarray[i].name);
        }
    }
    fprintf(fp,"\n");

    sem_post(&sharedmemory->sharedmemoryaccess);
}
void printstats(FILE *fp,sharedmemory_t *sharedmemory){
        sem_wait(&sharedmemory->sharedmemoryaccess);
        fprintf(fp,"\n<--------------Stats---------------->\n");
        fprintf(fp,"Average total waiting time %.2f minutes\n",sharedmemory->taverage);
        fprintf(fp,"Average s waiting time %.2f minutes\n",sharedmemory->saverage);
        fprintf(fp,"Average m waiting time %.2f minutes\n",sharedmemory->maverage);
        fprintf(fp,"Average l waiting time %.2f minutes\n",sharedmemory->laverage);
        fprintf(fp,"Port total income %d $ ,average income %.2f $\n",sharedmemory->totalincome,sharedmemory->averageincome);
        sem_post(&sharedmemory->sharedmemoryaccess);

}