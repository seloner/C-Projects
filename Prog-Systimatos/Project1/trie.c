 
#include <stdio.h>
#include "TRIE.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
List *listptr=NULL;

 
void TrieCreate(trieNode_t **root)
{
 *root = TrieCreateNode('\0', NULL);
}
 
trieNode_t *TrieCreateNode(char key, trieVal_t data)
{
 trieNode_t *node = NULL;
 node = (trieNode_t *)malloc(sizeof(trieNode_t));
 if(NULL == node)
 {
  printf("Malloc failed\n");
  return node;
 }
 
 node->key = key;
 node->next = NULL;
 node->children = NULL;
 node->value = data;
 node->parent= NULL;
 node->prev= NULL;
 return node;
}
 
void TrieAdd(trieNode_t **root, char *key,int textid, char *textlist)
{
 trieNode_t *pTrav = NULL;
 trieNode_t *found=NULL;
 trieVal_t dataToInsert=NULL;
 trieVal_t listfound=NULL;
 
 if(NULL == *root)
 {
  printf("NULL tree\n");
  return;
 }

// printf("\nInserting key %s: \n",key);

 pTrav = (*root)->children;
 
  
 
 if(pTrav == NULL)
 {

  /*First Node*/
  for(pTrav = *root; *key; pTrav = pTrav->children)
  {
   pTrav->children = TrieCreateNode(*key, NULL);
   pTrav->children->parent = pTrav;
#ifdef DEBUG
   printf("Inserting: [%c]\n",pTrav->children->key);
#endif
   key++;
  }
  dataToInsert=ListCreateNode(textid,textlist);
  pTrav->children = TrieCreateNode('\0', dataToInsert);
  pTrav->children->parent = pTrav;
#ifdef DEBUG
  printf("Inserting: [%c]\n",pTrav->children->key);
#endif
  return;
 }



	found=TrieSearch(pTrav, key);
 


 if(found)
 {
  //printf("Duplicate!\n");

  #ifdef DEBUG
  		listfound=ListSearch(textid,found->value);
      

  		if(listfound!=NULL)
 
	#endif
   ListInsert(textid,found->value);
  
  return;
 }
 
 
 while(*key != '\0')
 {
  if(*key == pTrav->key)
  {
   key++;
#ifdef DEBUG
   printf("Traversing child: [%c]\n",pTrav->children->key);
#endif
   pTrav = pTrav->children;
  }
  else
   break;
 }
 
 while(pTrav->next)
 {
  if(*key == pTrav->next->key)
  {
   key++;
   TrieAdd(&(pTrav->next), key, textid,textlist);
   return;
  }
  pTrav = pTrav->next;
 }

 
 if(*key)
 {
  pTrav->next = TrieCreateNode(*key, NULL);
 }
 else
 {
  dataToInsert=ListCreateNode(textid,textlist);


  pTrav->next = TrieCreateNode(*key, dataToInsert);

 }
 
 pTrav->next->parent = pTrav->parent;
 pTrav->next->prev = pTrav;
 
#ifdef DEBUG
 printf("Inserting [%c] as neighbour of [%c] \n",pTrav->next->key, pTrav->key);
#endif
 
 if(!(*key))
  return;
 
 key++;
 
 for(pTrav = pTrav->next; *key; pTrav = pTrav->children)
 {
  pTrav->children = TrieCreateNode(*key, NULL);
  pTrav->children->parent = pTrav;
#ifdef DEBUG
  printf("Inserting: [%c]\n",pTrav->children->key);
#endif
  key++;
 }
 
 dataToInsert=ListCreateNode(textid,textlist);
 pTrav->children = TrieCreateNode('\0',dataToInsert);
 pTrav->children->parent = pTrav;
#ifdef DEBUG
 printf("Inserting: [%c]\n",pTrav->children->key);
#endif	
 return;
}
 
trieNode_t* TrieSearch(trieNode_t *root, const char *key)
{
 trieNode_t *level = root;
 trieNode_t *pPtr = NULL;
 
 int lvl=0;
 while(1)
 {
  trieNode_t *found = NULL;
  trieNode_t *curr;
 
  for (curr = level; curr != NULL; curr = curr->next)
  {
   if (curr->key == *key)
   {
    found = curr;
    lvl++;
    break;
   }
  }
 
  if (found == NULL)
   return NULL;
 
  if (*key == '\0')
  {
   pPtr = curr;
   return pPtr;
  }
 
  level = found->children;
  key++;
 }
}
 

 
void TrieDestroy( trieNode_t* root )
{
 trieNode_t *tPtr = root;
 trieNode_t *tmp = root;
 
    while(tPtr)
 {
  while(tPtr->children)
   tPtr = tPtr->children;
 
  if( tPtr->prev && tPtr->next)
  {
   tmp = tPtr;
   tPtr->next->prev = tPtr->prev;
   tPtr->prev->next = tPtr->next;
#ifdef DEBUG
   printf("Deleted [%c] \n", tmp->key);
#endif
   free(tmp);
  }
  else if(tPtr->prev && !(tPtr->next))
  {
   tmp = tPtr;
   tPtr->prev->next = NULL;
#ifdef DEBUG
   printf("Deleted [%c] \n", tmp->key);
#endif
   free(tmp);
  }
  else if(!(tPtr->prev) && tPtr->next)
  {
   tmp = tPtr;
   tPtr->parent->children = tPtr->next;
   tPtr->next->prev = NULL;
   tPtr = tPtr->next;
#ifdef DEBUG
   printf("Deleted [%c] \n", tmp->key);
#endif
   free(tmp);
  }
  else
  {
   tmp = tPtr;
   if(tPtr->parent == NULL)
   {
    /*Root*/
    free(tmp);
    return;
   }
   tPtr = tPtr->parent;
   tPtr->children = NULL;
#ifdef DEBUG
   printf("Deleted [%c] \n", tmp->key);
#endif
   free(tmp);
     }
    }
   }
   
 

void ListInsert(int id,trieVal_t root){
	trieVal_t data,dataToInsert,newListNode;
	int flagfound=0;
	data=root;
	while(data!=NULL){
		if(data->id==id){
			data->times++;
			flagfound=1;
			break;
		}
		data=data->next;
	}
  


	if(flagfound==0){
		data=root;
		while(data!=NULL){ 

			if(data->next==NULL){
				newListNode=ListCreateNode(id," ");
        data->next=newListNode;
        break;		

				}
        data=data->next;


			}


		}



	
  flagfound=0;

}
 trieVal_t ListCreateNode(int id,char  *text){      //dimiourgia node gia tin lista
    trieVal_t data;
        data=(trieVal_t)malloc(sizeof(List));
  if (data==NULL){
    printf("malloc failed \n");
    return data;

  }
  //printf("error\n");

  
  #ifdef DEBUG
       printf("Inserting node with text id %d \n",id);
  #endif
  data->id=id;
  data->next=NULL;
  data->times=1;
  data->text=(char*)malloc((strlen(text)+1)*sizeof(char));
  strcpy(data->text,text);

  return data;


}

trieVal_t ListSearch(int id,trieVal_t root){
	trieVal_t current;
	current=root;

	while(current){

		if(current->id==id){

			return current;
		}
		current=current->next;


	}
	return current;


}


void printStatsofword(trieNode_t **root,const char *key){
	trieNode_t *currentNode;
	trieVal_t currentList;

	currentNode=TrieSearch((*root)->children,key);
	currentList=currentNode->value;
	while(currentList!=NULL){
		printf("The word %s exists in text number %d %d times\n",key,currentList->id,currentList->times );
		currentList=currentList->next;

	}

}



double plithosqi(trieVal_t qi){ //plithos emfanisewn tis leksis
  double sum=0;
  trieVal_t current;
  current=qi;
  while(current){
    sum=sum+current->times;
    current=current->next;


  }
  return sum;

 } 
int plithoskeimenon(trieVal_t qi){
  trieVal_t current;
  current=qi;
  int sum=0;
  //if(current==NULL)
     // printf("error \n ");
  while(current){
    sum=sum+1;
    current=current->next;

    }
  return sum;
}

int fqi(const char *string,trieNode_t **root,int id){
  int sum=0;
  trieNode_t *pTrav;
  trieNode_t *found;
  trieVal_t current;
  pTrav=(*root)->children;
  found=TrieSearch(pTrav,string);
    
  if(found){
    current=found->value;
    while(current){
      printf("%d \n",current->times);
     if(current->id==id)
      sum=sum+current->times;
      current=current->next;


    }


    return sum;


  }

  return sum;

}
void printall( trieNode_t *root )
{
 trieNode_t *current;
 trieVal_t nodetoprint;
 current=root;

 int sum;
  while(current!=NULL){
    if(current->next){                //pigene deksia sto dentro
        printall(current->next);


    }
    if (current->children!=NULL){

        printall(current->children);    //pigene pros ta katw


    }


    if(current->value){
      nodetoprint=current->value;
      sum=plithoskeimenon(nodetoprint);
      printf("%s %d \n",(nodetoprint->text),sum); //emfanise ton komvo tis listas

    }

    break;

 
  } 
}
void listdestroy(trieNode_t *root){

    trieNode_t *current;
    trieVal_t nodetodelete;
    trieVal_t temp;

    while(current!=NULL){
    if(current->next){
        listdestroy(current->next);


    }
    if (current->children!=NULL){

        listdestroy(current->children);


    }


    if(current->value){
      nodetodelete=current->value;
      while(nodetodelete){
        temp=nodetodelete->next;
        current->value=temp;
        free(nodetodelete->text);
        free(nodetodelete);
        nodetodelete=current->value;

      

    }

    break;

  } 





}


}

void tf(trieVal_t nodetoprint,int id){
    char *textforprint;
    textforprint=(char*)malloc((strlen(nodetoprint->text)+1)*sizeof(char));
    strcpy(textforprint,nodetoprint->text);
    while(nodetoprint){
      if(id==(nodetoprint->id))
        printf("%d %s %d \n",nodetoprint->id,textforprint,nodetoprint->times);
      
        nodetoprint=nodetoprint->next;
    }


    }

    
