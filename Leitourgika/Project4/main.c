#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include "Structures.h"
#include "CreateTree.h"
#include "InotifyFunctions.h"
extern volatile int endflag;
volatile int fd;


int main(int argc, char const *argv[])
{
    

   
    DIR *dir;
    int dir_result;
    treeNode_t *result,*backupRoot=NULL,*root=NULL,*searchNode=NULL;
    inodeList_t *inodeListBackUp,*inodeList;
    char *ptr,sourcePath[100],backUpPath[100];
    if(argc<3){
        printf("./mirr source backup\n");
        return 0;
    }
    ptr=realpath(argv[1],sourcePath);
    if(ptr==NULL){
        perror("Realpath");
        return -1;
    }
    
    dir = opendir(argv[2]);
    if (dir)        //if backup exists
    {  
        closedir(dir);
    }
    else if (ENOENT == errno)
    {
       dir_result = mkdir(argv[2], 0755);       //directory backup not exist
       if(dir_result != 0 ){ 
           if(dir_result!=0){
                printf("mdkir errorr\n");
                return -1;
            }
       }                 
    }   
    ptr=realpath(argv[2],backUpPath );
    if(ptr==NULL){
        perror("Realpath");
        return -1;
    }

    inodeList=makeInodeList();
    root=treeRootInit(sourcePath,inodeList);
    if(root==NULL){
        printf("Cant init tree\n");
        return -1;
    }
    buildTree(sourcePath,inodeList,root);

    inodeListBackUp=makeInodeList();
    backupRoot=treeRootInit(backUpPath,inodeListBackUp);
    if(backupRoot==NULL){
        printf("Cant init back up tree\n");
        return -1;
    }
    
    root->inodeP->inodePBackUp = backupRoot->inodeP;

    buildTree(backUpPath,inodeListBackUp,backupRoot);
    syncTree(root,backupRoot,sourcePath,backUpPath,inodeListBackUp); 

    dirList_t *nodelist;
    dirList_t *nodelistB; 
    // while(endflag==0){
        
        nodelist = dirNodeListInit();
        nodelistB = dirNodeListInit();
        fun(nodelist, nodelistB, root,backupRoot, inodeListBackUp,inodeList);
        

    // }
    printf("fd main %d\n",fd );
    // printf("vghkaaaaaaa\n");
    printTree(backupRoot,0);
    printTree(root,0);
     destroyInodeList(inodeList);
     destroyTree(root);
    destroyTreeRoot(root);
    
    destroyInodeList(inodeListBackUp);
    destroyTree(backupRoot);
    destroyTreeRoot(backupRoot);
    
    removeWatch(fd, nodelist);
    removeWatch(fd, nodelistB);
    close(fd);

    deletedirNodeList(nodelist);
    deletedirNodeList(nodelistB);
    free(nodelist);
    free(nodelistB);
}
