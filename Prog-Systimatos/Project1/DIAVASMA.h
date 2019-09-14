#ifndef DIAVASMA_H_
#define DIAVASMA_H_
#include "TRIE.h"

 int diavasma(FILE*fptr);
 void eisagwgh(FILE*fptr,char **array,int arraysize);
 int words( char *text);
 double averagecount(char **array,int arraysize);
 double idf(double n,double qi);
 double scorefunc(double n,double qi,const char *string,trieNode_t *root,int id,double k,char *text,char **array,double b);



 #endif