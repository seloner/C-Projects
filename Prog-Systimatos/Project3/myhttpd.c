#include <stdio.h>
#include <sys/wait.h>	     /* sockets */
#include <sys/types.h>	     /* sockets */
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
#include "MYHTTPD.h"
pthread_mutex_t mtx ;
pthread_cond_t cond ;

node *head=NULL;
volatile int flag = 0;
volatile unsigned long  int tsize=0,npages=0;




char* createheader(char *header,char *path,int typeofanswer,unsigned long int size){
	char *content;char *answer0="HTTP/1.1 200 OK\n",*answer1="HTTP/1.1/403  Forbidden\n",*server="Server: myhttpd/1.0.0 (Ubuntu64)\n",*connection="Connection: closed\n",*type="Content-Type: text/html\n";
	char s[68],badrequest[70]="\n<!DOCTYPE html>\n<html>\n 	<body>\n 	Wrong Request\n 	</body>\n</html>";
	FILE *fp;
    int rval,fileok=0;
    long int contentsize;
	
	
	memset(header, 0, sizeof(header));   
	time_t t = time(NULL);
    struct tm tm = *gmtime(&t);
    strftime(s, sizeof s, "%a, %d %b %Y %H:%M:%S %Z", &tm);

  
    if(typeofanswer==-1){									//badrequest,construct the header
    	strcpy(header,"HTTP/1.0 400 Bad Request\n");
			strcat(header,"Date: ");
	    	strcat(header,s);
	    	strcat(header,"\n");
	    	strcat(header,server);
	    	strcat(header,"Content-Length: ");
	    	sprintf(s,"%lu",strlen(badrequest)+1);
            pthread_mutex_lock(&mtx);
            tsize=tsize+strlen(badrequest);
            npages++;
            pthread_mutex_unlock(&mtx);
	    	strcat(header,s);
	    	strcat(header,"\n");
	    	strcat(header,type);
	    	strcat(header,connection);
	    	content=malloc(strlen(badrequest)+1);
            strcpy(content,badrequest);
            return content;




    }
    else{                                                  //header ok continue

         rval = access (path, F_OK);                        //check if file exists
         
         if (rval == 0) {

             rval = access (path, R_OK);                     //check if is readable
             if (rval == 0){
                
                    fileok=1;
            	    if ((fp=fopen(path,"r")) == NULL)					//request ok check for the file
            		{
            			 printf("Error! opening file %s\n",path);
            			          
            			          			return(NULL);         

            		}
                    fseek(fp, 0, SEEK_END);
                    contentsize = ftell(fp);
                    fseek (fp, 0, SEEK_SET);
                    content = malloc (contentsize*sizeof(char)+1);
                    if (content)
                    {
                        fread (content, 1, contentsize,fp);
                        content[contentsize] = '\0';

                    }

                    fclose(fp);
            }
            else
                fileok=-1;
            if(fileok==1)
			    strcpy(header,answer0);
            else
                strcpy(header,answer1);

			strcat(header,"Date: ");
	    	strcat(header,s);
	    	strcat(header,"\n");
	    	strcat(header,server);
	    	strcat(header,"Content-Length: ");
            if(fileok==1){
	    	      sprintf(s,"%lu",contentsize);
                  pthread_mutex_lock(&mtx);
                  tsize=tsize+strlen(content);
                  npages++;
                  pthread_mutex_unlock(&mtx);
            }
            else{
                  sprintf(s,"%lu",strlen("<html>Trying to access this file but don't think I can make it.</html>")+1);
                  pthread_mutex_lock(&mtx);
                  tsize=tsize+strlen("<html>Trying to access this file but don't think I can make it.</html>");
                  npages++;
                  pthread_mutex_unlock(&mtx);
	    	}
            strcat(header,s);
	    	strcat(header,"\n");
	    	strcat(header,type);
	    	strcat(header,connection);
            strcat(header,"\n");
            if(fileok==-1){
                content=malloc(strlen("<html>Trying to access this file but don't think I can make it.</html>")+1);
                strcpy(content,"<html>Trying to access this file but don't think I can make it.</html>");
            }
            return content;





    	}else{
            strcpy(header,"HTTP/1.1 404 Not found\n");
            strcat(header,"Date: ");
            strcat(header,s);
            strcat(header,"\n");
            strcat(header,server);
            strcat(header,"Content-Length ");
            sprintf(s,"%lu",strlen("<html>Sorry dude,couldn't find this file .</html>")+1);

            strcat(header,s);
            strcat(header,"\n");
            strcat(header,type);
            strcat(header,connection);
            strcat(header,"\n");
            content=malloc(strlen("<html>Sorry dude,couldn't find this file .</html>")+1);
            pthread_mutex_lock(&mtx);
            tsize=tsize+strlen("<html>Sorry dude,couldn't find this file .</html>");
            npages++;
            pthread_mutex_unlock(&mtx);
            strcpy(content,"<html>Sorry dude,couldn't find this file .</html>");
            return content;

        }

    }

	
}
int checkheader(char *buffer){                       /*chech if http header is ok */
	int headargs=0;
	char *token;


	token = strtok(buffer," ");

	while(token!=NULL){
		
		if(strcmp(token,"GET")==0){
			headargs++;
		}
		else if(strcmp(token,"User-Agent:")==0){
			headargs++;
			

		}
		else if(strcmp(token,"Host:")==0){
			headargs++;
		}
		else if(strcmp(token,"Accept-Language:")==0){
			headargs++;
			
		}
		else if(strcmp(token,"Accept-Encoding:")==0){
		
			headargs++;
		}
		else if(strcmp(token,"Connection:")==0){

			headargs++;
		}	
		token = strtok(NULL, "\n""\t"" "); //get the first word of the next line
		
		



	}
    
	if(headargs==6)
		return 0;
	else 
		return -1;
}	
void perror_exit(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void *threadcreate(void *message) {
    static const long ok_return = 0;
    int id,fd,flagexit=0,headerflag=0;
    char buffer[4086];
    unsigned long int size=0;
  	char *content,*answer,*token,*headercheck,*currentdir,*directory,*wholeanswer;
    directory=malloc(sizeof(message)+200);
    strcpy(directory,message);
    if(strlen(directory)>2)
        directory[strlen(directory)-1]='\0'; 

    node *p;
    
    
    while(1){
    	
 
       pthread_mutex_lock(&mtx);
        if(flag==1){
            printf("SHUTDOWN\n");
             break;
        }
        while(head==NULL){
            

            pthread_cond_wait(&cond , &mtx);     /*if head NULL wait */      
            if(flag==1){
                
                flagexit=1;
                pthread_mutex_unlock(&mtx);
                break;
            }
            

        }    
            
            if(flagexit==1)
                break;
            p=head;
            head=head->next;
            if(p!=NULL){
                pthread_mutex_unlock(&mtx);       
                read(p->number,buffer,4080);
                headercheck=malloc(strlen(buffer)+1);        /*checkheader*/
                strcpy(headercheck,buffer);
                headerflag=checkheader(headercheck);
                free(headercheck);
                size=strlen(buffer);
                answer=malloc(size*sizeof(char)+300);                  
                currentdir=malloc(strlen(directory)+100);
                strcpy(currentdir,directory);
                
                if(headerflag<0){
                	
                	content=createheader(answer,"",-1,size); //http wrong request

                }
                else{

                	
                	token = strtok(buffer," ");
                	token = strtok(NULL, " ");
                	content=createheader(answer,strcat(currentdir,token),0,size); //try to find the html page
                }
                
                free(currentdir);
                if(content!=NULL){
                    wholeanswer=malloc(strlen(answer)+strlen(content)+1);       
                    if(wholeanswer!=NULL){
                    strcpy(wholeanswer,answer);
                    strcat(wholeanswer,content);

                    write(p->number,wholeanswer,strlen(wholeanswer)+1);  /*send the page*/
                    
                    }
                 }
                 else{
                    write(p->number,"error",strlen("error"));

                 } 
                free(content);
                free(wholeanswer);
                free(answer);
                printf("Server closing connection\n");
                close(p->number);
                free(p);
                
            }                       
  
            
            

    }
    
    printf("Thread exit\n");
    free(directory);
    pthread_mutex_lock(&mtx);
    if(head!=NULL)
        free(head);
    pthread_mutex_unlock(&mtx);

    return (void*)&ok_return;
   
     
      


}


int main(int argc, char *argv[]) {
    int    i,clientflag=0,serverflag=0,breakflag=0,port, port2, sock,sock2, newsock, newsock2,numberofthreads;
    struct sockaddr_in server, client;
    socklen_t clientlen = sizeof(client);
    struct sockaddr *serverptr=(struct sockaddr *)&server;
    struct sockaddr *clientptr=(struct sockaddr *)&client;                   /*servers init*/
    struct hostent *rem;
    pthread_t *tid;
    node *temp;
    struct pollfd fds[2];
    time_t start, current;
    start = time(NULL);            //time count starts
    char serverbuffer[800],tempbuffer[50],path[100];
    unsigned long int totaltime,min,hr,sec,readsize=0;
    struct node *p;
    pthread_mutex_init(&mtx, 0);
    pthread_cond_init(&cond, 0);    
    
    if (argc != 9) {
        printf("Please give right args\n");exit(1);}
    port = atoi(argv[2]);
    port2= atoi(argv[4]);
    strncpy(path,argv[8],strlen(argv[8])+1);
    numberofthreads=atoi(argv[6]);
    tid=malloc(numberofthreads*sizeof(pthread_t));
    

    
  
   
    /* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit("socket");
    server.sin_family = AF_INET;       /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);      /* The given port */
    /* Bind socket to address */
    if (bind(sock, serverptr, sizeof(server)) < 0)
        perror_exit("bind");
    /* Listen for connections */
    if (listen(sock, 5) < 0) perror_exit("listen");
    printf("Listening for connections to port %d\n", port);
    // signal(SIGCHLD, sigchld_handler);
    /* Create socket */
    if ((sock2 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit("socket");
    server.sin_family = AF_INET;       /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port2);      /* The given port */
    /* Bind socket to address */
    if (bind(sock2, serverptr, sizeof(server)) < 0)
        perror_exit("bind");
    /* Listen for connections */
    if (listen(sock2, 5) < 0) perror_exit("listen");
    printf("Listening for connections to port %d\n", port2);
    

    

    for ( i = 0; i < numberofthreads; i++){
         pthread_create(&tid[i], NULL, threadcreate,(void*)path);        /*create threads and pass the directory to them*/
        
    
    }
    fds[0].fd =sock;
    fds[0].events = POLLIN;                              /* prepare for poll*/
    fds[1].fd=sock2;
    fds[1].events=POLLIN;

 

    while (1) {
        if(flag==1){
            pthread_cond_broadcast(&cond);
            break;
        }
        poll(fds, 2,0);                              /*check for incoming connections*/
        for(int i=0; i<2; i++){                             
            if(fds[i].revents==POLLIN){
                if(i==0)
                    clientflag=1;
                 if(i==1)
                    serverflag=1;   

            }


        }
       
        
    	if(clientflag==1){
            clientflag=0;
            if ((newsock = accept(sock, clientptr, &clientlen)) < 0) perror_exit("accept");
        	
        	printf("Accepted connection port %d:\n",port);
            if(flag==0){ 
                p=malloc(sizeof(struct node));
                if(p!=NULL){
                    p->number=newsock;
                    pthread_mutex_lock(&mtx);
                    p->next=head;
                    head=p;
                    pthread_mutex_unlock(&mtx);
                    pthread_cond_signal(&cond);
                }
            }
            else{

                printf("Server is going down,cannot accept the request\n");
                close(newsock);



            }
        }
        if(serverflag==1){
            serverflag=0;
            
            if ((newsock2 = accept(sock2, clientptr, &clientlen)) < 0) perror_exit("accept");
            /* Find client's address */
            printf("Accepted connection port %d:\n",port2);
            
            
            memset(serverbuffer,0,sizeof(serverbuffer));
            readsize=read(newsock2,serverbuffer,800);
            printf("%s\n",serverbuffer );
            if(readsize<0){
            	printf("Read Fail\n");
            	write(newsock2,"Cant read your data",strlen("Cant read your data"));
            	close(newsock2);
            }
          	else{
    
          		if(serverbuffer[strlen(serverbuffer)-1]=='\n'){
   
          			serverbuffer[strlen(serverbuffer)-2]='\0';
   
          		}
      
	            if(strcmp(serverbuffer,"STATS:")==0){               /*send stats*/
	            	 //printf("Size:%lu\n",readsize);
	                 current = time(NULL); 
	                
	                totaltime = ((long int) (current-start));
	                hr = totaltime/3600;
					totaltime = totaltime%3600;
					min = totaltime/60;
					totaltime = totaltime%60;
					sec = totaltime;
					strcpy(serverbuffer,"Server up for ");
	                sprintf(tempbuffer, "%lu",hr);
	                strcat(serverbuffer,tempbuffer);
	                sprintf(tempbuffer, "%lu",min);

	                strcat(serverbuffer,":");
	                strcat(serverbuffer,tempbuffer);
	                sprintf(tempbuffer, "%lu",sec);
	                strcat(serverbuffer,":");
	                strcat(serverbuffer,tempbuffer);
	                strcat(serverbuffer," , served ");
                    pthread_mutex_lock(&mtx);
                    sprintf(tempbuffer,"%lu",npages);
                    strcat(serverbuffer,tempbuffer);
                    strcat(serverbuffer," pages, ");
                    sprintf(tempbuffer,"%lu",tsize);
                    strcat(serverbuffer,tempbuffer);
                    strcat(serverbuffer," bytes");
                    printf("%s\n",serverbuffer );
	                if(write(newsock2,serverbuffer,strlen(serverbuffer)+1)<0){
	                	perror_exit("Server write");
	            	}
	            	close(newsock2);


	                //printf("%s\n",serverbuffer);
	            }
                else if(strcmp(serverbuffer,"SHUTDOWN:")==0){
                    flag=1;
                    printf("Server shutdown\n");
                    write(newsock2,"server shutdown",strlen("server shutdown"));
                    close(newsock2);
                    pthread_mutex_unlock(&mtx);
                    pthread_cond_broadcast(&cond);





                }
	            else{
	 
	            	
	            	write(newsock2,"error\n",strlen("error\n")+1);
	            	close(newsock2);


	            }
	         }   




        }
            

          
    }


     for (int j = 0; j < numberofthreads; j++)       /*wait for threads to finish*/
       pthread_join(tid[j], NULL);

    
    free(tid);


   
    return 0;
}