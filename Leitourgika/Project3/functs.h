#ifndef functs_H_
#define functs_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
typedef struct config{
    bool S;
    bool M;
    bool L;
    int ca1;
    int ca2;
    int ca3;
    float co1;
    float co2;
    float co3;
}config_t;

typedef struct vessel{
    char type;
    char  name[50];
    char  actiontime[50];
    int duration;
    int status;
}vessel_t;

typedef struct publicledger{
    int scurrent;
    int mcurrent;
    int lcurrent;
    char logfile[50];
    vessel_t *portvessels;


}publicledger_t;

typedef struct sharedmemory{
    int enterflag;
    int leaveflag;
    int portending;
    int position;
    int twaiting;
    int stwaiting;
    int mtwaiting;
    int ltwaiting;
    float taverage;
    float saverage;
    float maverage;
    float laverage;
    int totalvessels;
    int svessels;
    int mvessels;
    int lvessels;
    int totalincome;
    float averageincome;
    publicledger_t publicledger;
    vessel_t vesselinformation;
    sem_t portaccess;
    sem_t vessel;
    sem_t portok;
    sem_t vesselok;
    sem_t sharedmemoryaccess;
    sem_t porttraffic;
    sem_t waitforcompletion;
    sem_t squeue;
    sem_t mqueue;
    sem_t lqueue;
}sharedmemory_t;


int setupconfig(config_t *config,char *filename);
void printport(vessel_t *vesselarray,FILE *fp,int totalslots,sharedmemory_t *sharedmemory);
void printstats(FILE *fp,sharedmemory_t *sharedmemory);


#endif