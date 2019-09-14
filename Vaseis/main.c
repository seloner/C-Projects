#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BF.h"
#include "Ht.h"
#include "Sht.h"

int main(int argc, char const *argv[])
{
    HT_info *hashinfo;
    SHT_info *secondaryhashinfo;
    Record **recordsarray;
    Record record;
    char name[50]="name_0";
    SecondaryRecord secondaryrecord ; 
    hash_node_t *hashtablearray;
    int position,value=255,err,arraysize;
    arraysize=numberofrecords((char* const)argv[1]);
    recordsarray=malloc(arraysize*(sizeof(Record*)));
    long int first_block_number,block_number;
    for(int i=0; i<arraysize; i++){
        recordsarray[i]=malloc(sizeof(Record));
    }
    fillrecordsarray(recordsarray,arraysize,(char* const)argv[1]);
    for(int i=0; i<arraysize; i++){
        long x=string_hash(recordsarray[i]->name,4);
    
        
    }
     BF_Init();
    if(HT_CreateIndex("filename",'i',"id",sizeof(int),4)<0){
        fprintf(stderr,"Error creating hash index\n");
        exit(1);
    }else{
        printf("Create Index success\n");
    }

    hashinfo=HT_OpenIndex("filename");
    if(hashinfo!=NULL){
        printf("Open Index success\n");
        
    }else{
        printf("Error getting info\n");
        return -1;
    }
    for(int i=0; i<arraysize; i++){
        err=HT_InsertEntry(*hashinfo,*recordsarray[i]);
        if(err<0){
            printf("Insert Entry error\n");
            return -1;
        }

    }
    err=SHT_CreateSecondaryIndex("secondfilename","name",strlen("name"),10);
    if(err<0){

        printf("SHT_CreateSecondaryIndex error\n");
        return -1;
    }else{
        printf("SHT_CreateSecondaryIndex success\n");
    }

    secondaryhashinfo=SHT_OpenSecondaryIndex("secondfilename");
    if(secondaryhashinfo==NULL){
        printf("SHT_OpenSecondaryIndex error\n");
    }else{
        printf("SHT_OpenSecondaryIndex success\n");
    }

    hashtablearray=returnhashtable(*hashinfo);
     for(int i=0; i<arraysize; i++){
        if(hashinfo->attrType=='i'){
            position=int_hashfunction(*recordsarray[i],hashinfo->numberofbuckets);
            first_block_number=hashtablearray[position].firstblock;
            block_number=SearchAndReturnBlockNumber(hashinfo->fileDesc,first_block_number,recordsarray[i]->id);
            secondaryrecord.blockId=block_number;
            strcpy(secondaryrecord.key,recordsarray[i]->name);
            err=SHT_SecondaryInsertEntry(*secondaryhashinfo,secondaryrecord);
            if(err<0){
                printf("SHT_SecondaryInsertEntry error");
            }        
        }
    }
    err=SHT_SecondaryGetAllEntries(*secondaryhashinfo,(char *)name);
    if(err<0){
        printf("Get all secondary entries error\n");
        return -1;
    }else{
        printf("Entries found %d\n",err);
    }

    
    // err=Ht_DeleteEntry(*hashinfo,&value);
    // if(err<0){
    //     printf("Ht_DeleteEntry Error\n");
    // }
    // err=HT_GetAllEntries(*hashinfo,&value);
    // if(err<0){
    //     printf("Get all entries error\n");
    //     return -1;
    // }else{
    //     printf("Entries found %d\n",err);
    // }








    err=HT_CloseIndex(hashinfo);
    if(err<0){
        printf("Close index error\n");
        return -1;

    }else{
        printf("Close index success \n");
    }
    err=SHT_CloseSecondaryIndex(secondaryhashinfo);
    if(err<0){
        printf("Close secondary index error\n");
        return -1;

    }else{
        printf("Close secondary index success \n");
    }
    for(int i=0; i<arraysize; i++){
        free(recordsarray[i]);
    }
    free(recordsarray);
    free(hashinfo);
    free(hashtablearray);
    return 0;
}
