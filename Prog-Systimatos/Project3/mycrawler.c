
#include "MYCRAWLER.h"
#include "HELPFUCTS.h"

volatile int flag = 0,numbferofpages=0;
volatile unsigned long int tsize=0;

pthread_mutex_t mtx ;
pthread_cond_t cond ;
cnode *head=NULL;
cnode *startoflist=NULL;

void perror_exit(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

void *troutine(void *args){
    char *token,*htmlmessage,*message,*link,*strp;
	char buffer[4086],directoryopen[500];char temp[512];    
    struct sockaddr_in server;                              /*init for connection*/
    struct sockaddr *serverptr = (struct sockaddr*)&server;
    struct hostent *rem;
    targs *targsptr;
    int sock,port,endflag=0;
    DIR* dir;
    FILE *fp;
    serverptr = (struct sockaddr*)&server;
    pthread_mutex_lock(&mtx);
    targsptr=(targs*)args;
    cnode *p,*current,*newhead;
    unsigned long int size=0,sz=0;
    int counter=0,ok=0,removeheader,listflag=0;
    while(1){

		    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		        perror_exit("socket");

		    if ((rem = gethostbyname(targsptr->hostname)) == NULL) {    
		       perror("gethostbyname");;
		    }
		    port=targsptr->port;
		    pthread_mutex_unlock(&mtx);
		    server.sin_family = AF_INET;
		    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
		    server.sin_port = htons(port);
		    
    
        	pthread_mutex_lock(&mtx);
            if (flag==1)                                /*need to shutdown */
                break;
        	while(head==NULL){

                    pthread_cond_wait(&cond , &mtx);     /*wait until something in list */
                    
                 }   
	            counter=0;
	           
	            removeheader=0;
	            p=head;
	            head=head->next;         
	            link=malloc(strlen(p->link)*sizeof(char)+1);
	            strcpy(link,p->link);
                
	            pthread_mutex_unlock(&mtx);
	            strp=link;
	            while ((token = strtok_r(strp, "/", &strp))){       /*get string until first slash*/
	                
	                if(counter==0){                                          /*create directory */
	                   strcpy(directoryopen,targsptr->directory);
	                   strcat(directoryopen,token);
	                   
	                   mkdir(directoryopen,S_IRUSR | S_IWUSR | S_IXUSR);
	                   counter++;

	                }
	                else if(counter==1){                                /*open file */
	                    strcat(directoryopen,"/");
	                    strcat(directoryopen,token);
	                    fp = fopen(directoryopen, "ab+");
	                    counter++;

	                }
	            }
	            free(link);        
	          
	            if (connect(sock, serverptr, sizeof(server)) < 0)                   /*connect to the server */
	                perror_exit("connect");
	            

	             message=request(p->link);

	             if(message!=NULL){
	             	//printf("%s\n",message);
	                write(sock,message,strlen(message)+1);
	                   
	                 while(size=read(sock,buffer,4000)>0){
	                     if (size<0)
	                        perror_exit("read error");
	               		if (strlen(buffer)>0){
                           
	               			if(fp!=NULL){
	                            if(removeheader==0){

	                                htmlmessage = strstr (buffer,"\n\n");               /*go after header */
                                    if(htmlmessage){
                                 
                                            htmlmessage[strlen(htmlmessage)]= '\0';
                                    }
	                                if(htmlmessage!=NULL){
                                            if(strlen(htmlmessage)>3){                            
    	                                        fprintf(fp, "%s", htmlmessage+2);           /*ignore the new lines*/    
    	                                       removeheader=1;
                                            } 
	                                }
	                            }
	                            else
                                    if(strlen(buffer)>0)
	                                    fprintf(fp, "%s", buffer);



	                        }

	               		}
	                }
                    free(message);
	             } 
                 sz = ftell(fp);                         /*find size*/
	             fclose(fp);
	             close(sock);
	             pthread_mutex_lock(&mtx);
                 numbferofpages++;
                 tsize=tsize+sz;
	             listflag=0;
                 newhead=NULL;
                if((fp = fopen(directoryopen, "r")) == NULL) {       /*start to find links inside file */
                    printf("fopen error\n");
                 }
                 while(fgets(temp, 512, fp) != NULL) {              
                    if((strstr(temp, "href=")) != NULL) {                 
                        token=strtok(temp,"\"");
                        token=strtok(NULL,"\"");
                        current=addtolist(startoflist,token);        /*find href and ingore first two slashes to get the link */
                        if(listflag==0 && current!=NULL){             /*if first link comes and list is empty update head */
                            newhead=current;
                            listflag=1;          
                        }
                        
                    }
        
                }
                fclose(fp);
	             if(newhead!=NULL	)                         /*update the head*/
	             	if(head==NULL)
	             		head=newhead;
                 if(head==NULL && newhead==NULL){                /*if list is empty and no more incoming links,shutdown*/
                     flag=1;
                 }
	             pthread_mutex_unlock(&mtx);

	             pthread_cond_signal(&cond);

        }
        pthread_mutex_unlock(&mtx);

        
      pthread_cond_broadcast(&cond);
      printf("Thread exit\n");

 }




int main(int argc, char const *argv[])
{
    int shutdown=0,i,newsock,sock,index=1,port,cport,numberofthreads,argsflag=0;;
    const char *hostname,*directory,* startinglink; 
    cnode *cnodeptr;
    targs *targsptr;
    struct sockaddr_in server, client;
    socklen_t clientlen = sizeof(client);                            /*init server*/
    struct sockaddr *serverptr=(struct sockaddr *)&server;
    struct sockaddr *clientptr=(struct sockaddr *)&client;
    struct hostent *rem;
    pthread_t *tid;
    char crawlerbuffer[500],tempbuffer[50],*cut;
    targsptr=malloc(sizeof(targs));
    unsigned long int totaltime,min,hr,sec,size=0;
    if(argc!=12){
        printf("Wrong Arguments\n");
        printf("%d\n",argc );
        return -1;  
    }
    while(index<11){
        if(strcmp(argv[index],"-h")==0){
            targsptr->hostname=argv[index+1];
            //printf("%s\n",hostname);
            argsflag++;
        }
        if(strcmp(argv[index],"-p")==0){
            targsptr->port=atoi(argv[index+1]);
            //printf("%d\n",port);
            argsflag++;
        }
        if(strcmp(argv[index],"-c")==0){
            cport=atoi(argv[index+1]);
            //printf("%d\n",cport );
            argsflag++;
        }
        if(strcmp(argv[index],"-t")==0){
            numberofthreads=atoi(argv[index+1]);
            argsflag++;
            //printf("%d\n",numberofthreads );

        }
        if(strcmp(argv[index],"-d")==0){
            targsptr->directory=(argv[index+1]);
            argsflag++;
            //printf("%s\n",directory );

        }
        index+=2;

    }
    startinglink=argv[11];

    
    if(argsflag!=5){
            printf("wrong arguments\n");
            return -1;
    }

    cut=strstr(startinglink,"site");                         /*cut and get the name of content*/
    if(cut!=NULL){
        cut=cut-1;
        printf("%s\n",cut );
    }

   

    time_t start, current;
    start = time(NULL);  
    pthread_mutex_init(&mtx, 0);
    pthread_cond_init(&cond, 0);
    tid=malloc(numberofthreads*sizeof(pthread_t));      
    startoflist=createhead();
    if(cut)
        addtolist(startoflist,(char*)cut);
    head=startoflist->next; 
    for ( i = 0; i < numberofthreads; i++){
         pthread_create(&tid[i], NULL, troutine,(targs*)targsptr);       /*create threads and pass the arguments*/
     } 
     pthread_cond_signal(&cond); 
     if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)                   /*start crawler server*/
        perror_exit("socket");
    server.sin_family = AF_INET;       /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(cport);      
    if (bind(sock, serverptr, sizeof(server)) < 0)
        perror_exit("bind");
    /* Listen for connections */
    if (listen(sock, 5) < 0) perror_exit("listen");
    printf("Listening for connections to port %d\n", cport);
    while(shutdown==0){
        if ((newsock = accept(sock, clientptr, &clientlen)) < 0) perror_exit("accept");
            
            size=read(newsock,crawlerbuffer,480);
            
            if(size<=0){
                
                close(newsock);
            }
            else{
                if(size>3){
                     if(crawlerbuffer[strlen(crawlerbuffer)-2]=='\n'){
           
                            crawlerbuffer[strlen(crawlerbuffer)-3]='\0';
                           
           
                     }
                }
                if(crawlerbuffer[strlen(crawlerbuffer)-1]=='\n'){
           
                            crawlerbuffer[strlen(crawlerbuffer)-2]='\0';
                           
           
                }
                if(strcmp(crawlerbuffer,"SHUTDOWN")==0){
                    printf("Crawler shutdown\n");
                    shutdown=1;
                    close(newsock);
                }
                printf("Crawler got command:%s\n",crawlerbuffer);                                /*stats command*/
                if(strcmp(crawlerbuffer,"STATS")==0 || strcmp(crawlerbuffer,"STATS\n")==0){
                    memset(crawlerbuffer, 0, 500*sizeof(*crawlerbuffer));
                    current = time(NULL);
                    totaltime = ((long int) (current-start));
                    hr = totaltime/3600;
                    totaltime = totaltime%3600;
                    min = totaltime/60;
                    totaltime = totaltime%60;
                    sec = totaltime;
                    strcpy(crawlerbuffer,"Crawler up for ");
                    sprintf(tempbuffer, "%lu",hr);
                    strcat(crawlerbuffer,tempbuffer);
                    sprintf(tempbuffer, "%lu",min);

                    strcat(crawlerbuffer,":");
                    strcat(crawlerbuffer,tempbuffer);
                    sprintf(tempbuffer, "%lu",sec);
                    strcat(crawlerbuffer,":");
                    strcat(crawlerbuffer,tempbuffer);
                    strcat(crawlerbuffer," ,donwloaded pages ");
                    sprintf(tempbuffer, "%d",numbferofpages);
                    strcat(crawlerbuffer,tempbuffer);
                    sprintf(tempbuffer,"%lu",tsize);
                    strcat(crawlerbuffer,",");
                    strcat(crawlerbuffer,tempbuffer);
                    strcat(crawlerbuffer," bytes");
                    strcat(crawlerbuffer,"\n");
                    write(newsock,crawlerbuffer,strlen(crawlerbuffer)+1);
                    close(newsock);
                    



                }
            }
            close(newsock);
            memset(crawlerbuffer, 0, 500*sizeof(*crawlerbuffer));

           

        

    }

     for (int j = 0; j < numberofthreads; j++)           /*wait for threads to finish*/
       pthread_join(tid[j], NULL);
    
    free(tid);
    

    free(targsptr);
    deletelist(startoflist);
    return 0;
}