#ifndef trie_H_
#define trie_H_
#include "graph.h"
typedef struct trienode {       //trienode
    char key;
    graph* pointer;
    struct trienode *next;
    struct trienode *prev;
    struct trienode *children;
    struct trienode *parent;
}trienode;
 void triecreate(trienode **root);
 void trieinit(trienode **root);
 int trieadd(trienode **root, char *key,graph *pointer);
 trienode* triesearch(trienode *root, char *key);
 void triespecial(trienode **root, char *key);
 void triedestroy(trienode *root);






 #endif
