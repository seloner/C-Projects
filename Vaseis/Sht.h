#ifndef Sht_H_
#define Sht_H_

#include "Ht.h"

typedef struct SHT_info {
	int fileDesc; 
	char attrName[25]; 
	int attrLength; 
	long int numBuckets; 
	char fileName[50]; 
}SHT_info;

typedef struct sec_hash_node_t{
    int id;
    int firstblock;
    int lastblock;
    int currentrecords;
}sec_hash_node_t;

typedef struct sec_blockinformation{
    int currentrecords;
    int nextblock;
}sec_blockinformation_t;

typedef struct sec_record{
    char key[40];
	long int blockId;
}SecondaryRecord;



int SHT_CreateSecondaryIndex(char *filename, char *attrName, int attrLength, int buckets);
SHT_info* SHT_OpenSecondaryIndex (char *sfilename);
int SHT_CloseSecondaryIndex (SHT_info* header_info );
int SHT_SecondaryInsertEntry(SHT_info header_info, SecondaryRecord record);
int SHT_SecondaryGetAllEntries(SHT_info sec_header_info,void *value);
#endif
