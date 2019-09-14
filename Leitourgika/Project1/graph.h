#ifndef graph_H_
#define graph_H_

typedef struct edge edge;
typedef struct graph graph;
typedef struct edgenode edgenode;
typedef struct graph{           //graph node
    edgenode *edgelist;
    char  *string;
    graph *next;
    int colour;
    graph *parent; 
    int storedvalue;
}graph;

typedef struct edge{            //edge
    int value;
    graph *destination;
    graph *source;
    int visited;
}edge;

typedef struct edgenode{        //edge node
    edge *theedge;
    edgenode *next;
}edgenode;
typedef struct list{
    graph *head; 
}list;
graph* graphaddtolist(list *thelist  ,char *string );
graph* creategraphnode(char *string);
list* initgraphlist();
void deletenode(list **thelist,char *string);
void destroy(list * thelist);
graph* searchforgraph(list *thelist  ,char *string );
edge* createdge(graph *source,graph *destination,int value);
void addedgetographnode(graph *node1,graph *node2,edge *edgetoadd);
edge* searcedge(graph *node1,graph *node2);
void deletealledgesfromnode(graph *target);
void deletededge(edge *target);
edge* searchedgebyvalue(graph *source,graph *destination,int value);
void modify(edge *target,int newvalue);
void recievingprint(graph *target);
void graphprintf(list *thelist,char *filepath);
void findanddeleteedgenode(edge *edgetofind,graph *nodetosearch);
void printalledges(graph *target);

 #endif