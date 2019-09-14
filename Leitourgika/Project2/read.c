#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "read.h"

int searchforsubstring(record_t rec,char *substring){
    char castid[50];
    char floatstring[50];
    char houseidstring[50];
    sprintf(castid,"%ld",rec.custid);
    sprintf(floatstring,"%-9.2f",rec.amount);
    sprintf(houseidstring,"%d",rec.HouseID);
    //search for the pattern using strstr
    if(strstr(rec.FirstName,substring) != NULL || strstr(rec.LastName,substring) != NULL ||strstr(castid,substring) || strstr(rec.postcode,substring)
    ||strstr(rec.Street,substring)  || strstr(floatstring,substring) != NULL || strstr(rec.City,substring) != NULL || strstr(houseidstring,substring) != NULL) {
        //printf("%ld firstname: %s lastname: %s  street: %s house id: %d city: %s postcode: %s float: %-9.2f  \n", rec.custid, rec.LastName, rec.FirstName, rec.Street, rec.HouseID, rec.City, rec.postcode, rec.amount);
        return 1;
    }
    else{
        return 0;
    }   
}

void copyrecord(record_t *record,pipe_record_t *piperec,int flaglast){  //help function to copy the records.
    piperec->custid = record->custid;
    strcpy(piperec->FirstName,record->FirstName);
    strcpy(piperec->LastName,record->LastName);
    strcpy(piperec->Street,record->Street);
    strcpy(piperec->City,record->City);
    strcpy(piperec->postcode,record->postcode);
    piperec->HouseID = record->HouseID;
    piperec->amount = record->amount;
    piperec->flaglast=flaglast;

}

size_t writetopipe(int fd,pipe_record_t *piperec,size_t structsize){
    size_t n=0,total=0;
    n=write(fd,piperec,structsize);
    while(errno==4){
        if(errno==4){                                   //if signal caused failure to sys call  it means no data read
            errno=0;
            n=write(fd,piperec,structsize);
        }
    }
    total+=n;
    while(total<structsize){
        while(errno==4){
            if(errno==4){                                   //if signal caused failure to sys call  it means no data read
                errno=0;
                n=write(fd,piperec,structsize);
            }
        }    
    
        n=write(fd,piperec+n,structsize-n);
        total+=n;
    }
}           

