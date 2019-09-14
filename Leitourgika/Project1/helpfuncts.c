#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "helpfuncts.h"
int currentpath=0;
int flagfound=0;

void feed(list *thelist,trienode *root,const char *path){
      int counter=0;
      FILE *fp;
      size_t bufsize = 500;
      size_t nread;
      char buffer2[500],*token;
      char *buffer=malloc(bufsize*sizeof(char));
      trienode *trienodefound;
      graph *graphfound,*destination,*source,*pointer;
      edge *edgetoadd;
      int value;
      int numberofrecords=0;
      if ((fp= fopen(path, "r")) == NULL)         //open file to feed with data
      {
          printf("Error! opening file,cant feed with data\n");
          exit(1);
          
                          
      }else{
       
        while(nread=getline(&buffer,&bufsize,fp)!=-1){    //read line by line
          
          strcpy(buffer2,buffer);
      
          token=strtok(buffer2," \t");              
          while(token){
              if(strcmp(token,"\n")!=0){                  
                       
                if(counter==0){
                    trienodefound=triesearch(root->children,token);   //first node to insert
                    if(trienodefound==NULL){
                          source=graphaddtolist(thelist,token);
                          trieadd(&root,token,source);
                    }else{
                          source=trienodefound->pointer;
                    }
                }
                if(counter==1){                                         //second node to insert
                  trienodefound=triesearch(root->children,token);
                  if(trienodefound==NULL){
                    destination=graphaddtolist(thelist,token);
                    trieadd(&root,token,destination);

                    }else{
                    destination=trienodefound->pointer;
                  }
              }
              if(counter==2){                                   //value of the edge
                  value=atoi(token);
              }
              counter++;
              
            }
            token=strtok(NULL," \t");
          }
          if(counter==3 ){
            edgetoadd=createdge(source,destination,value);      //create edge
            
            if(edgetoadd){
                        addedgetographnode(source,destination,edgetoadd);
                        numberofrecords++;
            }
          }
          counter=0;
          
          
        }
        fclose(fp);
      }
      free(buffer);
}

void dfs(graph *start,graph *parent){       //dfs for circle find
  graph *current=start;
  edgenode *currentedgenode=start->edgelist;
    current->parent=parent;
    if(parent==NULL)
      flagfound=0;
    current->colour=1;                    //for the first node colour =1 else =2
    if(parent!=NULL){
            current->colour=2;
    }

    while(currentedgenode){                       //for every edge node for the current graph node

      if(currentedgenode->theedge->source==current && currentedgenode->theedge->destination->colour!=2){        //if is outcoming edge and edge is not visited
        if(currentedgenode->theedge->destination->colour==0){ 
          current->storedvalue=currentedgenode->theedge->value;           //store the value of the edge 
          
            
          dfs(currentedgenode->theedge->destination,current);           //call again dfs for the new node
        }
        else if(currentedgenode->theedge->destination->colour==1){      //if you found the first node it means we have a cycle
          current->storedvalue=currentedgenode->theedge->value;
          graph *temp,*nodetoprint=current;
         
          List *list=makelist();
          while(nodetoprint!=NULL){                             //cycle!!!!
            temp=nodetoprint;
            listadd(0,list,nodetoprint,NULL);              
            nodetoprint=nodetoprint->parent;
            
          }
            flagfound=1;
            reverse(list);                //reverse the list
            display(list);                //print cycles
            listdestroy(list);            //free list
                   
        }
      }
      currentedgenode=currentedgenode->next;
      
    }
    if(flagfound==0 && parent==NULL)         //if no cycle found
      printf("No-circle found |%s| \n",current->string);
    current->colour=0;
    current->storedvalue=0;
    current->parent=NULL;
  
}
void dfs2(graph *start,graph *parent,int minvalue,List *list){        //dfs for find circles
  graph *current=start;
  edgenode *currentedgenode=start->edgelist;
    if(parent==NULL){
      current->colour=1;
      flagfound=0;                                    //mark the first node with colour 1
    }
    while(currentedgenode){                               //for every edge node
          if(currentedgenode->theedge->destination->colour==1 && currentedgenode->theedge->value>=minvalue ){   //if you found the marked node and the value of the edge is accepted
            currentedgenode->theedge->visited=2;
            if(currentedgenode->theedge->destination==currentedgenode->theedge->source){          //if destination==source
                printf("Cir-found |%s|->%d->|%s|\n",currentedgenode->theedge->source->string,currentedgenode->theedge->value,currentedgenode->theedge->destination->string);
                flagfound=1;

            }if(!listempty(list)){
              flagfound=1;
              listadd(currentedgenode->theedge->value,list,current,currentedgenode->theedge);         //add the last node in the list and then print
              display2(list);
              deletebyedge(list,currentedgenode->theedge);          //start to delete every node in the list by edge 
            }       
        }else if(currentedgenode->theedge->source==current && currentedgenode->theedge->visited!=2 && currentedgenode->theedge->value>=minvalue){   //if the outcoming edge is unvisited
          currentedgenode->theedge->visited=2;
          listadd(currentedgenode->theedge->value,list,current,currentedgenode->theedge);       //add the node to the list
          dfs2(currentedgenode->theedge->destination,current,minvalue,list);            //dfs again for the destination node
          deletebyedge(list,currentedgenode->theedge);                    //when you return delete the node by edge from the list
        }
        currentedgenode=currentedgenode->next;
    }
    if(parent==NULL){
      if(flagfound==0)
        printf("No-circle-found involving |%s| %d\n",current->string,minvalue);          
      current->colour=0;
    }
    
}

void dfs3(graph *start,graph *target,int  maxpath,graph *parent,List *list){                                                      //dfs for traceflow
  graph *current=start;
  edgenode *currentedgenode=start->edgelist;

  if(parent==NULL){
    current->colour=1;            //1 for first node 
    target->colour=3;              //3 for the node I am tracing
    currentpath=0;
    flagfound=0; 

  }
  currentpath++;
  
  if(currentpath==1 && current->colour==3){
    display3(list);
    flagfound=1;
  }  
  if(currentpath<=maxpath){
    while(currentedgenode){
        
      if(currentedgenode->theedge->destination->colour==3 && currentedgenode->theedge->source==current){    //if you found the target and the source is the current node
          listadd(currentedgenode->theedge->value,list,current,currentedgenode->theedge);
          display3(list);
          flagfound=1;
          currentpath=0;
          deletebyedge(list,currentedgenode->theedge);

      }    
      else if(currentedgenode->theedge->source==current && currentedgenode->theedge->visited!=1 ){   //if the outcoming edge is unvisited
          currentedgenode->theedge->visited=2;
          listadd(currentedgenode->theedge->value,list,current,currentedgenode->theedge);       //add the node to the list
          dfs3(currentedgenode->theedge->destination,target,maxpath,current,list);            //dfs again for the destination node
          deletebyedge(list,currentedgenode->theedge);                    //when you return delete the node by edge from the list
        }
      
        currentedgenode=currentedgenode->next;
      }
      

  }
  

  if(parent==NULL){
    if(flagfound==0){       //no trace found
      printf("No-trace from |%s to |%s|\n",start->string,target->string);
    }
    current->colour=0;           
    target->colour=0; 
    flagfound=0;            
  }
}
 
void circlefind(graph *start){ 
        
      dfs(start,NULL);
}
void findcircles(graph *start,int minvalue){
   List *list=makelist();
  dfs2(start,NULL,minvalue,list);
  
    listdestroy(list);
}
void traceflow(graph *start,graph *target,int maxpath){
  List *list=makelist();
  dfs3(start,target,maxpath,NULL,list);
  if(!listempty(list))
  listdestroy(list);
}

node *createnode(int value,graph *pointer,edge *epointer){         //create node for the list
  node * newNode = malloc(sizeof(node));
  if (!newNode) {
    return NULL;
  }
  newNode->value = value;
  newNode->pointer=pointer;
  newNode->next = NULL;
  newNode->epointer=epointer;
  return newNode;
}

List * makelist(){                                    //init list
  List *list = malloc(sizeof(List));
  if (!list) {
    return NULL;
  }
  list->head = NULL;
  return list;
}

void display(List *list) {            //display circle find
  node * current = list->head;
  if(list->head == NULL) 
    return;
  printf("Cir-found ");
  for(; current != NULL; current = current->next) {
    printf("|%s|->%d->", current->pointer->string,current->pointer->storedvalue);
  }
  printf("|%s|",list->head->pointer->string);
  printf("\n");
}

void display2(List *list) {       //display find circles 
  int counter=0;
  node * current = list->head;
  if(list->head == NULL) 
    return;
  printf("-Cir-found ");
  for(; current != NULL; current = current->next) {
    if(counter==0){
  
      printf("|%s|->%d->|%s|",current->epointer->source->string,current->epointer->value,current->epointer->destination->string);
    }
    else{
      printf("->%d->|%s|",current->epointer->value,current->epointer->destination->string);
    }
    counter++;
  }
  printf("\n");
}
void display3(List *list){
  int counter=0;
  node *current = list->head;
  if(list->head == NULL) 
    return;
 for(; current != NULL; current = current->next) {
    if(counter==0){
  
      printf("Tra-found |%s|->%d->|%s|",current->epointer->source->string,current->epointer->value,current->epointer->destination->string);
    }
    else{
      printf("->%d->|%s|",current->epointer->value,current->epointer->destination->string);
    }
    counter++;
  }
  printf("\n");
}
void listadd(int value, List * list,graph *pointer,edge *epointer){     //add node to list
  node * current = NULL;
  if(list->head == NULL){
    list->head = createnode(value,pointer,epointer);
  }
  else {
    current = list->head; 
    while (current->next!=NULL){
      current = current->next;
    }
    current->next = createnode(value,pointer,epointer);
  }
}
void listdelete(List *list,graph *todelete){          //delete a node from list
    node *current,*temp; 
    if(list!=NULL){   
      current=list->head;
      if(current->next==NULL && current->pointer==todelete){          //if first node
        free(current);
        list->head=NULL;
      }
      while(current->next){                 //else
        if(current->next->pointer==todelete){         //find the node
          temp=current->next;
          current->next=temp->next;
          if(temp->epointer)
            temp->epointer->visited=0;
          free(temp);                       //delete it
          break;
        }
        current=current->next;
      }
    }
   
}
void deletebyedge(List *list,edge *edgetodelete){           //delete a node from list based on an edge
  node *current,*temp;
    if(list!=NULL){
      current=list->head;
      if(current->next==NULL && current->epointer==edgetodelete){     //if first node
        current->epointer->visited=0;
        free(current);
        list->head=NULL;
      }else{
        while(current->next){                                   //else
          if(current->next->epointer==edgetodelete){
            temp=current->next;
            current->next=temp->next;
            if(temp->epointer)
              temp->epointer->visited=0;                
          
            free(temp);                                         //delete the node
            break;
          }
          current=current->next;
        }
    }
  }
}
int listempty(List *list){
  if(list->head==NULL)
    return 1;
  else
    return 0;
}



void reverse(List * list){                //reverse all nodes in the list
  node *reversed = NULL;
  node *current = list->head;
  node *temp = NULL;
  while(current != NULL){
    temp = current;
    current = current->next;
    temp->next = reversed;
    reversed = temp;
  }
  list->head = reversed;
}                                  

void listdestroy(List * list){                      //destroy list
  node *current = list->head;
  node *next = current;
  while(current != NULL){
    next = current->next;
    if(current->epointer)
      current->epointer->visited=0;
    free(current);
    current = next;
  }
  free(list);
}