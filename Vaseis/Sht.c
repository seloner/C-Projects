#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "BF.h"
#include "Sht.h"
#include "Ht.h"

int sec_numberofbuckets=0;
int sec_maxrecordsperblock=0;

int SHT_CreateSecondaryIndex(char *sfilename, char *attrName, int attrLength, int buckets){
	void *block;
    int filecreated=0,sec_hashtablesize=0,err=0,fd=0;
    SHT_info sht_info;
    sec_hash_node_t *sec_hashtablearray;
    sec_numberofbuckets=buckets;
    sec_maxrecordsperblock=(BLOCK_SIZE-sizeof(sec_blockinformation_t))/sizeof(SecondaryRecord); 
    err=BF_CreateFile(sfilename);
    if(err!=0){
            BF_PrintError("File create error");
            return -1;
    }
    fd=BF_OpenFile(sfilename);
    if(fd<0){
            BF_PrintError("File open error");
            return -1;
    }
    err=BF_AllocateBlock(fd);
    if(err<0){
            BF_PrintError("Allocate block error");
    }
    err=BF_ReadBlock(fd,0,&block);
    if(err<0){
            BF_PrintError("Reading block error");
            return -1;
    }
    //arxikopoihsh info gia hash table
    sht_info.numBuckets=sec_numberofbuckets;
    sht_info.attrLength=attrLength;
    strcpy(sht_info.attrName,attrName);
    strcpy(sht_info.fileName,sfilename);
    memcpy(block,&sht_info,sizeof(SHT_info));
    err=BF_WriteBlock(fd,0);
    if(err<0){
            BF_PrintError("Writing block error");
            return -1;
    }
    //Molis arxikopoihthike to prwto block tou arxeiou mas
    err=BF_AllocateBlock(fd);
    if(err<0){
            BF_PrintError("Allocate block error");
    }
     err=BF_ReadBlock(fd,1,&block);
    if(err<0){
            BF_PrintError("Reading block error");
            return -1;
    }
    if((sec_numberofbuckets*sizeof(sec_hash_node_t))>BLOCK_SIZE)
        printf("Error hash table cant fit into one block");
    if((sec_hashtablearray=malloc(sec_numberofbuckets*sizeof(sec_hash_node_t)))==NULL){
            printf("Malloc failed\n");
    }
    for(int i=0; i<sec_numberofbuckets;i++){
            sec_hashtablearray[i].id=i;
            sec_hashtablearray[i].firstblock=-1;
            sec_hashtablearray[i].currentrecords=0;
            sec_hashtablearray[i].lastblock=-1;
    }
     for(int i=0; i<sec_numberofbuckets;i++){
            memcpy(block,&sec_hashtablearray[i],sizeof(sec_hash_node_t));
            block=block+sizeof(sec_hash_node_t);
    }
    err=BF_WriteBlock(fd,1);
    if(err<0){
            BF_PrintError("Writing block error");
            return -1;
    }
    //perasma tou hash table ston disko
    err=BF_CloseFile(fd);
    free(sec_hashtablearray);
    return 0;
}

SHT_info* SHT_OpenSecondaryIndex (char *sfilename){
	int err,fd;
    void *block;
    SHT_info *sht_info;
    sht_info=malloc(sizeof(SHT_info));
       
    fd=BF_OpenFile(sfilename);
    if(fd<0){
        BF_PrintError("File open error");
        return NULL;
        }
         err=BF_ReadBlock(fd,0,&block);
        if(err<0){
            BF_PrintError("Reading block error");
            return NULL;
        }
        memcpy(sht_info,block,sizeof(SHT_info));
        sht_info->fileDesc=fd;
        return sht_info;
}

int SHT_CloseSecondaryIndex (SHT_info* header_info ){
        int fd;
        fd=BF_CloseFile(header_info->fileDesc);
        if(fd<0){
            BF_PrintError("Close file error");
            return -1;
        }else
            return 0;   
}


int SHT_SecondaryInsertEntry(SHT_info sec_infonode,SecondaryRecord sec_record){
	void *block;
    sec_blockinformation_t sec_blockinformation;
    sec_hash_node_t *sec_hashtablearray;
    int flaghchange=0,sht_size,previous,flagstop=0,nextblock,blocknumber,fd,position=-1,err;
    fd=sec_infonode.fileDesc;    
    position=string_hash(sec_record.key,sec_numberofbuckets);
    err=BF_ReadBlock(fd,1,&block);
    if(err<0){
        BF_PrintError("Hello Reading block error");
        return -1;
    }
    sht_size=sec_infonode.numBuckets;
    sec_hashtablearray=malloc(sht_size*sizeof(sec_hash_node_t));
    for(int i=0; i<sht_size; i++){
        memcpy(&sec_hashtablearray[i],(sec_hash_node_t*)block,sizeof(sec_hash_node_t));
        block+=sizeof(sec_hash_node_t);
    }
   
    if(sec_hashtablearray[position].firstblock==-1){    //if no block available
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
        sec_hashtablearray[position].firstblock=blocknumber;
        sec_hashtablearray[position].lastblock=blocknumber;
        if(err<0){
            BF_PrintError("Write block error");
            return -1;
        }
        sec_blockinformation.nextblock=-1;
        sec_blockinformation.currentrecords=1;
        //printf("Allocated the first block\n");
        err=BF_ReadBlock(fd,blocknumber,&block);
        if(err<0){
            BF_PrintError("Reading block error");
            return -1;
        }
        memcpy(block,&sec_blockinformation,sizeof(sec_blockinformation_t));
        block=block + sizeof(sec_blockinformation_t);
        memcpy(block,&sec_record,sizeof(SecondaryRecord));
        err=BF_WriteBlock(fd,blocknumber);
        if(err<0){
            BF_PrintError("Write block error");
            return -1;
        }
    }
	else{
            err=BF_ReadBlock(fd,sec_hashtablearray[position].lastblock,&block);
            if(err<0){
                BF_PrintError("Print error\n");
            }
            memcpy(&sec_blockinformation,block,sizeof(sec_blockinformation_t));
            if(sec_blockinformation.currentrecords<sec_maxrecordsperblock){                         //if the records fits
            	sec_blockinformation.currentrecords++;
            	memcpy(block,&sec_blockinformation,sizeof(sec_blockinformation_t));
           	 	block=block +(sec_blockinformation.currentrecords-1)*sizeof(SecondaryRecord)+sizeof(sec_blockinformation_t);
            	memcpy(block,&sec_record,sizeof(SecondaryRecord));
            	err=BF_WriteBlock(fd,sec_hashtablearray[position].lastblock);
            	if(err<0){
            	    BF_PrintError("Write block error");
            	    return -1;
            	}
			}
			else{
                previous=sec_hashtablearray[position].lastblock;
                err=BF_AllocateBlock(fd);
                if(err<0){
                    BF_PrintError("Allocate block error");
                }
                blocknumber=BF_GetBlockCounter(fd)-1;
                sec_blockinformation.nextblock=blocknumber;
                memcpy(block,&sec_blockinformation,sizeof(sec_blockinformation_t));
                sec_hashtablearray[position].lastblock=blocknumber;
                err=BF_WriteBlock(fd,previous);
                if(err<0){
                    BF_PrintError("Write block error");
                }
                err=BF_ReadBlock(fd,blocknumber,&block);
            	if(err<0){
                    BF_PrintError("Reading block error");
                	return -1;
                }
                sec_blockinformation.currentrecords=1;
                sec_blockinformation.nextblock=-1;
                memcpy(block,&sec_blockinformation,sizeof(sec_blockinformation_t));
                block=block+sizeof(sec_blockinformation_t);
                memcpy(block,&sec_record,sizeof(SecondaryRecord));
                err=BF_WriteBlock(fd,blocknumber);
                if(err<0){
                    BF_PrintError("Write block error");
                    return -1;
                }
			}
    }                
    BF_ReadBlock(fd, 1, &block);
    for ( int i=0 ; i<sht_size ; i++ ) {
        memcpy(block, &sec_hashtablearray[i], sizeof(sec_hash_node_t)) ; 
        block += sizeof(sec_hash_node_t);
    }
    BF_WriteBlock(fd, 1) ;
    free(sec_hashtablearray);
    return 0;
}

int SHT_SecondaryGetAllEntries(SHT_info sec_header_info,void *value){
        int flagfound=0,result=0;
        int position=-1,*firstblockarray,sht_size,err,fd=sec_header_info.fileDesc;
        sec_hash_node_t *sec_hashtablearray;
        void *block;
        err=BF_ReadBlock(fd,1,&block);
        if(err<0){
            BF_PrintError("Reading block error");
            return -1;
        }
        sht_size=sec_header_info.numBuckets;
        firstblockarray=malloc(sizeof(int)*sht_size);
        sec_hashtablearray=malloc(sht_size*sizeof(sec_hash_node_t));
        for(int i=0;i<sht_size; i++){
            memcpy(&sec_hashtablearray[i],block,sizeof(sec_hash_node_t));
            firstblockarray[i]=sec_hashtablearray[i].firstblock;
            block=block+sizeof(sec_hash_node_t);
        }
       
            position=string_hash((char*)value, sec_header_info.numBuckets);
            result=printblocks(fd,firstblockarray[position],value,'s');
        
        if(result>0)
            return result;
        else    
            return -1;        
}

