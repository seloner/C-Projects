#ifndef Structures_H_
#define Structures_H_

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
typedef struct treeNode treeNode_t;
typedef struct inode inode_t ;
typedef struct inodeListNnode inodeListNnode_t;
typedef struct stringNode stringNode_t;

typedef struct stringNode{
	char *string;
	stringNode_t *next;
}stringNode_t;

typedef struct stringsList{
	stringNode_t *head;
}stringsList_t;

typedef struct inode{	//komvos inode
	unsigned int inodeId;
	time_t lastChange;
	long fileSize;
	stringsList_t *stringsList;
	int namesCount;
	inode_t *inodePBackUp;
} inode_t;


typedef struct treeNodeList{  //
	treeNode_t *head;
	int numOfNodes;  //prepei na to afxanw kathe fora p kanw insert
} treeNodeList_t;


typedef struct treeNode{	//komvos tou dentrou
	char *Sname;
	char type;
	inode_t *inodeP;
	treeNode_t *next;
	treeNodeList_t *list;
	char *fullpath;
} treeNode_t;

typedef struct inodeListNnode{		//komvos tis listas apo inodes
	unsigned int inodeId;
	treeNode_t *treeNode;
	inodeListNnode_t *next;
}inodeListNnode_t;

typedef struct inodeList{	//lista apo inodes(voithiiki)
	inodeListNnode_t *head;
}inodeList_t;
//tree functions----------------//


//-----------------------------//

//inode functions-------------//
//
inodeListNnode_t* inodeListNnodeCreate(unsigned int inodeId,treeNode_t *treeNode);
inodeList_t* makeInodeList();
void pushToInodeList(inodeList_t *inodeList,unsigned int inodeId,treeNode_t *treeNode);
void deleteFromInodeList(inodeList_t *inodeList,unsigned int inodeId);
void destroyInodeList(inodeList_t *inodeList);
void printInodeList(inodeList_t *inodeList);
inode_t* inodeCreate(unsigned int inodeId,long fileSize,time_t lastChange,int namesCount);
inode_t* inodeSearch(inodeList_t *inodeList,unsigned int inodeId);

//-----------------------------//
//strings list functions-------//
stringsList_t* makeStringsList();
void destroyStringList(stringsList_t *stringsList);
void pushToStringsList(stringsList_t *stringsList,stringNode_t *stringNode);
void deleteFromStringList(stringsList_t *stringsList,char  *string);
stringNode_t* createStringNode(char *string);
void printStringsList(stringsList_t *stringsList);
//-----------------------------//
#endif
