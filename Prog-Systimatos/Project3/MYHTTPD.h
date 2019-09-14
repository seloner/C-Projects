#ifndef MYHTTPD_H_
#define MYHTTPD_H_
#include <stdio.h>
#include <stdlib.h>	         
#include <string.h>
#include <time.h>
typedef struct node{

    int number;
    struct node *next;
    
}node;
char* createheader(char *header,char *path,int typeofanswer,unsigned long int size);
int checkheader(char *buffer);
void perror_exit(char *message);





 #endif