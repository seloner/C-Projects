#ifndef TRIE_H_
#define TRIE_H_



typedef struct List{
	int id;
	int times;
	struct List *next;
	char *text;
}List;

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
void ListInsert(int id,trieVal_t root);
trieVal_t ListSearch(int id,trieVal_t root);
trieVal_t ListCreateNode(int id,char *text);
void printStatsofword(trieNode_t **root,const char *key);
int fqi(const char *string,trieNode_t **root,int id);
double plithosqi(trieVal_t qi);
int plithoskeimenon(trieVal_t qi);
void printall( trieNode_t *root );
void tf(trieVal_t nodetoprint,int id);
void listdestroy(trieNode_t *root);

double score();



 #endif





