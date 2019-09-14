#ifndef DIAVASMA_H_
#define DIAVASMA_H_
   typedef struct pidarray{
 
    pid_t *array;
    
   }pidarray;

  int diavasma(FILE*fptr);

  void eisagwgh(FILE*fptr,char **array,int arraysize);
  void pathallocation(char **array,int arraysize,int number);
  char* addstrings(char *str1,char *str2);
  void forker(int nprocesses,int counter,pidarray *ptrarray);
  void printtext(char *filename);
 #endif