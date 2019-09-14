#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "BF.h"
#include "Ht.h"
#include "Sht.h"
//Synartiseis HT
int numberofbuckets=0;
int maxrecordsperblock=0;

int HT_CreateIndex(char *filename,char attrType,char *attrName,int attrLength,int buckets){
    void *block;
    int blockcounter,filecreated=0,hashtablesize=0,numberofrecords=0,err=0,fd=0;
    HT_info info;
    hash_node_t *hashtablearray;
    numberofbuckets=buckets;
    size_t sizeofhasinfo;
    maxrecordsperblock=(BLOCK_SIZE-sizeof(blockinformation_t))/sizeof(Record);                //calculate max records per block
    err=BF_CreateFile(filename);                                        //create the file
    if(err!=0){
            BF_PrintError("File create error");
            return -1;
    }                                                           
    fd=BF_OpenFile(filename);                                          //open the file
    if(fd<0){
            BF_PrintError("File open error");
            return -1;
    }
    err=BF_AllocateBlock(fd);
    blockcounter=BF_GetBlockCounter(fd);
    if(err<0){
            BF_PrintError("Allocate block error");
    }
    err=BF_ReadBlock(fd,blockcounter-1,&block);                                 
    if(err<0){
            BF_PrintError("Reading block error");
            return -1;
    }
    //arxikopoihsh info gia hash table
       info.attrLength=attrLength;
       info.numberofbuckets=numberofbuckets;
       info.fileDesc=-1;
       strcpy(info.attrName,attrName);
       info.attrType=attrType;
       info.fileDesc=-1;
       memcpy(block,&info,sizeof(HT_info));
       err=BF_WriteBlock(fd,blockcounter-1);
         if(err<0){
            BF_PrintError("Writing block error");
            return -1;
        }
    
    //Molis arxikopoihthike to prwto block tou arxeiou mas
    err=BF_AllocateBlock(fd);
    if(err<0){
            BF_PrintError("Allocate block error");
            return -1;
    }
    blockcounter=BF_GetBlockCounter(fd);
     err=BF_ReadBlock(fd,blockcounter-1,&block);
    if(err<0){
            BF_PrintError("Reading block error");
            return -1;
    }
    if((numberofbuckets*sizeof(hash_node_t))>BLOCK_SIZE){
        printf("Error hash table cant fit into one block");
        return -1;
    }
    if((hashtablearray=malloc(numberofbuckets*sizeof(hash_node_t)))==NULL){
            printf("Malloc faield\n");
            return -1;
    }
    for(int i=0; i<numberofbuckets;i++){
            hashtablearray[i].id=i;
            hashtablearray[i].firstblock=-1;
            hashtablearray[i].currentrecords=0;
            hashtablearray[i].lastblock=-1;

    }
     for(int i=0; i<numberofbuckets;i++){
            memcpy(block,&hashtablearray[i],sizeof(hash_node_t));
            block=block+sizeof(hash_node_t);
    }
    err=BF_WriteBlock(fd,blockcounter-1);
    if(err<0){
            BF_PrintError("Writing block error");
            return -1;
    }
    //perasma tou hash table ston disko
    free(hashtablearray);
     err=BF_CloseFile(fd);
     if(err<0){
             BF_PrintError("close file error");
             return -1;
     }
    
    
    return 0;


}

HT_info* HT_OpenIndex(char *filename){
        int err,fd;
        void *block;
        HT_info *info;
        info=malloc(sizeof(HT_info));
       
        fd=BF_OpenFile(filename);                               //open the file
         if(fd<0){
            BF_PrintError("File open error");
            return NULL;
        }
         err=BF_ReadBlock(fd,0,&block);                         //read the block about hash info
        if(err<0){
            BF_PrintError("Reading block error");
            return NULL;
        }
        memcpy(info,block,sizeof(HT_info));
        info->fileDesc=fd;
        return info;                           
}
int HT_CloseIndex(HT_info *info){
        int fd;
        fd=BF_CloseFile(info->fileDesc);
         if(fd<0){
                 BF_PrintError("Close file error");
                 return -1;
        }else{
                return 0;
        }      

}
int HT_InsertEntry(HT_info infonode,Record record){
        void *block;
        blockinformation_t blockinformation;
        hash_node_t *hashtablearray;
        int flaghchange=0,ht_size,previous,flagstop=0,nextblock,blocknumber,fd,position=-1,err;
        fd=infonode.fileDesc;
        if(infonode.attrType=='i'){      //if key is an integer 
                position=int_hashfunction(record,infonode.numberofbuckets);
        }
        
        err=BF_ReadBlock(fd,1,&block);
        if(err<0){
            BF_PrintError("Hello Reading block error");
            return -1;
        }
       
        ht_size=infonode.numberofbuckets;
        hashtablearray=malloc(ht_size*sizeof(hash_node_t));
        for(int i=0; i<ht_size; i++){
                memcpy(&hashtablearray[i],(hash_node_t*)block,sizeof(hash_node_t));
                block+=sizeof(hash_node_t);
        }
        // for(int i=0; i<ht_size; i++){
        //         printf("i:%d firstblock->%d\n",i,hashtablearray[i].firstblock);
        // }
        if(hashtablearray[position].firstblock==-1){    //if no block available
                
                flaghchange=1;
                err=BF_AllocateBlock(fd);
                if(err<0){
                        BF_PrintError("Allocate block error");
                }
                blocknumber=BF_GetBlockCounter(fd)-1;
                if(blocknumber<0){
                        BF_PrintError("Get block counter error");
                        return -1;
                }
                hashtablearray[position].firstblock=blocknumber;
                hashtablearray[position].lastblock=blocknumber;
                if(err<0){
                        BF_PrintError("Write block error");
                        return -1;
                }
                blockinformation.nextblock=-1;
                blockinformation.currentrecords=1;
                //printf("Allocated the first block\n");
                err=BF_ReadBlock(fd,blocknumber,&block);
                if(err<0){
                        BF_PrintError("Reading block error");
                        return -1;
                }
                memcpy(block,&blockinformation,sizeof(blockinformation_t));
                block=block + sizeof(blockinformation_t);
                memcpy(block,&record,sizeof(Record));
                err=BF_WriteBlock(fd,blocknumber);
                if(err<0){
                        BF_PrintError("Write block error");
                        return -1;
                }
        }else{
                
                 
                err=BF_ReadBlock(fd,hashtablearray[position].lastblock,&block);
                     
                if(err<0){
                        BF_PrintError("Print error\n");
                }
              
                memcpy(&blockinformation,block,sizeof(blockinformation_t));
                  
                if(blockinformation.currentrecords<maxrecordsperblock){                         //if the records fits
                       blockinformation.currentrecords++;
                       memcpy(block,&blockinformation,sizeof(blockinformation_t));
                       block=block +(blockinformation.currentrecords-1)*sizeof(Record)+sizeof(blockinformation_t);
                       memcpy(block,&record,sizeof(Record));
                       err=BF_WriteBlock(fd,hashtablearray[position].lastblock);
                       if(err<0){
                                   BF_PrintError("Write block error");
                                   return -1;
                        }


                }else{
                       
                        previous=hashtablearray[position].lastblock;
                        err=BF_AllocateBlock(fd);
                        if(err<0){
                                BF_PrintError("Allocate block error");
                        }
                        blocknumber=BF_GetBlockCounter(fd)-1;
                        blockinformation.nextblock=blocknumber;
                        memcpy(block,&blockinformation,sizeof(blockinformation_t));
                        hashtablearray[position].lastblock=blocknumber;
                        err=BF_WriteBlock(fd,previous);
                        if(err<0){
                                BF_PrintError("Write block error");
                        }
                        err=BF_ReadBlock(fd,blocknumber,&block);
                        if(err<0){
                                BF_PrintError("Reading block error");
                                return -1;
                        }
                        blockinformation.currentrecords=1;
                        blockinformation.nextblock=-1;
                        memcpy(block,&blockinformation,sizeof(blockinformation_t));
                        block=block+sizeof(blockinformation_t);
                        memcpy(block,&record,sizeof(Record));
                        //printf("Added record with id %d\n",record.id);
                        err=BF_WriteBlock(fd,blocknumber);
                        if(err<0){
                                BF_PrintError("Write block error");
                                return -1;
                        }

             }
        }                
        BF_ReadBlock(fd, 1, &block);
        for ( int i=0 ; i<ht_size ; i++ ) {
                memcpy(block, &hashtablearray[i], sizeof(hash_node_t)) ; 
                block += sizeof(hash_node_t);
        }
        BF_WriteBlock(fd, 1) ;
        free(hashtablearray);
        
        return 0;

}
int HT_GetAllEntries(HT_info header_info,void *value){
        int flagfound=0,result=0;
        int position=-1,*firstblockarray,ht_size,err,fd=header_info.fileDesc;
        hash_node_t *hashtablearray;
        void *block;
        err=BF_ReadBlock(fd,1,&block);
        if(err<0){
                BF_PrintError("Reading block error");
                return -1;
        }
        ht_size=header_info.numberofbuckets;
        firstblockarray=malloc(sizeof(int)*ht_size);
        hashtablearray=malloc(ht_size*sizeof(hash_node_t));
        for(int i=0;i<ht_size; i++){
                memcpy(&hashtablearray[i],block,sizeof(hash_node_t));
                firstblockarray[i]=hashtablearray[i].firstblock;
                block=block+sizeof(hash_node_t);
        }
        if(header_info.attrType=='i'){
                Record temprecord;
                temprecord.id=*(int *)value;
                position=int_hashfunction(temprecord,header_info.numberofbuckets);;
                result=printblocks(fd,firstblockarray[position],value,header_info.attrType);
                
        }
        if(result>0)
                return result;
        else    
                return -1;        
}

int Ht_DeleteEntry(HT_info header_info,void *value){
        void *block;
        long int starting_block,block_returned;
        blockinformation_t blockinformation;
        hash_node_t *hashtablearray;
        int numberofrecords,position,err,fd,ht_size,counter=0;
        fd=header_info.fileDesc;
        ht_size=header_info.numberofbuckets;
        Record temp,record;
        err=BF_ReadBlock(fd,1,&block);
        if(err<0){
            BF_PrintError("Hello Reading block error");
            return -1;
        }
        hashtablearray=malloc(ht_size*sizeof(hash_node_t));
        for(int i=0; i<ht_size; i++){
                memcpy(&hashtablearray[i],(hash_node_t*)block,sizeof(hash_node_t));
                block+=sizeof(hash_node_t);
        }
        if(header_info.attrType=='i'){
                temp.id=*(int*)value;
                position=int_hashfunction(temp,header_info.numberofbuckets);
                starting_block=hashtablearray[position].firstblock;
                block_returned=SearchAndReturnBlockNumber(fd,starting_block,temp.id);
                if(block_returned<0){
                        printf("Record not found \n");
                        return -1;
                }
                err=BF_ReadBlock(fd,block_returned,&block);
                memcpy(&blockinformation,block,sizeof(blockinformation_t));
                numberofrecords=blockinformation.currentrecords;
                block=block+sizeof(blockinformation_t);
                for(int i=0; i<numberofrecords; i++){
                        memcpy(&record,block,sizeof(Record));
                        if(record.id==*(int *)value){
                                counter=i;
                                break;
                        }
                        block=block+sizeof(Record);
                                
                }
                err=BF_ReadBlock(fd,block_returned,&block);
                block=block+sizeof(blockinformation_t)+counter*sizeof(Record);
                temp.id=-1;     //mark this record because it gets deleted
                memcpy(block,&temp,sizeof(record));
                 err=BF_WriteBlock(fd,block_returned);
                        if(err<0){
                                BF_PrintError("Write block error");
                }
                err=BF_ReadBlock(fd,block_returned,&block);
                block=block+sizeof(blockinformation_t);
               



                


        }
        free(hashtablearray);
        return 0;

}


long int SearchAndReturnBlockNumber(int fd,long int block_number,int id){
        void *block;
        blockinformation_t blockinformation;
        hash_node_t *hashtablearray;
        Record record;
        int flagfound=0,err,numberofrecords=0;
        while(block_number!=-1){
                 err=BF_ReadBlock(fd,block_number,&block);
                 if(err<0){
                                BF_PrintError("Print block");
                                return -1;
                }
                 memcpy(&blockinformation,block,sizeof(blockinformation_t));
                numberofrecords=blockinformation.currentrecords; 
                block=block+sizeof(blockinformation_t);
                for(int i=0; i<maxrecordsperblock; i++){
                        memcpy(&record,block,sizeof(Record));
                        
                        if(record.id==id){
                                flagfound=1;
                                break;                      
                        }
                        block=block+sizeof(Record);
                        
                }
                if(flagfound==1){
                        return block_number;
                }
                block_number=blockinformation.nextblock;

        } 
        if(flagfound==0)
                return -1;       
        

}

int printblocks(int fd,int block_number,void *value,char type){
        void *block;
        Record record;
        SecondaryRecord secondaryrecord;
        long int blocksread=0;
        int err,recordsfound=0,numberofrecords=0;
        if(type=='i'){          //if we are searching for integer
                blockinformation_t blockinformation;
                while(block_number!=-1){
                        err=BF_ReadBlock(fd,block_number,&block);
                        blocksread++;
                        if(err<0){
                                BF_PrintError("Print block");
                                return -1;
                        }
                        memcpy(&blockinformation,block,sizeof(blockinformation_t));
                        numberofrecords=blockinformation.currentrecords;  
                        block_number=blockinformation.nextblock;
                        block=block+sizeof(blockinformation_t);
                        for(int i=0; i<numberofrecords; i++){
                                memcpy(&record,block,sizeof(Record));
                                
                                if(record.id==*(int *)value){
                                       printf("Id %d,Name %s,Surname %s,Adress %s\n",record.id,record.name,record.surname,record.adress);    
                                       recordsfound++;                  
                                }
                                block=block+sizeof(Record);
                                
                                
                        }
                        
                }
        }
        if(type=='s'){
                blockinformation_t blockinformation;
                while(block_number!=-1){
                        err=BF_ReadBlock(fd,block_number,&block);
                        blocksread++;
                        if(err<0){
                                BF_PrintError("Print block");
                                return -1;
                        }
                        memcpy(&blockinformation,block,sizeof(blockinformation_t));
                        numberofrecords=blockinformation.currentrecords;  
                        block_number=blockinformation.nextblock;
                        block=block+sizeof(blockinformation_t);
                        for(int i=0; i<numberofrecords; i++){
                                memcpy(&secondaryrecord,block,sizeof(SecondaryRecord));
                                
                                       if(strcmp(secondaryrecord.key,(char*)value)==0){
                                                printf("Name %s,primary block %ld\n",secondaryrecord.key,secondaryrecord.blockId);    
                                                recordsfound++; 
                                       
                                       }
                                                       
                                
                                block=block+sizeof(SecondaryRecord);
                                
                                
                        }
                        
                }



        }
        printf("Total blocks read %ld\n",blocksread);
        if(recordsfound ==0){
                return 0;
        }else{
                return recordsfound;
        }        
}
int int_hashfunction(Record record,int buckets){  
    int bucketnumber;
    if(record.id<0){
            record.id=record.id*(-1);//for negetive values convert to positive
    }
    bucketnumber=record.id%buckets;
    return bucketnumber;
} 
unsigned long int string_hash(unsigned char *str,long int numberofbuckets)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	hash=hash%numberofbuckets;
    return hash;
}      

int  numberofrecords(char *filename){
         // count the number of lines in the file called filename                                    
        FILE *fp = fopen(filename,"r");
        int ch=0;
        int lines=0;

        if (fp == NULL){
                printf("Error opening file %s\n",filename);
        }
                
        while(!feof(fp))
        {
                ch = fgetc(fp);
                if(ch == '\n')
                {
                        lines++;
                }       
        }
        fclose(fp);
        return lines;
}

void fillrecordsarray(Record **recordsarray,int arraysize,char *filename){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char *token;
    int i=0,turn=0;

    fp = fopen(filename, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        turn=0;    
        token = strtok(line, "{ \" , }");
        while(token){
                
                if(turn==0){
                        recordsarray[i]->id=atoi(token);

                }
                if(turn ==1){
                        strcpy(recordsarray[i]->name,token);
                
                }
                if(turn==2){
                        strcpy(recordsarray[i]->surname,token);
                }
                if(turn==3){
                        strcpy(recordsarray[i]->adress,token);
                }
                
                token=strtok(NULL,"{ \" } , \n");
                turn++;
        }
        i++;
    }
    if(i!=arraysize){
            printf("ERROR fill record array\n");    
    }

    fclose(fp);
    if (line)
        free(line);

}
hash_node_t* returnhashtable(HT_info header_info){
        hash_node_t *hashtablearray;
        int err,fd,ht_size;
        void *block;
        fd=header_info.fileDesc;
        ht_size=header_info.numberofbuckets;
        Record temp,record;
        err=BF_ReadBlock(fd,1,&block);
        if(err<0){
            BF_PrintError("Hello Reading block error");
            return NULL;
        }
        hashtablearray=malloc(ht_size*sizeof(hash_node_t));
        if(hashtablearray==NULL){
                printf("Malloc failed\n");
                return NULL;
        }
        for(int i=0; i<ht_size; i++){
                memcpy(&hashtablearray[i],(hash_node_t*)block,sizeof(hash_node_t));
                block+=sizeof(hash_node_t);
        }
        return hashtablearray;


}