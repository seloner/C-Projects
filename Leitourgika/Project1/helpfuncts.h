#ifndef helpfuncts_H_
#define helpfuncts_H_
#include "trie.h"
#include "graph.h"

typedef struct node {
  int value;
  graph *pointer;
  edge *epointer;
  struct node * next;
}node;
typedef struct cyclelist {
  node * head; 
}List;

void feed(list *thelist,trienode *root,const char *path);
void dfs(graph *start,graph *parent);
void dfs2(graph *start,graph *parent,int minvalue,List *list);
void dfs3(graph *start,graph *target,int  maxpath,graph *parent,List *list);
void circlefind(graph *start);
void findcircles(graph *start,int minvalue);
void traceflow(graph *start,graph *target,int maxpath);
List * makelist();
void listadd(int value, List * list,graph *pointer,edge *epointer);
node *createnode(int value,graph *pointer,edge *epointer);
void display(List *list);
void display2(List *list);
void display3(List *list);
void reverse(List *list);
void listdestroy(List *list);
void listdelete(List *list,graph *todelete);
void deletebyedge(List *list,edge *edgetodelete);
int listempty(List *list);











#endif