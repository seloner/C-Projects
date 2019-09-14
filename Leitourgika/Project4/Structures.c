#include "Structures.h"
//inode functions------------------------------//


inode_t* inodeCreate(unsigned int inodeId,long fileSize,time_t lastChange,int namesCount){
    inode_t *inode;
    inode=malloc(sizeof(inode_t));
    if(inode==NULL){
        printf("Malloc error\n");
        return NULL;
    }
    inode->inodeId=inodeId;
    inode->fileSize=fileSize;
    inode->lastChange=lastChange;
    inode->namesCount=namesCount;
    inode->stringsList=makeStringsList();
    inode->inodePBackUp=NULL;


}
inodeList_t* makeInodeList(){
    inodeList_t *inodeList=malloc(sizeof(inodeList_t));
    if(inodeList==NULL){
        printf("Malloc error\n");
        return NULL;
    }
    inodeList->head=NULL;
    return inodeList;
}
inode_t* inodeSearch(inodeList_t *inodeList,unsigned int inodeId){
    int flagfound=0;
    inodeListNnode_t *current;
    if(inodeList->head==NULL){      //List empty
        return NULL;
    }else{
        current=inodeList->head;
        while(current){
            if(current->inodeId==inodeId){   //if the inodeId exists
                flagfound=1;
                break;
            }
            current=current->next;  
        }
        if(flagfound==1){
            return current->treeNode->inodeP;
        }else{
            return NULL;
        }
    }




}

void pushToInodeList(inodeList_t *inodeList,unsigned int inodeId,treeNode_t *treeNode){
    inodeListNnode_t *nodeToInsert,*current;
    nodeToInsert=inodeListNnodeCreate(inodeId,treeNode);
    if(nodeToInsert==NULL){
        printf("Node Creation Error\n");
        return;
    }
    if(inodeList->head==NULL){  //first node to insert 
        inodeList->head=nodeToInsert;
        
    }else{
        current=inodeList->head;
        while(current->next!=NULL){
            current=current->next;
        }
        current->next=nodeToInsert;
    }
    //printf("Added %d ,treenode %s\n",nodeToInsert->inodeId,nodeToInsert->treeNode->Sname);

}
void deleteFromInodeList(inodeList_t *inodeList,unsigned int inodeId){
    inodeListNnode_t *previous,*current;
    current=inodeList->head;
    previous=current;
    while(current){
        if(current->inodeId==inodeId){
            previous->next=current->next;
            if(current==inodeList->head){
                inodeList->head=current->next;
            }
            free(current);
            return;
        }
        previous=current;
        current=current->next;
    }

}
void destroyInodeList(inodeList_t *inodeList){
    inodeListNnode_t *next,*current;
    current=inodeList->head;
    next=current;
    while(current){
        next=current->next;
       // printf("Deleting %d \n",current->inodeId);
        free(current);
        current=next;
    }
    free(inodeList);
}

void printInodeList(inodeList_t *inodeList){
    inodeListNnode_t *current;
    current=inodeList->head;
    // if(current){
    //     printf("Exists\n");
    // }
    while(current!=NULL){
        printf("%s \n",current->treeNode->inodeP->stringsList->head->string);
        printf("%s \n",current->treeNode->inodeP->inodePBackUp->stringsList->head->string);
        //printStringsList(current->treeNode->inodeP->stringsList);
        current=current->next;
    }
}

inodeListNnode_t* inodeListNnodeCreate(unsigned int inodeId,treeNode_t *treeNode){
     inodeListNnode_t *inodeListNnode;
     inodeListNnode=malloc(sizeof(inodeListNnode_t));
     if(inodeListNnode==NULL){
         printf("Malloc error");
         return NULL;
     }
     inodeListNnode->inodeId=inodeId;
     inodeListNnode->treeNode=treeNode;
     inodeListNnode->next=NULL;

}

//-------------------------------------//




//strings list functions------------------------//
stringsList_t* makeStringsList(){
    stringsList_t *stringNodeList=malloc(sizeof(stringsList_t));
    if(stringNodeList==NULL){
        printf("Malloc error\n");
        return NULL;
    }
    stringNodeList->head=NULL;
    return stringNodeList;


}
stringNode_t* createStringNode(char *string){
    stringNode_t *stringNode;
    stringNode=malloc(sizeof(stringNode_t));
    if(stringNode==NULL){
        printf("Malloc error\n");
        return NULL;
    }
    stringNode->string=malloc((strlen(string)+1)*sizeof(char));
    if(stringNode->string==NULL){
        printf("Malloc error\n");
        return NULL;
    }
    strcpy(stringNode->string,string);
    stringNode->next=NULL;
    return stringNode;
}

void pushToStringsList(stringsList_t *stringsList,stringNode_t *stringNode){
    stringNode_t *current=NULL;
    if(stringsList->head==NULL){  //first node to insert 
        stringsList->head=stringNode;
        
    }else{
        current=stringsList->head;
        while(current->next!=NULL){
            current=current->next;
        }
        current->next=stringNode;
    }
   

}
void destroyStringList(stringsList_t *stringsList){
    stringNode_t *next,*current;
    current=stringsList->head;
    next=current;
    while(current){
        next=current->next;
        free(current->string);
        free(current);
        current=next;
    }
    free(stringsList);

}

void printStringsList(stringsList_t *stringsList){
    stringNode_t *current;
    if(stringsList->head==NULL)
        return;
    current=stringsList->head;
    while(current){
        printf("%s\n",current->string);
        current=current->next;
    }    
}

void deleteFromStringList(stringsList_t *stringsList,char *string){
    stringNode_t *current, *previous;
    current=stringsList->head;
    previous=current;
    while(current){
        if(strcmp(current->string,string)==0){
            previous->next=current->next;
            if(current==stringsList->head){
                stringsList->head=current->next;
            }
            free(current->string);
            free(current);
            return;
        }
        previous=current;
        current=current->next;
    }
}

//---------------------------------------------//