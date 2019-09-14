
#define _POSIX_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<signal.h>
#include "TRIE.h"
#include <dirent.h>
#include"DIAVASMA.h"
#include <signal.h>

int MessageFlag=0;
void handler(int signo){

	MessageFlag++;
	// /printf("got the signal\n");

}

int main(int argc, char const *argv[])
{
	int id;
	char *myfifo="jobtoworker",*myfifo2="workertojob";
	ssize_t nread,nwrite,size=0,numberofwords=0,numberoflines=0;
	int fd,fdw,arraysize,stopflag=0;
	FILE *fp;
	trieNode_t *root,*found;
	fileinfo *head;
    trieVal_t  currentnodelist;
    char message[4096];

	char *str = (char*) malloc(1+strlen(argv[1])+strlen(myfifo));
	char *str2 = (char*) malloc(1+strlen(argv[1])+strlen(myfifo2));
	strcpy(str,myfifo);
	strcat(str,argv[1]);

	strcpy(str2,myfifo2);
	strcat(str2,argv[1]);
    
	char *path=(char*)malloc(4096*sizeof(char));
	struct sigaction mySigActions;
    mySigActions.sa_handler = handler;
	

	// printf("Child with id %d created \n",getpid());

	 DIR *d;
     struct dirent *dir;
      

   
	;
	if( (fd=open(str,O_RDONLY)) < 0)
		{ perror("pipe  open  error\n");
		exit(1); 
	}
																	//anoikse ta pipes

    if( (fdw=open(str2,O_WRONLY)) < 0)
		{ perror("pipe  open  error\n");
		exit(1); 
	}
		

  		char **arraypath;
  		int counter=0;													//dimiourgise to trie 
  		TrieCreate(&root);
  		
  		char **textmap,**secondtextmap;
	 	fileinfo *nodetoinsert;
	 	fileinfo *startnode;
	 	int readcounts=0;
	    fileinfo **structarray;
        while(stopflag==0){ 
	     	     nread=read(fd,path,40);
	         if (sigaction(SIGUSR1, &mySigActions, NULL) < 0) {
			     printf("Sigaction failed for SIGHUP");
			     exit(1);														
	         }

	       		arraypath=malloc(sizeof(char*));
	       		arraypath[counter]=malloc((strlen(path)+1)*sizeof(char));
	       		strcpy(arraypath[counter],path);
	       		structarray=malloc(sizeof(fileinfo**));
	       		structarray[counter]=malloc(sizeof(fileinfo*));
	       	    head=NULL;
		     	//printf("child %d Nread: %lu  and message is %s \n",getpid(),nread,path);
		     strcpy(message,"ready");
		     if  (( nwrite=write(fdw,message, strlen(message) +1)) ==  -1){ 
	                    perror("Error  in  Writing");
	                    exit(2);
	         }
	         if(strcmp(path,"***stopreading***")==0)
	         	break;


	         		
	         if((d = opendir(path))==NULL){

	         	printf("cant open path %s\n",path);
	         	exit(3);
	         }

	   		 if (d)
	   		 {
	        	while ((dir = readdir(d)) != NULL)												//anoikse ola ta perioxomena toy path 
	        	{
	        	   if(strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0){
		 	        	char *pathname;
		 	        	pathname=malloc(strlen(path)+strlen(dir->d_name)+1);
		 	        	strcpy(pathname,path);
		 	        	strcat(pathname,dir->d_name);

		 	        	

		 	        	if ((fp=fopen(pathname,"r")) == NULL)
		    			 {
		         			 printf("Error! opening file %s\n",path);
		          
		          			return(-1);         

		 	        	 }
		 	        	 //printf("opening path: %s\n",pathname );
		 	        	arraysize=diavasma(fp);
		 	        	//printf("arraysize is %d\n",arraysize );
		     			fclose(fp);
		     			if ((fp=fopen(pathname,"r")) == NULL)
		    			 {
		         			 printf("Error! opening file %s\n",path);
		          
		          			return(-1);         

		 	        	 }

		 	        	 
		 	        	 textmap=(char**)malloc(arraysize*(sizeof(char*)));
		 	        	 secondtextmap=(char**)malloc(arraysize*(sizeof(char*)));
		 	        	 eisagwgh(fp,textmap,arraysize);
		 	        	 fclose(fp);
		 	        	 	if ((fp=fopen(pathname,"r")) == NULL)
		    			 {
		         			 printf("Error! opening file %s\n",path);
		          
		          			return(-1);         

		 	        	 }
		 	        	 fseek(fp, 0, SEEK_END); // seek to end of file
                         size =size+ftell(fp); // get current file pointer
                         fseek(fp, 0, SEEK_SET); // seek back to beginning 
                         numberoflines=numberoflines+arraysize;
		 	      		eisagwgh(fp,secondtextmap,arraysize);
		 	      		fclose(fp);


		 	        	 nodetoinsert=fileinfocreate(arraysize,pathname,textmap);
		 	        	 if(head==NULL){
		 	        	 	head=insertliststart(nodetoinsert);

		 	        	 }else{
		 	        	 	fileinfoinsert(head,nodetoinsert);


		 	        	 }

		 	        	  
		 	        	  for(int i=0;i<arraysize;i++){
            
          						 char *token = strtok(secondtextmap[i], " \t");        //pairne leksi leksi to perioxomeno toy map
          						 while (token) {
           						
           	              					 
					              
					              			  TrieAdd(&root,token,i,pathname);
					              			  numberofwords=numberofwords+1;				              			  
					              			  //printf("Inserting token :%s \n",token);				//valtin sto trie 
					              			  token = strtok(NULL, " \n \t");
              	

              					}
              	                          				 

		      			}
		 	        	 

		     		}


	        	}

	        closedir(d);

    	}
    	
    	structarray[counter]=head;
    	counter++;

	     		 	     	
	    }
	    
	     
	 	while(1){
	 		     nread=read(fd,path,4096);

	 		     if (strcmp(path,"/max")==0){
	 		     	 strcpy(message,"ready");
	 		     	 if (( nwrite=write(fdw,message, strlen(message) +1)) ==  -1){ 
	                    perror("Error  in  Writing");
	                    exit(2);
	         	  	}	         	  	
	 		     
	 		    	 nread=read(fd,path,4096);
	 		    	 
		 		      char times[5];
		         	  found=NULL;

		       		  found=TrieSearch(root->children,path);
		       		  if(found==NULL){
		       		  		
		       		  	
		       		   		sprintf(times,"%d",0);
		       		       // message=malloc(strlen(times)+1);	
		       		        strcpy(message,times);
		       		        //message=malloc(strlen("The word doesnt exist in all workers")+2);
		       		        strcat(message,"*");
		       		        strcat(message,"The word doesnt exist in all workers");
		       		        
		       		     }   
		       		  if(found){
							currentnodelist=findmax(found->value);
		       		        sprintf(times,"%d",currentnodelist->times);
		       		        
		       		       // message=malloc(strlen(times)+2+strlen(currentnodelist->text));

		       		        strcpy(message,times);
		       		        strcat(message,"*");
		       		        strcat(message,currentnodelist->text);

		       		        

		       		}

		       		 if  (( nwrite=write(fdw,message, strlen(message) +1)) ==  -1){ 
		                    perror("Error  in  Writing");
		                    exit(2);
		         	  }
		         	  

	       		}
	       		 if (strcmp(path,"/min")==0){
	 		     	 strcpy(message,"ready");
	 		     	 if (( nwrite=write(fdw,message, strlen(message) +1)) ==  -1){ 
	                    perror("Error  in  Writing");
	                    exit(2);
	         	  	}	         	  	
	 		     
	 		    	 nread=read(fd,path,4096);
	 		    	 
		 		      char times[5];
		         	  found=NULL;
		       		  found=TrieSearch(root->children,path);
		       		  if(found==NULL){
		       		   		sprintf(times,"%d",0);
		       		       // message=malloc(strlen(times)+1);
		       		        strcpy(message,times);
		       		        //message=malloc(strlen("The word doesnt exist in all workers")+2);
		       		        strcat(message,"*");
		       		        strcat(message,"The word doesnt exist in all workers");
		       		     }   
		       		  if(found){
							currentnodelist=findmin(found->value);
							
		       		        sprintf(times,"%d",currentnodelist->times);
		       		       // message=malloc(strlen(times)+2+strlen(currentnodelist->text));
		       		        strcpy(message,times);
		       		        strcat(message,"*");
		       		        strcat(message,currentnodelist->text);
		       		        

		       		}

		       		 if  (( nwrite=write(fdw,message, strlen(message) +1)) ==  -1){ 
		                    perror("Error  in  Writing");
		                    exit(2);
		         	  }

	       		}
	       		if (strcmp(path,"/wc")==0){
	       			
	       			char sizecount[50];
	       			char tempsize[30];
	       			sprintf(sizecount,"%lu",size);
	       			strcat(sizecount,"*");
	       			sprintf(tempsize,"%lu",numberoflines);
	       			strcat(sizecount,tempsize);
	       			sprintf(tempsize,"%lu",numberofwords);
	       			//printf("numberofwords is: %lu and string is %s\n",numberofwords,tempsize );
	       			strcat(sizecount,"*");
	       			strcat(sizecount,tempsize);
	       			//printf("%s\n",sizecount);
	       			 if  (( nwrite=write(fdw,sizecount, strlen(sizecount) +1)) ==  -1){ 
		                    perror("Error  in  Writing");
		                    exit(2);
		         	  }





	       		}
	       		if(strcmp(path,"/search")==0){
	       			 
	       			 strcpy(message,"readyforsearch");
	 		     	   if (( nwrite=write(fdw,message, strlen(message) +1)) ==  -1){ 
		                    perror("Error  in  Writing");
		                    exit(2);
	         	  	    }

	         	  		int searchflag=0;
	         	  		nread=read(fd,path,4096);
	         	  		trieVal_t temp;  	  		
	         	  		char *token=strtok(path," ");  		
	         	  		strcpy(message,"");   
	         	  		while(token!=NULL){
	         	  			//printf("%s\n",token);
	         	  			found=NULL;
		       		 		found=TrieSearch(root->children,token);
		       		 		if(found!=NULL){
		       		 			temp=found->value;
		       		 			
		       		 			while(temp){			
		       		 				putresults(temp,message);
		       		 				temp=temp->next;

		       		 				
		       		 			}
		       		 			//printf("%s\n",message );
		       		 			searchflag=1;
		       		 		}
		       		 		token=strtok(NULL," ");
		       		 	 }
		       		 	 	//printf("%s\n",message);
	         	  		    
		       		 	 	if(searchflag==0){
		       		 	 		memset(message,0,strlen(message));
		       		 	 		
		       		 	 		strcpy(message,"  ");
		       		 	 		
		       		 	 	}	
		       		 	 	
		       		 	 	

		       		 	 
		     		 	 
		       		 	 if (( nwrite=write(fdw,message, strlen(message) )) ==  -1){ 
	                   			 perror("Error  in  Writing");
	                   			 exit(2);
	         	  		 	}
	         	  		 	//printf("nwrite %lu\n",nwrite);

		       		 	
		         		
		         			    			
				}	   	 	
	       		if(strcmp(path,"/exit")==0){

	       			
	       			break;
	       		}



	 	}

     

     //printf("Child with id %d finished \n",getpid());
    
    // fclose(fp);
   	 for(int i=0; i<counter; i++){
   	 	free(arraypath[counter]);
   	 	free(structarray[counter]);

   	 }

   	 free(structarray);
   	 free(arraypath);
     close(fdw);
     close(fd);
     unlink(str);
     unlink(str2);
     free(str);
     free(str2);
     free(path);
     listdestroy(root);
     TrieDestroy(root);
     printf("Child exit\n");
	return 0;
}