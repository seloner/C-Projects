#include <stdio.h>
#include "HELPFUCTS.h"

cnode *createhead() {						 /*create head for list*/
	cnode *insertnode;
	insertnode=malloc(sizeof(cnode));
	if(insertnode){
		insertnode->next=NULL;
		insertnode->link=malloc(10*sizeof(char));
		strcpy(insertnode->link,"head");
		return insertnode;
	}
	
	return NULL;

	

}

char *request(char *link){						 /*create the http header*/
	char *answer;
	answer=malloc(500*sizeof(char));
	if(answer==NULL){
		printf("malloc error\n");
		return NULL;
	}

    strcpy(answer,"GET ");
    strcat(answer,link);
    strcat(answer," HTTP/1.1\n");
    strcat(answer,"User-Agent: mycrawler\n");
    strcat(answer,"Host: localhost\n");
    strcat(answer,"Accept-Language: en-us\n");
    strcat(answer,"Accept-Encoding: gzip, deflaete\n");
    strcat(answer,"Connection: Keep-Alive\n");
    return answer;




}

cnode *addtolist(cnode *head,char *link){
	cnode *temp,*nodetoinsert;
	temp=head;
	int flagfound=0;
	while(temp!=NULL){
		if(temp->link!=NULL){
			if(strcmp(link,temp->link)==0){			 /*if link exists in list*/
				flagfound=1;
				
				break;
			}
		}

		if(temp->next==NULL){
			break;

		}
		temp=temp->next;
	}
	if (flagfound==0)
	{ 
	
		nodetoinsert=malloc(sizeof(cnode));
		if(nodetoinsert){
			nodetoinsert->link=malloc(strlen(link)*sizeof(char)+1);
			if (nodetoinsert)
				strcpy(nodetoinsert->link,link);
			nodetoinsert->next=NULL;
			temp->next=nodetoinsert;
			
			return nodetoinsert;
		}


	}
	if(flagfound==1)
		return NULL;



}
void deletelist(cnode *head){
	cnode *current=head;
	cnode *next;
	while(current!=NULL){
		next=current->next;
		if(current->link){
			printf("Deleting link %s\n",current->link);
			free(current->link);
		}
		free(current);
		current=next;

	}
	head=NULL;


}

