#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include"DIAVASMA.h"
#include<errno.h>
#include<signal.h>
#include <poll.h>

int flag=0;
void  ALARMhandler(int sig)
{
  signal(SIGALRM, SIG_IGN);
  flag=1;          /* ignore this signal       */
  
  signal(SIGALRM, ALARMhandler);   

}


int main(int argc, char const *argv[])
{	
	FILE *fp;
	int *fd,*fdr,number=0,arraysize=0,j=0,flagstop,maxtimes=0,mintimes=666666,timeout,counterofworkers=0;
	char **patharray,*myfifo="jobtoworker",*myfifo2="workertojob",*maxpath,*minpath,timeoutstr[20],dstr[5];
	pid_t pid,wpid;
    pidarray *pidarrayptr;
	number=atoi(argv[2]); 
    int status =0,pathstoread=1;
    ssize_t nwrite,nread,size=0,lines=0,words=0;
  

   signal(SIGALRM, ALARMhandler);
 

	if ((fp=fopen(argv[1], "r")) == NULL)
     {
          printf("Error! opening file");
          
          return(-1);         
     }
     arraysize=diavasma(fp);
     fclose(fp);


	if ((fp=fopen(argv[1], "r")) == NULL)
     {
          printf("Error! opening file");
          
          return(-1);         
     }  
     if(arraysize<number)
        number=arraysize;
     pidarrayptr=malloc(sizeof(pidarray));
     pidarrayptr->array=malloc(number*sizeof(pid_t));
     patharray=(char**)malloc(arraysize*(sizeof(char*)));
     eisagwgh(fp,patharray,arraysize);
     fclose (fp);
     fd=malloc(number*sizeof(int));
     fdr=malloc(number*sizeof(int));
     char *searchmessage;
     searchmessage=malloc(100*sizeof(char));
	   forker(number,0,pidarrayptr);
	
        for(int i=1;i<=number;i++){
			 	
		 	char *str=(char*)malloc(5*sizeof(char));
		 	sprintf(str, "%d",i);
		 	char * str3 = (char *) malloc(1 + strlen(str)+ strlen(myfifo));
	 		strcpy(str3,myfifo);
	 		strcat(str3,str);                                                                 //anoikse ta pipes
            char * str4 = (char *) malloc(1 + strlen(str)+ strlen(myfifo2));
            strcpy(str4,myfifo2);
            strcat(str4,str);
	 						
		 	if( (fd[i-1]=open(str3,O_WRONLY)) < 0)
						{ perror("pipe  open  error\n");
						exit(1); 
			}
            if( (fdr[i-1]=open(str4,O_RDONLY)) < 0)
                        { perror("pipe  open  error\n");
                        exit(1); 
            }
            

            //kill(pidarrayptr->array[i-1],SIGSTOP);
					
		
				


						
		}
		
	
         for (int i=1; i<=number;  i++){
                 
          if  (( nwrite=write(fd[i-1], patharray[i-1] , strlen(patharray[i-1]) +1)) ==  -1){ 
                    perror("Error  in  Writing");
                    exit(2);
           } 
            //printf("parent wrote message %s\n",patharray[i-1]);
         }  
        char *childmessage;
        childmessage=malloc(4096*sizeof(char));
        
      for (int i=1; i<=number;  i++){
            
            nread=read(fdr[i-1],childmessage,100);
            //printf("parent read %lu bytes and message is %s\n",nread,childmessage);
       
       }
       int counter=number;
       if(number==arraysize)
           flagstop=1;
      else
          flagstop=0;
       while(flagstop==0){
       		for(int i=1; i<=number; i++){
       			  if  (( nwrite=write(fd[i-1], patharray[counter] , strlen(patharray[counter]) +1)) ==  -1){ 
                    perror("Error  in  Writing");
                    exit(2);
           	    	}
           	    	//printf("Parent wrote message: %s\n",patharray[counter]); 


           	if((nread=read(fdr[i-1],childmessage,100))==-1){

           		perror("Error in reading");
           		exit(2);

           	}
           	counter++;
           	if(counter==arraysize){
           		flagstop=1;
           		break;

           	}


       		}





       }
        
        /*while(j<number){        
            kill(pidarrayptr->array[j],SIGUSR1);
            j++;

        }
        */
        
       int k;
          char  *stopreading="***stopreading***";
          for (int i=1; i<=number;  i++){
                 
            if  (( nwrite=write(fd[i-1], stopreading ,strlen(stopreading) +1)) ==  -1){ 
                      perror("Error  in  Writing");                                         //wra gia tis entoles
                      exit(2);
             } 
            //printf("parent wrote message %s\n",patharray[i-1]);
         }
        
        childmessage=malloc(100*sizeof(char));
        
      for (int i=1; i<=number;  i++){
            
            nread=read(fdr[i-1],childmessage,100);
            //printf("parent read %lu bytes and message is %s\n",nread,childmessage);
       
       }
    char buffer[100],buffer2[100];
    
    scanf("%s",buffer);
    scanf("%[^\n]s",buffer2);
   
                while(strcmp(buffer,"/exit")!=0){

                    if(strcmp(buffer,"/max")==0){
                      
                      if(strlen(buffer2)!=0 ){
                        char *token = strtok(buffer2,"  ");
                       
                        for(int i=1; i<=number; i++){
                          if  (( nwrite=write(fd[i-1],buffer, strlen(buffer)+1)) == -1){ 
                            perror("Error  in  Writing");
                            exit(2);
                          }

                          if((nread=read(fdr[i-1],childmessage,100))==-1){

                          perror("Error in reading");
                          exit(2);   
                          }
                         }  
                         for(int i=1; i<=number; i++){
                            if  (( nwrite=write(fd[i-1],token, strlen(token)+1)) == -1){ 
                              perror("Error  in  Writing");
                              exit(2);
                            }
                            if((nread=read(fdr[i-1],childmessage,100))==-1){

                            perror("Error in reading");
                            exit(2);   
                            }
                            
                            char *token = strtok(childmessage,"*");
                            int times=atoi(token);
                            token = strtok(NULL, " \n \t");
                            if(times>=maxtimes){
                              if(times==maxtimes){
                                if(strcmp(token,maxpath)<0){
                                  maxpath=malloc(strlen(token)+1);
                                  maxtimes=times;
                                  strcpy(maxpath,token);

                                }
                              }
                              else {
                                
                                maxpath=malloc(strlen(token)+1);
                                maxtimes=times;
                                strcpy(maxpath,token);


                              }

                            }
                            

                        }
                        if(maxtimes==0)
                          printf("Can't find the word in all workers\n");
                        else 
                          
                         printtext(maxpath);
                        maxtimes=0;  

                       }
                      }
                      if(strcmp(buffer,"/min")==0){
                        if(strlen(buffer2)!=0 ){
                        char *token = strtok(buffer2,"  ");
                      
                        for(int i=1; i<=number; i++){
                          if  (( nwrite=write(fd[i-1],buffer, strlen(buffer)+1)) == -1){ 
                            perror("Error  in  Writing");
                            exit(2);
                          }

                          if((nread=read(fdr[i-1],childmessage,100))==-1){

                          perror("Error in reading");
                          exit(2);   
                          }
                         }  
                         for(int i=1; i<=number; i++){
                            if  (( nwrite=write(fd[i-1],token, strlen(token)+1)) == -1){ 
                              perror("Error  in  Writing");
                              exit(2);
                            }
                            if((nread=read(fdr[i-1],childmessage,100))==-1){

                            perror("Error in reading");
                            exit(2);   
                            }
                            //printf("%s\n",childmessage );
                            char *token = strtok(childmessage,"*");
                            int times=atoi(token);
                            //printf("%d\n",times );
                            token = strtok(NULL, " \n \t");

                            if(times<=mintimes && times!=0){
                              if(times==mintimes){
                                if(strcmp(token,minpath)<0){
                                  minpath=malloc(strlen(token)+1);
                                  mintimes=times;
                                  strcpy(minpath,token);


                                }
                              }
                              else {
                                
                                minpath=malloc(strlen(token)+1);
                                mintimes=times;
                                strcpy(minpath,token);                          

                              }

                            }
                            

                        }
                  
                        if(mintimes==666666)
                          printf("Can't find the word in all workers\n");
                        else 
                          printtext(minpath);
                        mintimes=666666;  

                       }
                      }
                      if(strcmp(buffer,"/wc")==0){
                        for(int i=1; i<=number; i++){
                          if  (( nwrite=write(fd[i-1],buffer, strlen(buffer)+1)) == -1){ 
                            perror("Error  in  Writing");
                            exit(2);
                          }
                          
                          if((nread=read(fdr[i-1],childmessage,100))==-1){

                          perror("Error in reading");
                          exit(2);   
                          }
                          char *token = strtok(childmessage,"*");
                          //printf("%s\n",token );
                          size=size+atoi(token);
                          //printf("childmessage is %s\n", childmessage);
                          token = strtok(NULL, "*");                        
                          lines=lines+atoi(token);
                          token=strtok(NULL,"\t \n");
                          words=words+atoi(token);

                          

                       
                        }
                        printf("Total size: %lu bytes,total lines: %lu,total words: %lu \n",size,lines,words);
                        size=0;
                        lines=0;
                        words=0;

                       }
                       if(strcmp(buffer,"/search")==0){
                          
                         struct pollfd fds[number];
                         for(int i=0; i<number; i++){
                                      fds[i].fd=fdr[i];
                                      fds[i].events=0;
                                      fds[i].events |=POLLIN;                         
                         } 
                         int searchflag=0;
                          if(strlen(buffer2)!=0 ){
                              char *buffer2cpy;
                              buffer2cpy=malloc(strlen(buffer2)+1);
                              strcpy(buffer2cpy,buffer2);
                              
                                char *token = strtok(buffer2," ");
                                int strcounter=0;         
                                while(token!=NULL){
                                  strcounter++; 
                                  token=strtok(NULL," ");          
                                }                              
                                if(strcounter<=2 || strcounter>10){
                                  printf("Wrong arguments for search\n");
                                  searchflag=1;

                                }
                                else{
                                    int secondcounter=0;
                                    token=strtok(buffer2cpy," ");
                                    secondcounter++;
                                    
                                    strcpy(searchmessage,"");  
                                    while(token!=NULL && secondcounter<strcounter-1){                                        
                      
                                        strcat(searchmessage,token);
                                        strcat(searchmessage," ");
                                        token=strtok(NULL," ");
                                        secondcounter++;

                                    }
                                   
                                }
                            
                             if(searchflag==0){
                               strcpy(dstr,token);
                                    token=strtok(NULL," ");                                  
                                    timeout=atoi(token);
              
                                  for(int i=1; i<=number; i++){
                                    if  (( nwrite=write(fd[i-1],buffer, strlen(buffer)+1)) == -1){ 
                                      perror("Error  in  Writing");
                                      exit(2);
                                    }                        
                                 }
                                 for(int i=0; i<number; i++){
                                  if((nread=read(fdr[i],childmessage,100))==-1){
                                     perror("Error in reading");
                                     exit(2);   
                                   }
                                   
                                  }
                                  
                                        
                                  int ret;
                    
                  
                             
                              for(int i=1; i<=number; i++){
                                  if  (( nwrite=write(fd[i-1],searchmessage, strlen(searchmessage)+1)) == -1){ 
                                    perror("Error  in  Writing");
                                    exit(2);
                                  }
                               }
                                  int i=0;
                                  flag=0;
                                 alarm(timeout);
                                 while(flag==0){
                                  poll(fds,number,0);
                                

                                 }
                                 int counterofworkers=0;
                                 for(int i=0; i<number; i++){
                                  if(fds[i].revents==POLLIN){
                                        
                                      counterofworkers++;

                                  }

                                 }


                                 flag=0;
                                 printf("Number of workers answered %d\n",counterofworkers); 
                                  for(int i=0; i<number; i++){
                                  if(fds[i].revents==POLLIN){
                                        memset(childmessage,0,strlen(childmessage));  
                                       if((nread=read(fdr[i],childmessage,3300))==-1){
                                            perror("Error in reading");
                                            exit(2);   
                                        }
                                       
                                        printf("%s",childmessage);
                                        //fflush(stdout);
                                        memset(childmessage,0,strlen(childmessage));
                                      
                                      
                                  }

                                 }
                                 while (counterofworkers<number){
                                    poll(fds,number,0);
                                    for(int i=0; i<number; i++){
                                      if(fds[i].revents==POLLIN){
                                          counterofworkers++;
                                         if((nread=read(fdr[i],childmessage,3300))==-1){
                                              perror("Error in reading");
                                              exit(2);   
                                          }
          
                                          memset(childmessage,0,strlen(childmessage));
                                        
                                        
                                      }
                                    }
                                    


                                 }
                                 counterofworkers=0;

                                 
                             

                                }

                                
                                
                          }
                          
                         

                       }

				          memset(buffer2,0,strlen(buffer2));
                  memset(buffer,0,strlen(buffer));  
                  
				          printf("\n");
                  scanf("%s",buffer);
				  
                  scanf("%[^\n]s",buffer2);
                    

                }
              
             

                 for(int i=1; i<=number; i++){
                          if  (( nwrite=write(fd[i-1],buffer, strlen(buffer)+1)) == -1){ 
                            perror("Error  in  Writing");
                            exit(2);
                          }
                  }        

                  printf("Programm is going to exit\n");

        



        

    
        
        while ((wpid = wait(&status)) > 0);
        for(int i=0; i<number; i++){

            close(fd[i]);
            close(fdr[i]);

        }
        free(fd);
 
        free(fdr);
       
        //printf("arraysize is %d\n",arraysize );
        for(int i=0; i<arraysize; i++){
         
          free(patharray[i]);


        }
        
         free(pidarrayptr->array);
        
        free(patharray);
       
        
        free(pidarrayptr);
        
      
        
         
      
        
	return 0;
}