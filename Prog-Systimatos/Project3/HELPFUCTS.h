#ifndef HELPFUCTS_H_
#define HELPFUCTS_H_
#include <stdio.h>
#include <stdlib.h>	         
#include <string.h>
#include <time.h>
#include "MYCRAWLER.h"
cnode *createhead();
char *request(char *link);
cnode *addtolist(cnode *head,char *link);
void deletelist(cnode *head);


#endif