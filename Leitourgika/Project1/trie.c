#include <stdio.h>
#include <stdlib.h>
#include "trie.h"


trienode* triecreatenode(char key, graph *pointer){ //create trie node
    trienode *newnode;
    newnode=malloc(sizeof(trienode));
    if(newnode){
        newnode->next=NULL;
        newnode->children = NULL;
        newnode->parent= NULL;
        newnode->prev= NULL;
        newnode->key = key;
        newnode->pointer=pointer;
    }else{
        printf("Malloc failed\n");
    }
}

void trieinit(trienode **root){         //init trie
    *root=triecreatenode('\0',NULL);
}
int trieadd(trienode **root, char *key,graph *pointer){     //add node to trie
    trienode *current;
    if(*root){
        current=(*root)->children;
        if(current==NULL){          //First Node
            current=*root;
            while(*key){
                current->children=triecreatenode(*key,NULL);        //create a node for every char of the string
                current->children->parent=current;
                key++;
                current=current->children;
            }
        
        current->children=triecreatenode('\0',pointer);         //create the last node
        current->children->parent=current;
        return 0;
        }else if(triesearch(current,key)){          //if string exists in trie
            trienode *special=triesearch(current,key);
            if(special){
                if(special->pointer==NULL){
                    special->pointer=pointer;
                }else{
                    return -1;
                }

            }
            return -1;
        }else{
            while(*key != '\0'){                    //while string 
                if(*key==current->key){         //if you found a same key continue to children
                    key++;
                    current=current->children;
                }else{
                    break;
                }
            
            }
            while(current->next){                   
                if(*key==current->next->key){           //if next key is the same then call again triedd for the next node
                    key++;
                    trieadd(&(current->next),key,pointer);
                    return 0;
                }
                current=current->next;
            }
            if(*key){                                           //if string then add keys
                current->next=triecreatenode(*key,NULL);
            }else{
                current->next=triecreatenode(*key,pointer);     //if string ended add the last node
            }
            current->next->parent=current->parent;
            current->next->prev=current;
            if(!(*key))                                 //if key ended return
             return 0;
            key++;
            current=current->next;
            while(*key){


                current->children=triecreatenode(*key,NULL);        //add key 
                current->children->parent=current;
                key++;
                current=current->children;                          //update children
            } 
            current->children=triecreatenode('\0',pointer);                 //add the last node
            current->children->parent=current;
            return 0;

        }
    }
    else{
        printf("Root Null\n");
    }


}


trienode* triesearch(trienode *root, char *key){
    trienode *start=root;
    trienode *lastnode=NULL;;
    trienode *found;
    trienode *current=start;
    while(1){
        found=NULL;
        while(current){
            if(current->key==*key){         //if found a horizontal break
                found=current;
                break;
            }
            current=current->next;

        }
        if(found==NULL){
            return NULL;
        }
        if(*key=='\0'){                 //if is is the end node
            lastnode=current;
            return lastnode;
            
        }
        current=found->children;        //continue vertically
        key++;

    }


}
void triespecial(trienode **root, char *key)        //update if the graph account gets deleted
{
    trienode *found,*current;
    if(*root){
        current=(*root)->children;
        found=triesearch(current,key);
        if(found){
            found->pointer=NULL;
        }
    }
}


void triedestroy(trienode *root){       //destroy trie
    trienode *current=root;
    trienode *temp=root;
    while(current){                     //start from route
        while(current->children)        //for every children you find
            current=current->children;
            if(current->prev && current->next){ //if it has prev and next
                temp=current;
                current->next->prev = current->prev;    //update
                current->prev->next = current->next;
                free(temp);                             //delete key
            }else if(current->prev && !(current->next)){           //if it only has prev
                temp=current;
                current->prev->next=NULL;                   //only update previous
                free(temp);
            }else if(!(current->prev) && current->next){    //if it has only next;
                temp = current;
                current->parent->children = current->next;      //update parent(cause we found a first node)
                current->next->prev = NULL;                    
                current = current->next;
                free(temp);

            }else{
                temp=current;
                if(temp->parent==NULL){             //we found root  
                    free(temp);
                    return;
                }
            current=current->parent;
            current->children=NULL;
            free(temp);             //delete root
           }
    }
}
