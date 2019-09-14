#ifndef TRIE_H_
#define TRIE_H_



typedef struct List{
	int line;
	struct List *next;
	char *text;
    int times;
}List;
typedef struct fileinfo
{
    int numberoflines;
    char *pathname;
    char **textmap;
    struct fileinfo *next;
    
}fileinfo;

typedef List* trieVal_t;
 
typedef struct trieNode {
    char key;
    trieVal_t value;
    struct trieNode *next;
    struct trieNode *prev;
    struct trieNode *children;
    struct trieNode *parent;
} trieNode_t;

trieNode_t *TrieCreateNode(char key,trieVal_t  data); 
void TrieCreate(trieNode_t **root);
trieNode_t* TrieSearch(trieNode_t *root, const char *key);
void TrieAdd(trieNode_t **root, char *key,int textid,char *textlist);
void TrieDestroy( trieNode_t* root );
void ListInsert(int line,trieVal_t root,char *text);
trieVal_t ListCreateNode(int line,char  *text);
int plithoskeimenon(trieVal_t qi);
void listdestroy(trieNode_t *root);
fileinfo *fileinfocreate(int numberoflines,char *pathname,char **textmap);
void fileinfoinsert(fileinfo *head,fileinfo *nodetoinsert);
void fileinfolistcreate(fileinfo *head);
fileinfo *insertliststart(fileinfo *toinsert);
trieVal_t findmax(trieVal_t startnode);
trieVal_t findmin(trieVal_t startnode);
void putresults(trieVal_t node,char *array);


 #endif





