#ifndef Ht_H_
#define Ht_H_
typedef struct HT_info{
    char attrType;
    int attrLength;
    char attrName[20];
    long int numberofbuckets;
    int fileDesc;
}HT_info;
typedef struct record{
    int id;
    char name[15];
    char surname[20];
    char adress[40];
}Record;

typedef struct hash_node_t{
    int id;
    int firstblock;
    int lastblock;
    int currentrecords;

}hash_node_t;


typedef struct blockinformation{
    int currentrecords;
    int nextblock;
}blockinformation_t;

int HT_CreateIndex(char *filename,char attrType,char *attrName,int attrLength,int buckets);

int int_hashfunction(Record record,int buckets);
HT_info* HT_OpenIndex(char *filename);
int HT_CloseIndex(HT_info *info);
int HT_InsertEntry(HT_info infonode,Record record);
int Ht_DeleteEntry(HT_info header_info,void *value);
int  numberofrecords(char *filename);
void fillrecordsarray(Record **recordsarray,int arraysize,char *filename);
int printblocks(int fd,int block_number,void *value,char type);
int HT_GetAllEntries(HT_info header_info,void *value);
unsigned long int string_hash(unsigned char *str,long int numberofbuckets);  
long int SearchAndReturnBlockNumber(int fd,long int block_number,int id);
hash_node_t* returnhashtable(HT_info header_info);      



#endif