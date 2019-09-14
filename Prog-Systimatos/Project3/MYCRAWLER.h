#ifndef MYCRAWLER_H_
#define MYCRAWLER_H_
#include <stdio.h>
#include <sys/wait.h>	     /* sockets */
#include <sys/socket.h>	     /* sockets */
#include <netinet/in.h>	     /* internet sockets */
#include <netdb.h>	         /* gethostbyaddr */
#include <unistd.h>	         /* fork */		
#include <stdlib.h>	         /* exit */
#include <ctype.h>	         /* toupper */
#include <signal.h>          /* signal */
#include <pthread.h>          //threads
#include <semaphore.h>        // semaphores
#include <sys/poll.h>        //poll
#include <time.h>            //time
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct cnode
{
    char *link;
	struct cnode *next; 	
}cnode;
typedef struct targs{

	int port;
	const char *hostname;
	const char *directory;
	int sock;
	struct sockaddr_in server;			
    struct sockaddr *serverptr;

}targs;

void perror_exit(char *message);









 #endif