#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "graph.h"

graph* graphaddtolist(list *thelist  ,char *string ){           //add graph node to list
    graph *current=NULL;
    if(thelist->head==NULL){                        ///if list is empty
        thelist->head=creategraphnode(string);
        //printf("Created graph node %s \n",string);
        return thelist->head;
    }
    else{
        current=thelist->head;
        while(current->next!=NULL){
            current=current->next;
        }
        current->next=creategraphnode(string);          //else if list is not empty
        //printf("Created graph node %s \n",string);
        return current->next;
    }

}
graph* creategraphnode(char *string){           //create graph node
    graph* newgraphnode=malloc(sizeof(graph));
    if(newgraphnode==NULL){
        printf("Malloc failed\n");
        exit(1);
    }
    newgraphnode->string=malloc(strlen(string)+1);
    if( newgraphnode->string==NULL){
        printf("Malloc failed\n");
        exit(1);
    }
    newgraphnode->colour=0;
    newgraphnode->edgelist=NULL;
    strcpy (newgraphnode->string,string);
    newgraphnode->next=NULL;
    newgraphnode->parent=NULL;
    newgraphnode->storedvalue=0;
    return newgraphnode;
}
list* initgraphlist(){              //init list
    list *thelist=malloc(sizeof(list));
    thelist->head=NULL;
    return thelist;
}

void deletenode(list **thelist,char *string){           //delete node
  graph *prev;
  graph *temp = (*thelist)->head;   
   // If head node itself holds the key to be deleted          
    if(temp!=NULL && strcmp(temp->string,string)==0){
        (*thelist)->head=temp->next;
        free(temp->string);
        free(temp);
        temp=NULL;
        return;
    } 
     // Search for the key to be deleted, keep track of the 
    // previous node as we need to change 'prev->next' 
    while(temp!=NULL && strcmp(temp->string,string)!=0){
        prev=temp;
        temp=temp->next;
    }
    if(temp==NULL)
        return;
    prev->next=temp->next;
    free(temp->string);
    free(temp);    
                              
} 


void destroy(list * thelist){       //destroy every node in list
  graph *current = thelist->head;
  graph *next = current;
  while(current != NULL){
    next = current->next;
     if(current->edgelist)
         deletealledgesfromnode(current);
    free(current->string);
    free(current);
    current = next;
  }
  free(thelist);
}

graph* searchforgraph(list *thelist  ,char *string ){       //search for a graph in the list
        graph *current,*found=NULL;
        if(thelist->head){
            current=thelist->head;
            while(current!=NULL){
                if(strcmp(string,current->string)==0){      //graph found
                    found=current;
                    return found;
                }
                current=current->next;
            }
            if(found==NULL){
                return found;
            }


        }

}
edge* createdge(graph* source,graph *destination,int value){    //create edge 
    edge *newedge;
    newedge=malloc(sizeof(edge));
    if(newedge){
        newedge->destination=destination;
        newedge->source=source;
        newedge->value=value;
        newedge->visited=0;
        return newedge;

    }else{
        printf("Malloc error,cant create edge");
        return NULL;
    }

}

edge* searcedge(graph *node1,graph *node2){         //search edge between two graph nodes
    edgenode *current;
    current=node1->edgelist;
    while(current){
        if((current->theedge->source)==node2 || (current->theedge->destination)==node2){            //connection found
            return current->theedge;
        }else{
            current=current->next;
        }
    }

    return NULL;
}
void addedgetographnode(graph *node1,graph *node2,edge *edgetoadd){         //add edge between two graph nodes
    edge *current=NULL;
    edgenode *currentedgenodetoadd,*currentedgenode;
    currentedgenodetoadd=malloc(sizeof(edgenode));
    if(currentedgenodetoadd==NULL){
        printf("Malloc Failed\n");
    }
    currentedgenodetoadd->theedge=edgetoadd;
    currentedgenodetoadd->next=NULL;
    currentedgenode=node1->edgelist;
    if(currentedgenode==NULL){                      //if edge list for graph node is empty
        node1->edgelist=currentedgenodetoadd;
    }else{
        while(currentedgenode->next!=NULL){         //else insert at the end of list of edges
                currentedgenode=currentedgenode->next;
        }
        currentedgenode->next=currentedgenodetoadd;
    }
    if(node1!=node2){                           //if graph nodes are not the same , put the edge at list of the second graph node also
        currentedgenodetoadd=malloc(sizeof(edgenode));
        if(currentedgenodetoadd==NULL){
            printf("Malloc Failed\n");
        }
        currentedgenodetoadd->theedge=edgetoadd;
        currentedgenodetoadd->next=NULL;
        currentedgenode=node2->edgelist;
        
        if(currentedgenode==NULL){
            node2->edgelist=currentedgenodetoadd;
        }else{
            while(currentedgenode->next!=NULL){
                currentedgenode=currentedgenode->next;

            }
            currentedgenode->next=currentedgenodetoadd;
        }
    }    

 
        //printf("source %s,destination %s--->value %d\n",node1->string,node2->string,edgetoadd->value);
    

}
void findanddeleteedgenode(edge *edgetofind,graph *nodetosearch){
    edgenode *temp,*prev;
    temp=nodetosearch->edgelist;
    if(temp!=NULL && temp->theedge==edgetofind){                    //first node
        nodetosearch->edgelist=temp->next;
        free(temp);
        temp=NULL;
        return;
    }
    while(temp!=NULL && temp->theedge != edgetofind){               
        prev=temp;                                                       //keep previous
        temp=temp->next;
    }
    if(temp==NULL)
        return;
    prev->next=temp->next;    
    free(temp);    


}

void deletealledgesfromnode(graph *target){             //delete all edges from graph node
    edgenode *currentnode,*currentnodetemp;
    graph *nodetobeupdated;
    currentnode=target->edgelist;
    while(currentnode!=NULL){
        currentnodetemp=currentnode->next;
        if(currentnode->theedge){
            if((currentnode->theedge->source)!=(currentnode->theedge->destination)){    //if edge has different source and destination you need to update both nodes
                if(target!=currentnode->theedge->source){
                    nodetobeupdated=currentnode->theedge->source;        
                }else{
                    nodetobeupdated=currentnode->theedge->destination;
                }
                if(nodetobeupdated)
                    findanddeleteedgenode(currentnode->theedge,nodetobeupdated);             //update the edge node value for the node that needs to be updated

            }
        }
        if(currentnode->theedge)                    //delete edge 
            free(currentnode->theedge);
         if(currentnode)       
            free(currentnode);                      //delete edge   node
        currentnode=currentnodetemp;
    }
}
void deletededge(edge *target){             //delete edge
    edgenode *current,*temp,*prev;
    current=target->source->edgelist;
   
    current=target->source->edgelist;       //find and delete edge node for source node
    if(current!=NULL && current->theedge==target){
        target->source->edgelist=current->next;
        free(current);
        current=NULL;
    }
    while (current!=NULL && current->theedge!=target) 
    { 
        prev = current; 
        current = current->next; 
    }
    if(current){
        prev->next=current->next;
        free(current);
    }  
    current=target->source->edgelist;
   current=target->destination->edgelist;           //find end delete edge node for destination
   if(current){
       if(target->destination!=target->source){
        if(current!=NULL && current->theedge==target){
            target->destination->edgelist=current->next;
            free(current);
            free(target);
            current=NULL;
            return;
        }
        while (current!=NULL && current->theedge!=target) 
         { 
            prev = current; 
            current = current->next; 
        }
        if (current == NULL) return;
        prev->next=current->next;
        free(current);
        free(target);  
      }
    } 
                //free edge
} 
    
edge* searchedgebyvalue(graph *source,graph *destination,int value){        //find edge by value
    edgenode *current;
    current=source->edgelist;
    while(current){
        if(current->theedge){
            if( current->theedge->value==value && (current->theedge->destination)==destination){
                return current->theedge;
            }
        }
        current=current->next;
    }

    return NULL;        

}

void modify(edge *target,int newvalue){         //modify edge
    target->value=newvalue;
}

void recievingprint(graph *target){         //print for target graph node all incoming edges
    edgenode *current=target->edgelist;
    int flagfound=0;
    while(current){
        if(current->theedge){
            if(current->theedge->destination==target){
                printf("|%s|->%d->|%s|\n",current->theedge->source->string,current->theedge->value,current->theedge->destination->string);
                flagfound=1;
            }
        }
        current=current->next;
        
    }
    if(flagfound==0){
        printf("No rec-edges %s\n",target->string);
    }
}


void graphprintf(list *thelist,char *filepath){     //print graph to file
    FILE *fp;
    graph *current;
    edgenode *currentedgenode;
    fp=fopen(filepath,"w");
    if(fp==NULL){
        printf("Can't create/open output file\n");
    }else{
        if(thelist->head==NULL){
            fprintf(fp,"Empty Graph\n");
        }else{
            current=thelist->head; 
            while(current){                         //for every graph node print all outcoming edges to file
                fprintf(fp,"|%s|\n",current->string);
                currentedgenode=current->edgelist;
                while(currentedgenode){
                    if(currentedgenode->theedge->source==current){
                        fprintf(fp,"        %d->|%s|\n",currentedgenode->theedge->value,currentedgenode->theedge->destination->string);
                    }
                    currentedgenode=currentedgenode->next;
                }
                current=current->next;
                
            }
            fclose(fp);
        }

    }



}

void printalledges(graph *target){
    edgenode    *current=target->edgelist;
    while(current){
        printf("%s %s %d \n",current->theedge->source->string,current->theedge->destination->string,current->theedge->value);
        current=current->next;
    }

}