#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include "read.h"
#include <signal.h>

int main(int argc, char const *argv[])
{
    char idstring[10],startstring[50],heightstring[10],endstring[50],pipecounterstring[30],depthstring[10],filename[50],pattern[50],str[30],*pipebasename="pipe",nameofpipetocreate[50],buffer[200],pipetowrite[50],levelstring[30];
    int numberofrecords,wheretostart=0,numberofchildren=1,sum=0,needtoread=0,span,startrange1,startrange2,endrange1,endrange2,sflag=0,start,end,lvl,fdw,savedcounter,status=0,depth,fdarray[2],pipescounter=0;
    record_t rec;
    pipe_record_t piperec;
    info_t info,infotowrite;
    clock_t start_t, end_t;
    float total_t;
    start_t = clock();
    //   for(int i=0; i<argc; i++){
    //     printf("%s  ",argv[i]);
    // }
    // printf("\n");
    //init args needed
    strcpy(filename,argv[1]);   //data:filename
    pipescounter=atoi(argv[2]); //pipe number so far
    savedcounter=pipescounter;    //save the pipe counter
    depth=atoi(argv[3]);            //depth
    strcpy(heightstring,argv[10]);   //height
    strcpy(pipetowrite,argv[4]);    //pipe to write data to parent
    lvl=atoi(argv[5]);              //lvl
     strcpy(pattern,argv[8]);       //string to search for
    start=atoi(argv[6]);            //start range
    end=atoi(argv[7]);              //end range
    sflag=atoi(argv[11]); //sflag
    numberofrecords=atoi(argv[12]);
    span=(end-start)/2;             //span of search
    for (int i=1; i<=atoi(heightstring); i++ ){
        numberofchildren=numberofchildren*2;
    }
    if(sflag==0){                   //if s flag is false
                startrange1=start;          //startrange for first child
                endrange1=start+span;        //end range for first child
                startrange2=start+span;     //start range for the second child
                endrange2=end;              //end range for the second child
                if((end-start)%2!=0)
                    startrange2++;
                
    }else{
        if(depth>1){ 
          
                                                   //if we are before leafs && sflag==1
            startrange1=start; 
            endrange1=start+(end-start)/2;
            startrange2=endrange1+1;
            endrange2=end;
           
            
            
        }else{                  //if we are depth =1 and flag s is on
            
            for (int j = 1 ; j <=numberofchildren ; j++){           //calcuate the sum to divide with
                sum+=j;
                
            }
            for (int j = 1 ; j <start; j++){            //calcuate how many records have been read so far and start after that to read 
               wheretostart+=(j*numberofrecords)/sum;
               
            }
         
            
            startrange1=wheretostart;
            endrange1=wheretostart+ start*numberofrecords/sum;          //endrange wheretostart + the range (using the given formula)
            wheretostart=0;
            for (int j = 1 ; j <end; j++){
               wheretostart+=j*numberofrecords/sum;
                
            }
            
            startrange2=wheretostart;
            endrange2=wheretostart+end*numberofrecords/sum;
            if(end==numberofchildren){                      //if you are last search ,search also for something that have not been taken care of
                while(endrange2<numberofrecords){
                    endrange2++;
                }
            
            }
           
        }
        
    }
    
    pid_t pid,wpid;
    size_t twi=0,ni=0,wi=0,n=0,tnw=0,nw=0,tn=0;
    infotowrite.splitNmergetime=0;
    infotowrite.maxsplitmerge=0;
    infotowrite.minsplitmerge=0;
    infotowrite.maxsearcher=0;
    infotowrite.minsearcher=0;
    infotowrite.searcherstime=0;
    //create two pipes to communicate with childern
    for(int j=1; j<3; j++){
                strcpy(nameofpipetocreate,pipebasename);
                sprintf(str, "%d",j+pipescounter);
                strcat(nameofpipetocreate,str);
                if(mkfifo(nameofpipetocreate , 0666)==-1){      //create pipe,if error exit
                    if(errno==EEXIST){
                        perror("cant create pipe");
                        exit(2);
                    }
                }
    }
    //depending on which level you are,add how many pipes have been created because of this stage(so we dont have same pipe names)
    if(lvl==1){ 
        pipescounter=pipescounter+2;
    }
    if(lvl==2){    
        pipescounter=pipescounter+4;
    }
     if(lvl==3){
        pipescounter=pipescounter+8; 
     }
     if(lvl==4){
         pipescounter=pipescounter+16;
     } 
                         
    if(depth>1){                            //if we need to split 
         for(int j=1; j<3; j++){
            if ((pid = fork()) < 0){
                perror("fork failed\n");
            }
            else if(pid==0){                            //when child
                sprintf(levelstring,"%d",lvl+1);        //lvl
                sprintf(depthstring,"%d",depth-1);     //depth
                sprintf(pipecounterstring, "%d",pipescounter); 
                strcpy(nameofpipetocreate,pipebasename);
                sprintf(str, "%d",j+savedcounter);
                strcat(nameofpipetocreate,str); //pipe that child is going to  get
                if(j==1){
                    sprintf(startstring,"%d",startrange1);  //first child range to search
                    sprintf(endstring,"%d",endrange1);
                }else if(j==2){
                    sprintf(startstring,"%d",startrange2);  //second child range to search
                    sprintf(endstring,"%d",endrange2);
                }
                char *args[]={"./splitNmerge",filename,pipecounterstring,depthstring,nameofpipetocreate,levelstring,startstring,endstring,pattern,(char*)argv[9],
                heightstring,(char*)argv[11],(char*)argv[12],NULL}; //pass args and exec splitNmege
                execvp(args[0],args);
            }
            //depending on which level you are,add how many pipes have been created because of this stage(so we dont have same pipe names)
            if(lvl==1)
                pipescounter+=2;
             else if(lvl==2)
                pipescounter+=4 ;
            else  if(lvl==3)
                pipescounter+=8;    
            else if(lvl==4)
                pipescounter+=16;    
         }


    }
    if(depth==1){                   //you need to exec leafs now
    
        
        for(int j=1; j<3; j++){
            if ((pid = fork()) < 0){
                perror("fork failed\n");
            }
            else if(pid==0){                        //when child
                strcpy(nameofpipetocreate,pipebasename);
                sprintf(str, "%d",j+savedcounter);
                strcat(nameofpipetocreate,str);        //create the name of the pipe for child to communicate
                if(j==1){
                    if(sflag==1){
                        sprintf(idstring,"%d",start);
                    }
                    sprintf(startstring,"%d",startrange1);  //range for first child
                    sprintf(endstring,"%d",endrange1);
                }else if(j==2){
                    sprintf(startstring,"%d",startrange2);  //range for the second shild
                    sprintf(endstring,"%d",endrange2);
                    if(sflag==1){
                        sprintf(idstring,"%d",end);
                    }
                }      //if you are a child
                char *args[]={"./leaf",filename,startstring,endstring,str,nameofpipetocreate,pattern,(char*)argv[9],NULL};  //pass args and exec leaf
                execvp(args[0],args);

            }
           
         }    
         
    } 
    if( (fdw=open(pipetowrite,O_WRONLY)) < 0) {         //open pipe to write to parent
                perror("pipe  open  error\n");
                exit(1);              
        }
    for(int j=1; j<3; j++){                                //for the two pipes
        strcpy(nameofpipetocreate,pipebasename);
        sprintf(str, "%d",j+savedcounter);
        strcat(nameofpipetocreate,str); 
        if( (fdarray[j-1]=open(nameofpipetocreate,O_RDONLY)) < 0) {     //open pipe to read data from children
                perror("fd  open  error\n");
                exit(1);              
        }
    }  
    for(int j=1; j<3; j++){                     //for the two fds open for reading
        
        while((n= read( fdarray[j-1] ,&piperec, sizeof(pipe_record_t)))>0){   //while there is data to read
           // printf("N:%ld\n",n);
            if(piperec.flaglast==1 ){         //we need only one info node (because we are merging )
                if(j==1)
                    piperec.flaglast=2;     //flag with 2 so we dont care about that record            
                needtoread=1;
            }                
            while(tnw<n){                               //while data writen is lass than read

                    nw=write(fdw,&piperec,sizeof(pipe_record_t));
                    tnw+=nw;
                              
            }
            //printf("%ld %s %s  %s %d %s %s %-9.2f %d \n", piperec.custid, piperec.LastName, piperec.FirstName, piperec.Street, piperec.HouseID, piperec.City, piperec.postcode, piperec.amount,piperec.flaglast);
            tnw=0;
            if(needtoread==1){      //if you need to read info node 
                needtoread=0;
                while((ni=read( fdarray[j-1] ,&info, sizeof(info_t)))>0){       //read and calculate min/max etc 
                     if(depth==1){
                        infotowrite.splitNmergetime+=info.time;
                        if(info.time>infotowrite.maxsearcher){
                            infotowrite.maxsearcher=info.time;
                        }
                        if(infotowrite.minsearcher==0){
                            infotowrite.minsearcher=info.time;
                        }else{
                            if(info.time<infotowrite.minsearcher){
                                infotowrite.minsearcher=info.time;
                            }
                        }
                        infotowrite.searcherstime+=info.time;
                    
                    
                     }
                     else{
                        if(infotowrite.minsplitmerge==0){
                            infotowrite.minsplitmerge=info.splitNmergetime;
                        }else{
                            if(info.minsplitmerge<infotowrite.minsplitmerge){
                                infotowrite.minsplitmerge=info.minsplitmerge;
                            }
                        }
                        if(info.maxsplitmerge>infotowrite.maxsplitmerge){
                            infotowrite.maxsplitmerge=info.maxsplitmerge;
                        }
                        
                        if(info.maxsearcher>infotowrite.maxsearcher){
                            infotowrite.maxsearcher=info.maxsearcher;
                        }
                        if(infotowrite.minsearcher==0){
                            infotowrite.minsearcher=info.minsearcher;
                        }else{
                            if(info.minsearcher<infotowrite.minsearcher){
                                infotowrite.minsearcher=info.minsearcher;
                            }
                        }
                        
                        infotowrite.splitNmergetime+=info.splitNmergetime; 
                        infotowrite.searcherstime+=info.searcherstime; 
                        
                     }//printf("infotime: %f\n",info.time);
                    if(j==2){
                        end_t=clock();
                        total_t=(double)(end_t-start_t)/CLOCKS_PER_SEC;
                        //printf("Split merge time %f\n",total_t);
                        infotowrite.splitNmergetime+=total_t;
                        if(depth==1){
                            infotowrite.maxsplitmerge=infotowrite.splitNmergetime;
                            infotowrite.minsplitmerge=infotowrite.splitNmergetime;
                        }else{
                            if(infotowrite.splitNmergetime>infotowrite.maxsplitmerge){
                                infotowrite.maxsplitmerge=infotowrite.splitNmergetime;
                            }
                            

                        }
                        while(twi <sizeof(infotowrite)){
                            wi=write(fdw,&infotowrite,sizeof(info_t));
                            twi+=wi;
                        }
                    }
                }
               
                twi=0;
                
            }                  
        }
     
         

    } 

    while ((wpid = wait(&status)) > 0); //wait for children
    for(int j=1; j<3; j++){         //destroy the pipes
        close(fdarray[j-1]);
        strcpy(nameofpipetocreate,pipebasename);
        sprintf(str, "%d",j+savedcounter);
        strcat(nameofpipetocreate,str);
        unlink(nameofpipetocreate);
    }         
    close(fdw);         //close the fd of writing
    return 0;
}
