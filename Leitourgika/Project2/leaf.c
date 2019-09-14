#include <stdio.h>
#include "leaf.h"
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

int main(int argc, char const *argv[])
{
    int flagfound=0,i,fd,rootid,am,end=0,recordsize=0,locationOfSoughtRec=0;
    char pattern[50],filename[50],pipename[50];
    float salary;
    FILE *fpa;
    record_t rec;
    info_t info;
    pipe_record_t piperec;
    size_t tn=0,n=0;
    clock_t start_t, end_t;
    float total_t;
    start_t = clock();
    //init args
    strcpy(filename,argv[1]);       //data:filename
     fpa = fopen (filename,"rb");
     if (fpa== NULL) {
      	printf("Cannot access binary file %s\n",filename);
      	return 1;
   	} 
   
    locationOfSoughtRec=atoi(argv[2]);      //starting point for search
    end=atoi(argv[3]);                      //end
    end--;                                  //-1 because we are starting from 0 record
    
    strcpy(pipename,argv[5]);           //pipename to read data
    strcpy(pattern,argv[6]);            //string to search for
    rootid=atoi(argv[7]);               //root id
   
    recordsize=(sizeof(record_t));        //record size
    int recscounter=0;

    if( (fd=open(pipename,O_WRONLY)) < 0){          //open pipe for writing
         perror("pipe  open  error\n");
    }
        fseek(fpa,(sizeof(record_t))*(locationOfSoughtRec),SEEK_SET);     //fseek to the requested section
        for(i=locationOfSoughtRec; i<=end; i++){                //from start point until then end point    
            fread(&rec, sizeof(record_t), 1, fpa);              //read record from binary
            //printf("%ld %s Firstname %s  %s %d %s %s %-9.2f\n", rec.custid, rec.LastName, rec.FirstName, rec.Street, rec.HouseID, rec.City, rec.postcode, rec.amount);
            if(searchforsubstring(rec,pattern)==1){
                recscounter++;
                flagfound=1;
                copyrecord(&rec,&piperec,0);
                while(tn<sizeof(pipe_record_t)){                    //write the record to the pipe if found  
                    n=write(fd,&piperec,sizeof(pipe_record_t));           
                    tn+=n;
         
                }
            
                tn=0;
            }
            if(end==i){
                end_t = clock();                                    //calculate time 
                total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
               
                if(flagfound){
                    copyrecord(&rec,&piperec,1);         //flag a record and use it as last(dont care about the data in it)
                    while(tn<sizeof(pipe_record_t)){                      
                        n=write(fd,&piperec,sizeof(pipe_record_t));           
                        tn+=n;
            
                    }
                    tn=0;
                    
                     
                     info.time=total_t;
                     info.splitNmergetime=0;
                     while(tn<sizeof(info_t)){                   
                        n=write(fd,&info,sizeof(info_t));           
                        tn+=n;
                     }
                }

                                                                                       
            } 
        }
            
        
        tn=0;
     
        fclose(fpa);

    if((kill(rootid,SIGUSR1))<0){            //send signal to root
        perror("Signal sending error\n");
    }
    close(fd); 
    return 0;
}