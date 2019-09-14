#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <math.h> 
#include "read.h"

volatile int SIGUSR1counter=0;

 void my_handler(int signum)        //set up handler for SIGUSR1    
{

    if(signum==SIGUSR1){

        SIGUSR1counter++;
    }
}

int main(int argc, char const *argv[])
{
    int numferofsplitNmerge=0,numberofrecords,sflag=0,sort[1],numberofchildren=1,counter=0,fd,i,status=0,height,pipescounter=0,lvl=1;
    char numberofrecordsstring[20],sflagstring[10],counterstring[10],pidstring[50],pattern[100],filename[50],*pipebasename="pipe",nameofpipetocreate[50],str[30],heightstring[10],levelstring[30],startstring[10],endstring[50];
    pid_t pid,wpid;
    size_t n,lSize;
    record_t rec;
    pipe_record_t piperec;
    info_t info;
    clock_t start_t, end_t;
    float total_t;
    FILE *fpa,*fpw;
    start_t=clock();            //start clock;
    //Check args
    sprintf(pidstring,"%d",getpid());
    for(i=1; i<argc; i++){
        if(strcmp(argv[i],"-d")==0){                //data:filename
            if(i+1<argc){
                strcpy(filename,argv[i+1]);
            }
        }
        if(strcmp(argv[i],"-h")==0){                //height
        if(i+1<argc){
            height=atoi(argv[i+1]);

        }
        
        }
        if(strcmp(argv[i],"-s")==0){                //-s flag 
            sflag=1;
        }
        if(strcmp(argv[i],"-p")==0){            //string to search for
        if(i+1<argc){
           strcpy(pattern,argv[i+1]);

        }
        }
    }
    for ( i=1; i<=height; i++ ){            //calculate number of leafs
        numberofchildren=numberofchildren*2;
    }
    for( i=0; i<height; i++){
        numferofsplitNmerge+=pow(2,i);          //calculate number of splitnmergers
    }
    
    //open file
    fpa = fopen (filename,"rb");
   if (fpa== NULL) {
      	printf("Cannot access binary file %s\n",filename);
      	return 1;
   	}
    fpw = fopen ("results.cvs","w");
    if (fpw== NULL) {
      	printf("Cannot create results.cvs \n");
      	return 1;
   	}
    //set up signal
    signal(SIGUSR1,my_handler);         //set up handler 
    //find the size of file 
    fseek(fpa, 0, SEEK_END);
    lSize = ftell(fpa);
  
    numberofrecords=lSize/sizeof(record_t);
    sprintf(numberofrecordsstring,"%d",numberofrecords);

    fclose(fpa);
    if(sflag==1){
        sprintf(sflagstring,"%d",1);
        sprintf(startstring,"%d",1);
        sprintf(endstring,"%d",numberofchildren);    //if sflag is on use 1-numberofchildren range of search
        
    }else{
        sprintf(sflagstring,"%d",0);
        sprintf(startstring,"%d",0);
        sprintf(endstring,"%ld",lSize/sizeof(record_t));    //find the range to search for
    }
    //create the name of the pipe
    strcpy(nameofpipetocreate,pipebasename);            
    sprintf(str, "%d",pipescounter);
    strcat(nameofpipetocreate,str);                            
    if(mkfifo(nameofpipetocreate , 0666)==-1){      //create the pipe,if error exit
        if(errno==EEXIST){
            perror("cant create pipe");
            exit(2);
        }
    }
   sprintf(counterstring,"%d",numberofchildren); //set up counter to count children
    
    //fork the splitNmerge node
    if ((pid = fork()) < 0){
                perror("fork failed\n");
    }
    else if(pid==0){
                sprintf(levelstring,"%d",lvl);
                sprintf(str, "%d",pipescounter+1);
                sprintf(heightstring,"%d",height);
                char *args[]={"./splitNmerge",filename,str,heightstring,nameofpipetocreate,levelstring,startstring,endstring,
                pattern,pidstring,heightstring,sflagstring,numberofrecordsstring,NULL}; //pass the args needed and execute splitNMerge
                execvp(args[0],args);

    }
    if( (fd=open(nameofpipetocreate,O_RDONLY)) < 0) {   //open the pipe to read data
                perror("pipe  open  error\n");
                exit(1);
    }



    while(1){
        n=read(fd,&piperec,sizeof(pipe_record_t));                    //read records
        if(errno==4){                                   //if signal caused failure to sys call  it means no data read
            errno=0;
         }else{
             if(n==0)                               //if pipe is empty
                break;
             if(piperec.flaglast==1){           //if we have info node 
                 while(1){
                     n=read(fd,&info,sizeof(info_t)); 
                     if(errno==4){
                         printf("Hello Ernno");
                         errno=0;
                     }else{
                         printf("Min search %f ,Max search %f,Total search %f,Average search time %f\n",info.minsearcher,info.maxsearcher,info.searcherstime,
                         info.searcherstime/numberofchildren);
                         printf("Min splitNmerge %f , Max splitNmerge %f Total SplitNmerge %f,Average SplitNmerge %f\n",info.minsplitmerge,info.maxsplitmerge,
                         info.splitNmergetime,info.splitNmergetime/numferofsplitNmerge);
                         end_t = clock();
                         total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
                         printf("Turnaround time %f\n",total_t+info.splitNmergetime);
                         printf("SIGUSR1 recieved %d\n",SIGUSR1counter);
                         break;
                     }
                 }
             }else if(piperec.flaglast==0){
                // /printf("Size of record :%ld ,Read %ld \n",sizeof(pipe_record_t),n);
                fprintf(fpw,"%ld %s %s  %s %d %s %s %-9.2f \n", piperec.custid, piperec.LastName, piperec.FirstName, piperec.Street, piperec.HouseID, piperec.City, 
                        piperec.postcode, piperec.amount);
             }
             counter++;

         }   

    }
    


    while ((wpid = wait(&status)) > 0);         //wait for children
    fclose(fpw);            //close the resutls file
    system("sort -k 1 results.cvs>sorted.cvs");
    close(fd);                              //close fd
    unlink(nameofpipetocreate);             //destroy named pipe
    return 0;

}
