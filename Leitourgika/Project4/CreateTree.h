#ifndef CreateTree_H_
#define CreateTree_H_

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "Structures.h"
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>

//tree
void buildTree(char *path, inodeList_t *inodeList, treeNode_t *root);
treeNode_t *treeRootInit(char *path, inodeList_t *inodeList);
treeNode_t *treeBackupInit(char *path, inodeList_t *inodeListBackUp, treeNode_t *root);
void SortTreeList(treeNode_t **head);
void swapNodes(treeNode_t **head_ref, char *x, char *y);
void printTree(treeNode_t *root, int level);
void printTreeList(treeNodeList_t *treeNodeList);
void destroyTree(treeNode_t *root);
treeNode_t *treeNodeCreate(char *name, char *fullpath, char type, inode_t *inode);
treeNodeList_t *makeTreeNodeList();
void pushToTreeNodeList(treeNodeList_t *treeNodeList, treeNode_t *treeNode);
void destroyTreeNodeList(treeNodeList_t *treeNodeList);
void deletefromTreeNodeList(treeNode_t *treeNode, treeNodeList_t *treeNodeList);
treeNode_t *findTreenode(treeNode_t *root, inode_t *inode, char *Sname);
treeNode_t *findTreenodeOnSameLevel(treeNode_t *root, char *Sname);
void destroyTreeRoot(treeNode_t *root);
//
void createBackup(char *source, char *backup);
void updateSourceINodePointers(treeNode_t *root, treeNode_t *BackUpRoot);
void copyInodePointers(treeNodeList_t *source, treeNodeList_t *backUp);
void syncLevel_2a(treeNode_t *source, treeNode_t *backUp, char *currentsourcePath, char *currentbackUpPath, inodeList_t *backUpinodeList);
void syncTree(treeNode_t *source, treeNode_t *backUp, char *currentsourcePath, char *currentbackUpPath, inodeList_t *backUpinodeList);
void syncLevel_2c(treeNode_t *source, treeNode_t *backUp, char *currentsourcePath, char *currentbackUpPath, inodeList_t *backUpinodeList);
void syncLevel_2d(treeNode_t *source, treeNode_t *backUp, char *currentsourcePath, char *currentbackUpPath, inodeList_t *backUpinodeList);
void syncLevel_2e(treeNode_t *source, treeNode_t *backUp, char *currentsourcePath, char *currentbackUpPath, inodeList_t *backUpinodeList);
void syncLevel_2b(treeNode_t *backUp, treeNode_t *source);
int compareiNodes(inode_t *source, inode_t *backup);

#endif