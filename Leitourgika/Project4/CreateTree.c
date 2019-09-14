#include "CreateTree.h"

void buildTree(char * path,inodeList_t *inodeList,treeNode_t *root)
{

  DIR *d;
  treeNode_t *treeNode;
  inode_t *inode;
  char pathtocheckeveryfile[200];
  struct dirent *dir;
  struct stat statbuf;
  stringNode_t *stringNode;
  d = opendir(path);
  if (d) {

    while ((dir = readdir(d)) != NULL) {
      if(strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0){      //ignore . .. names
         
         strcpy(pathtocheckeveryfile,path);
         strcat(pathtocheckeveryfile,"/");
         strcat(pathtocheckeveryfile,dir->d_name);        //create the path name for the file
         stat(pathtocheckeveryfile, &statbuf);
        inode=inodeSearch(inodeList,dir->d_ino);                 //search if we have hard link
        stringNode=createStringNode(pathtocheckeveryfile);       //create the string node
        if(stringNode==NULL)
          return;
        if(inode!=NULL){                                    //if inode exists we need to add the information for this file too
            inode->namesCount++;                            //increase name count
            inode->fileSize=statbuf.st_size;                //update file size
            inode->lastChange=statbuf.st_mtime;             //update mod time
            pushToInodeList(inodeList,inode->inodeId,treeNode);
        }else{

          inode=inodeCreate(statbuf.st_ino,statbuf.st_size,statbuf.st_mtime,1);
            if(inode==NULL){
              return;
            }
          
          }  
          pushToStringsList(inode->stringsList,stringNode);
          if(S_ISREG(statbuf.st_mode)==0){                       //if we found a folder

                 treeNode=treeNodeCreate(dir->d_name,stringNode->string,'d',inode);                   //then create dir type inode           
                                        
          }else{
            treeNode=treeNodeCreate(dir->d_name,stringNode->string,'f',inode);
          }   
          if(treeNode==NULL)
            return;
          pushToInodeList(inodeList,inode->inodeId,treeNode);      //push the inode into the list
          pushToTreeNodeList(root->list,treeNode);                  //push the treenode into the list
          SortTreeList(&root->list->head);                          //sort the name of files
          if(S_ISREG(statbuf.st_mode)==0){                       //if we found a folder we need to recursive search again

                
                 buildTree (pathtocheckeveryfile,inodeList,treeNode);                             //then is folder so we need to check inside this folder too
          }
            
          
          



        
        
        
      }
    }
    closedir(d);
  }
}    

treeNode_t* treeRootInit(char *path,inodeList_t *inodeList){
    char *c_time_string,command[300],foldername[50];              //command to get the name of the folder without the full path
    inode_t *inode;
    struct stat buf; 
    treeNode_t *treeNode;
    stringNode_t *stringNode;
    stat(path,&buf);                                    //get information abour root folder
    strcpy(foldername,basename(path));
    inode=inodeCreate(buf.st_ino,buf.st_size,buf.st_mtime,0);    //create the inode
    if(inode==NULL){
      printf("Cant create inode\n");
      return NULL;
    }
    
    stringNode=createStringNode(path);
    if(stringNode==NULL)
      return NULL;
    inode->namesCount++;
    pushToStringsList(inode->stringsList,stringNode);
    treeNode=treeNodeCreate(foldername,path,'d',inode);    //create the tree node
    if(treeNode==NULL){
      printf("Cant create treNode\n");
      return  NULL;
    }
    pushToInodeList(inodeList,buf.st_ino,treeNode);       //push to the inode list
    return treeNode;
    
    
}

//tree functions------------------------//
treeNodeList_t* makeTreeNodeList(){
     treeNodeList_t *treeNodeList=malloc(sizeof(treeNodeList_t));
    if(treeNodeList==NULL){
        printf("Malloc error\n");
        return NULL;
    }
    treeNodeList->head=NULL;
    return treeNodeList;


}
void destroyTreeNodeList(treeNodeList_t *treeNodeList){
    treeNode_t  *next,*current;
    current=treeNodeList->head;
    next=current;
    while(current){
        next=current->next;
        if(current->inodeP->namesCount==1){   //if we only have one file then we need to delete the inode
          destroyStringList(current->inodeP->stringsList);
          free(current->inodeP);
        }else{
          
          current->inodeP->namesCount--;
          deleteFromStringList(current->inodeP->stringsList,current->fullpath);
          

        }
        free(current->fullpath);
        free(current->Sname);
        free(current);
        current=next;
    }
   
    free(treeNodeList);

}
void pushToTreeNodeList(treeNodeList_t *treeNodeList,treeNode_t *treeNode){

    treeNode_t *current=NULL;
    if(treeNodeList->head==NULL){  //first node to insert 
        treeNodeList->head=treeNode;
        
    }else{
        current=treeNodeList->head;
        while(current->next!=NULL){
            current=current->next;
        }
        current->next=treeNode;
    }
    //  printf("Added %s type %c\n",treeNode->Sname,treeNode->type);
}

void deletefromTreeNodeList(treeNode_t *treenode,treeNodeList_t *treeNodeList){
     treeNode_t *previous,*current;
    current=treeNodeList->head;
    previous=current;
    while(current){
        if(current==treenode){
            previous->next=current->next;
            if(current==treeNodeList->head){
                treeNodeList->head=current->next;
            }
            destroyTree(treenode);
            if(current->inodeP->namesCount==1){   //if we only have one file then we need to delete the inode
                 destroyStringList(current->inodeP->stringsList);
                 free(current->inodeP);
            }else{
                current->inodeP->namesCount--;
                deleteFromStringList(current->inodeP->stringsList,current->fullpath);
             }
            free(current->fullpath);
            free(current->Sname);
            free(current);
            return;
        }
        previous=current;
        current=current->next;
    }

}
treeNode_t* treeNodeCreate(char *name,char *fullpath,char type,inode_t *inode){
    treeNode_t *treeNode=NULL;
    treeNode=malloc(sizeof(treeNode_t));
    if(treeNode==NULL){
        printf("Malloc error\n");
        return NULL;
    }
    treeNode->Sname=malloc((strlen(name)+1)*sizeof(char));
    if(treeNode->Sname==NULL){
        printf("Malloc error\n");
        return NULL;
    }
    treeNode->fullpath=malloc((strlen(fullpath)+1)*sizeof(char));
    if(treeNode->Sname==NULL){
        printf("Malloc error\n");
        return NULL;
    }
    strcpy(treeNode->Sname,name);
     strcpy(treeNode->fullpath,fullpath);
    treeNode->inodeP=inode;
    treeNode->list=makeTreeNodeList();
    treeNode->type=type;
    treeNode->next=NULL;

}
treeNode_t* findTreenode(treeNode_t *root,inode_t *inode,char *Sname){
    treeNode_t *result=NULL,*current=NULL;
   
    current=root;
    if(current==NULL)           //if root is null
        return NULL;
    if(strcmp(current->Sname,Sname)==0){  //check if it is root
                return current;
    }
    
    result=findTreenode(current->next,inode,Sname); //search for the sibling and save result
    if(result==NULL){                               //if is not the sibling
        result=findTreenode(root->list->head,inode,Sname);  //search for the first child
    }
    return result;    
}
treeNode_t* findTreenodeOnSameLevel(treeNode_t *root,char *Sname){
    treeNode_t *result=NULL,*current=NULL;
    current=root;
    if(current==NULL)           //if root is null
        return NULL;
    if(strcmp(current->Sname,Sname)==0){  //check if it is root
                return current;
    }
    result=findTreenodeOnSameLevel(current->next,Sname); //search for the sibling and save result
    return result;

} 

void destroyTree(treeNode_t *root){
  treeNode_t *child,*parent;
  parent=root;
  if(parent==NULL)
    return;
   child=parent->list->head;
  
     while(child){
       destroyTree(child);
       child=child->next;
     }
     destroyTreeNodeList(parent->list);
}


void destroyTreeRoot(treeNode_t *root){
  destroyStringList(root->inodeP->stringsList);
  free(root->inodeP);
  free(root->Sname);
  free(root->fullpath);
  free(root);
}
void printTree(treeNode_t *root,int level){
  treeNode_t *current;
  int filelevel=level+1;
  if(root==NULL)
    return;
  if(level>0)  
    printf("%*s",level, "\t");
  printf("\n%s------>\n",root->Sname);
  current=root->list->head;
  if(current==NULL){
    return;
  }
  while(current!=NULL){
      printf("%*s", filelevel, "\t");
      printf("%s(%c)",current->Sname,current->type);
    
    if(current->type=='d'){
      printTree(current,level+1); 
    } 
    current=current->next;  
  }
  printf("\n");
}


void updateSourceINodePointers(treeNode_t *root,treeNode_t *BackUpRoot){
    
    if(root->list->head && BackUpRoot->list->head){
      updateSourceINodePointers(root->list->head,BackUpRoot->list->head);
      copyInodePointers(root->list,BackUpRoot->list);

    }
    
}

void copyInodePointers(treeNodeList_t *source,treeNodeList_t *backUp){
  treeNode_t *sourceChild=source->head,*backUpChild=backUp->head;

  while(sourceChild && backUpChild){
      sourceChild->inodeP->inodePBackUp=backUpChild->inodeP;
      sourceChild=sourceChild->next;
      backUpChild=backUpChild->next;
  }
}
void syncTree(treeNode_t *source,treeNode_t *backUp,char *currentsourcePath,char *currentbackUpPath,inodeList_t *backUpinodeList){
    treeNode_t *currentSource,*currentBackup=NULL;
    char *newSourcePath,*newBackupPath;
    if(source==NULL)
      return; 
    //sync 2a stage
    syncLevel_2a(source,backUp,currentsourcePath,currentbackUpPath,backUpinodeList);
    syncLevel_2c(source,backUp,currentsourcePath,currentbackUpPath,backUpinodeList);
    syncLevel_2d(source,backUp,currentsourcePath,currentbackUpPath,backUpinodeList);
    syncLevel_2e(source,backUp,currentsourcePath,currentbackUpPath,backUpinodeList);
    //SortTreeList(backUp);
    
    currentSource=source->list->head;

    while(currentSource){
        if(currentSource->type=='d' && currentSource->list->head!=NULL){
          currentBackup=findTreenodeOnSameLevel(backUp->list->head,currentSource->Sname);
          if(currentBackup==NULL){
            printf("Something is wrong \n");
          }
          newSourcePath=malloc(sizeof(char)*(strlen(currentsourcePath)+strlen(currentSource->Sname)+2));
          newBackupPath=malloc(sizeof(char)*(strlen(currentbackUpPath)+strlen(currentBackup->Sname)+2));
          if(newBackupPath==NULL || newBackupPath==NULL){
            printf("Malloc error\n");
            return;
          }
          strcpy(newSourcePath,currentsourcePath);
          strcat(newSourcePath,"/");
          strcat(newSourcePath,currentSource->Sname);  
          strcpy(newBackupPath,currentbackUpPath);
          strcat(newBackupPath,"/");
          strcat(newBackupPath,currentBackup->Sname);
          
          if(currentSource){
              if(currentSource->list->head!=NULL){
                
                syncTree(currentSource,currentBackup,newSourcePath,newBackupPath,backUpinodeList);
              }
          }
          free(newSourcePath);
          free(newBackupPath); 
      } 
        currentSource=currentSource->next;
       
       
             
    }   
}


void syncLevel_2a(treeNode_t *source,treeNode_t *backUp,char *currentsourcePath,char *currentbackUpPath,inodeList_t *backUpinodeList){
  treeNode_t *treeNode,*currentSource=NULL,*currentBackUp=NULL ,*result=NULL;
  inode_t *inode;
  stringNode_t *stringNode;
  
  int err=0,flagchange=0;
  struct stat buf;
  char *tempstring,*updatedSourcePath,*UpdatedBackUpPath,fileName[500],command[100]="rm ";
  if(source)        //if source exists get the first node
    currentSource=source->list->head;
  else 
      return;  
  if(backUp)       //if back up exists get the first node 
   currentBackUp=(backUp)->list->head;
  
  while(currentSource){       //loop through all nodes
    if(currentSource->type=='d'){   //for every directory in the source folder
      if(currentBackUp)   //when back up has elements inside
        result=findTreenodeOnSameLevel(currentBackUp,currentSource->Sname); //search for the name in the backup
      if(result!=NULL){ 
         if(result->type=='f'){     //if a file holds the name then we need to delete it
            strcat(command,currentbackUpPath);
            strcat(command,"/");
            strcat(command,result->Sname);
            err=system(command);
            if(err==-1){
              printf("Error %s\n",command);
            return;
            }
            deleteFromInodeList(backUpinodeList,result->inodeP->inodeId);
            deletefromTreeNodeList(result,backUp->list); 
            tempstring=malloc(sizeof(char)*(strlen("mkdir  ")+strlen(currentbackUpPath)+strlen(currentSource->Sname)+3)); //now create the needed folder name
            strcpy(tempstring,"mkdir ");
            strcat(tempstring,currentbackUpPath);
            strcat(tempstring,"/");
            strcat(tempstring,currentSource->Sname);
            err=system(tempstring);    //create the folder
            if(err<0){
                printf("%s",tempstring);
                return;
              }
              stat(tempstring,&buf);
            stringNode=createStringNode(tempstring);  
            inode=inodeCreate(buf.st_ino,currentSource->inodeP->fileSize,currentSource->inodeP->lastChange,1);   //create the inode
            pushToStringsList(inode->stringsList,stringNode);
            treeNode=treeNodeCreate(currentSource->Sname,tempstring,'d',inode); //create the tree node
            pushToTreeNodeList(backUp->list,treeNode);     //push to the tree
            pushToInodeList(backUpinodeList,buf.st_ino,treeNode); //push to the inode list
            currentSource->inodeP->inodePBackUp=inode;    //link the backup
            free(tempstring);
         }   
         else{
              if(currentSource->inodeP->inodePBackUp!=result->inodeP){ //check if the link is wrong
                  currentSource->inodeP->inodePBackUp=result->inodeP;
              }
         }
      }else{                                            //NULL result means this name doesnt exist
        //create the name of the folder
        strcpy(fileName,"mkdir ");
        strcat(fileName,currentbackUpPath);
        strcat(fileName,"/");
        strcat(fileName,currentSource->Sname);
         err=system(fileName);
        if(err<0){
          printf("%s",fileName);
          return;
        }
        strcpy(fileName,currentbackUpPath);
        strcat(fileName,"/");
        strcat(fileName,currentSource->Sname);
        //
        stat(fileName,&buf);
        inode=inodeCreate(buf.st_ino,buf.st_size,buf.st_mtime,1); //create the inode
        if(inode==NULL){
          printf("Inode create error\n");
          return;
        }
        stringNode=createStringNode(fileName);      //create the stringnode
        if(stringNode==NULL)
          return ;
        pushToStringsList(inode->stringsList,stringNode);   //push to string list
        treeNode=treeNodeCreate(currentSource->Sname,stringNode->string,'d',inode);   //create the treenode
        if(treeNode==NULL){
          printf("Cant create treNode\n");
          return  ;
        }
        currentSource->inodeP->inodePBackUp=inode;        //link the inodes
        pushToInodeList(backUpinodeList,buf.st_ino,treeNode);   //push inode to the list
        pushToTreeNodeList(backUp->list,treeNode);           //push treenode to the tree
      }
    } 
      currentBackUp=(backUp)->list->head;
      currentSource=currentSource->next;
      strcpy(fileName,"mkdir  ");
      strcpy(command,"rm ");
      result=NULL;
  }
  SortTreeList(&source->list->head);    //do sort
  //now we need to delete every extra folder in the backup;
  currentBackUp=(backUp)->list->head;
  source->list->head;
  while(currentBackUp){
      if(currentBackUp->type=='d'){   //for every folder in the backup search if we have something same in the source
          result=findTreenodeOnSameLevel(source->list->head,currentBackUp->Sname);
          if(result==NULL){           //we found something extra now we need to delete it
            flagchange=1;
              char command[200]="rm -rf ";
              strcat(command,currentBackUp->fullpath);
              err=system(command);
              if(err<0){
                printf("%s error\n",command);
                return;
              }
              treeNode_t *next;
              next=currentBackUp->next;
              deleteFromInodeList(backUpinodeList,currentBackUp->inodeP->inodeId);
              deletefromTreeNodeList(currentBackUp,backUp->list);
              currentBackUp=next;
              
          } 
      }
      if(flagchange==0)
        currentBackUp=currentBackUp->next;
      result=NULL;
      flagchange=0;
  }
  currentBackUp=(backUp)->list->head;
  if(currentBackUp)
      SortTreeList(&currentBackUp->list->head);
}
void syncLevel_2b(treeNode_t *backUp,treeNode_t *source){
  if(backUp==NULL || source ==NULL)
    return; 
  //printTreeList(backUp->list);
}
void syncLevel_2c(treeNode_t *source,treeNode_t *backUp,char *currentsourcePath,char *currentbackUpPath,inodeList_t *backUpinodeList){
    treeNode_t *treeNode,*currentSource=NULL,*currentBackUp=NULL ,*result=NULL;
    char command[400]="cp -p ";
    inode_t *inode;
    stringNode_t *stringNode;
    struct stat buf;
      if(source)        //if source exists get the first node
         currentSource=source->list->head;
      else 
         return;
      if(backUp)          //if backup exists get the first node
        currentBackUp=backUp->list->head;
      else 
          return;  
    
    while(currentSource){     //for every source node
          if(currentSource->type=='f'){  //for every file check
            if(currentBackUp){            //if backup has at least one node
              result=(findTreenodeOnSameLevel(currentBackUp,currentSource->Sname));      //search if the sname exists
            } 
          
            if(result==NULL){     //if the file doesnt exist in the backup
                       strcat(command,currentSource->fullpath);       //create the cp command to copy the file
                       strcat(command," ");
                       strcat(command,currentbackUpPath);
                       strcat(command,"/");
                       strcat(command,currentSource->Sname);
                       system(command);
                       strcpy(command,currentbackUpPath);     //creating full path for the file
                       strcat(command,"/");
                       strcat(command,currentSource->Sname);
                       stat(command,&buf);  //get information about the file
                       stringNode=createStringNode(command);
                       if(stringNode==NULL){
                         printf("Malloc stringnode eror\n");
                         return;
                       }
                      inode=inodeCreate(buf.st_ino,currentSource->inodeP->fileSize,currentSource->inodeP->lastChange,1);
                      if(inode==NULL){
                          printf("Malloc inode error\n");
                          return;
                      }
                      pushToStringsList(inode->stringsList,stringNode);
                       treeNode=treeNodeCreate(currentSource->Sname,command,'f',inode);
                       pushToTreeNodeList(backUp->list,treeNode);
                       pushToInodeList(backUpinodeList,inode->inodeId,treeNode);

            }   
        }
        strcpy(command,"cp -p ");
        currentSource=currentSource->next;
      }


}
void syncLevel_2d(treeNode_t *source,treeNode_t *backUp,char *currentsourcePath,char *currentbackUpPath,inodeList_t *backUpinodeList){
  treeNode_t *treeNode,*currentSource=NULL,*currentBackUp=NULL ,*next,*result=NULL;
  inode_t *inode;
  char command[200]="rm -f ";
  int flagdeleted=0;
  if(backUp==NULL){
    return;
  }
  if(source==NULL){
    return;
  }
  currentBackUp=backUp->list->head;
  while(currentBackUp){     //for every node in the backup
    if(currentBackUp->type=='f'){   // if it as file
      result=findTreenodeOnSameLevel(source->list->head,currentBackUp->Sname);    //find if we  have copy in the source
      if(result==NULL){     //if we dont have a copy in the source folder
        printf("Null\n");
        next=currentBackUp->next;   //save the next 
         strcat(command,currentBackUp->fullpath); //delete the file
         system(command);
        deletefromTreeNodeList(currentBackUp,backUp->list); //delete it from tree
        flagdeleted=1;    //mark that we have change
        currentBackUp=next; 
      }
    }
    if(flagdeleted==0)    //if we didnt have a delete update next
     currentBackUp=currentBackUp->next;
    flagdeleted=0; 
    result=NULL;
    strcpy(command,"rm -rf "); //init command
  }

}
void syncLevel_2e(treeNode_t *source,treeNode_t *backUp,char *currentsourcePath,char *currentbackUpPath,inodeList_t *backUpinodeList){
          
           treeNode_t *treeNode,*currentSource=NULL,*currentBackUp=NULL ,*next,*result=NULL;
          if(source==NULL)
              return;
          if(backUp==NULL)
            return;
          currentSource=source->list->head;       //get the first source node
          currentBackUp=backUp->list->head;     //get the first back up node
          while(currentSource){               //for every node in the source list
              if(currentSource->type=='f'){     //if it is a file
                if(backUp){   //if backup exists
                  result=findTreenodeOnSameLevel(currentBackUp,currentSource->Sname); //try to find it in the backup list
                  if(result){             //if you found it
                      if(result->type=='f'){      //make sure its a file
                             int diff=compareiNodes(currentSource->inodeP,result ->inodeP);    //compare the files
                             if(diff==0) {    //if the files are the same 
                                  currentSource->inodeP->inodePBackUp=result->inodeP;
                             }else{
                                    char command[500]="rm ";
                                    strcat(command,result->fullpath);
                                    system(command);
                                   deletefromTreeNodeList(result,backUp->list);
                                    syncLevel_2c(source,backUp,currentsourcePath,currentbackUpPath,backUpinodeList);
                                    strcpy(command,"rm ");
                             } 
                                    
                      }
                  }

                }
              }
            currentSource=currentSource->next;
            result=NULL;
          }  
}



void SortTreeList(treeNode_t **head)
{
	treeNode_t *current,*start;
  int flagstop=0;
  start=*head;
  if(start==NULL)
    return;
 
  
 while(start){ 
   current=start->next;
   
   if(current==NULL)
      break;

  while(flagstop==0){
     
     if(strcmp(start->Sname,current->Sname)>0){
      swapNodes(head,start->Sname,current->Sname);
      start=(*head);
      }    
      if(current->next==NULL)
        flagstop=1;
      current=current->next;  

    }
    flagstop=0;
    start=start->next;
    
 }
}
  
  

void swapNodes(treeNode_t **head_ref, char* x, char* y) 
{  
  
   // Search for x (keep track of prevX and CurrX 
   treeNode_t  *prevX = NULL, *currX = *head_ref; 
   while (currX && strcmp(currX->Sname,x)!=0 )
   { 
       prevX = currX; 
       currX = currX->next; 
   } 
  
   // Search for y (keep track of prevY and CurrY 
   treeNode_t *prevY = NULL, *currY = *head_ref; 
   while (currY && strcmp(currY->Sname,y)!=0 ) 
   { 
       prevY = currY; 
       currY = currY->next; 
   } 
  
   // If either x or y is not present, nothing to do 
   if (currX == NULL || currY == NULL) 
       return; 
  
   // If x is not head of linked list 
   if (prevX != NULL) 
       prevX->next = currY; 
   else // Else make y as new head 
       *head_ref = currY;   
  
   // If y is not head of linked list 
   if (prevY != NULL) 
       prevY->next = currX; 
   else  // Else make x as new head 
       *head_ref = currX; 
  
   // Swap next pointers 
   treeNode_t *temp = currY->next; 
   currY->next = currX->next; 
   currX->next  = temp; 
   
} 

void printTreeList(treeNodeList_t *treeNodeList){
  treeNode_t *current;
  current=treeNodeList->head;
  if(current==NULL){
    return;
  }
  printf("List start: ");
  while(current){
    printf("%s  ",current->Sname);
    
    current=current->next;
  }
  printf(" List end\n");
}

int compareiNodes(inode_t *source,inode_t *backup){

  if(source->fileSize!=backup->fileSize)
    return -1;
  if(source->lastChange!=backup->lastChange)
    return -1;
  return 0;  
}