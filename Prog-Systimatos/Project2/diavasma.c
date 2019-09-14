  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
  #include<sys/types.h>
  #include<unistd.h>
  #include<fcntl.h>
  #include<sys/stat.h>
  #include<sys/wait.h> 
  #include<errno.h>


  typedef struct pidarray{
 
    pid_t *array;
    
   }pidarray;  

  int diavasma(FILE*fptr){
    int c,arraysize=0;
    

    while ( (c = fgetc (fptr)) != EOF )
    if ( c == '\n' )
      arraysize++;
   
    if ( ferror (fptr) )
    printf ("Read Error\n"), exit (EXIT_FAILURE);

    return arraysize;
  }

  void eisagwgh(FILE*fptr,char **array,int arraysize){
    char *buffer;
    size_t bufsize = 4096;
    int i=0;
    buffer = (char *)malloc(bufsize * sizeof(char));
    if( buffer == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }

    for(i=0;i<arraysize;i++){
      getline(&buffer,&bufsize,fptr);
      int len = strlen(buffer);
      if (len > 0 && buffer[len-1] == '\n')
            buffer[len-1]=0;


        array[i]=(char *)malloc(bufsize*sizeof(char));
      

        if( array[i] == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }
      strcpy(array[i],buffer);

   }
   free(buffer);    

 }


 char* addstrings(char *str1,char *str2){
    char *str;
    str=malloc(strlen(str1)+strlen(str2)+3);
    strcpy(str,str1);
    strcat(str,"\n");
    strcat(str,str2);
    
    return str;
    


 }


 void pathallocation(char **array,int arraysize,int number){
    int counter=0,dif;  
    int firstindex=0,secondindex=arraysize-1;

if(arraysize>number){

    dif=arraysize-number;
    while(counter<dif){
        array[firstindex]=addstrings(array[firstindex],array[secondindex]);
        //free(array[secondindex]);
        firstindex++;
        secondindex--;
        if(firstindex==number-1){

          firstindex=0;
        }






    }

  } 




 }

  

void forker(int nprocesses,int counter,pidarray *ptrarray)
{
    pid_t pid;
    char str[100],*myfifo="jobtoworker",*myfifo2="workertojob";
    pidarray temptr;
     
    if(nprocesses > 0)
    {

      int i=0;
        if ((pid = fork()) < 0)
        {
            perror("fork");
        }
        else if (pid == 0)
        {

            
            
            //printf("%d \n", *((*array)+counter) );
          sprintf(str, "%d",counter+1);
            char *args[]={"./worker",str,NULL};
            execvp(args[0],args);
           
        }
        else if(pid > 0)
        {
          ptrarray->array[counter]=pid;
          counter++;
      sprintf(str, "%d",counter);
      char * str3 = (char *) malloc(1 + strlen(str)+ strlen(myfifo));
      strcpy(str3,myfifo);
      strcat(str3,str);

            char * str4 = (char *) malloc(1 + strlen(str)+ strlen(myfifo2));
            strcpy(str4,myfifo2);
            strcat(str4,str);

            if(mkfifo(str3, 0666)==-1){
              if(errno!=EEXIST){

                perror("cant create pipe");
                exit(6);
              }

            }
            if(mkfifo(str4, 0666)==-1){
                if(errno!=EEXIST){

                    perror("cant create pipe");
                    exit(6);
                }

            }




            //printf("created child with pid %d and id %d \n",pid,counter);
            forker(nprocesses - 1,counter,ptrarray);
            
        }
      
    }
    
}

void printtext(char *filename){
    FILE *fptr;
    char c;
    fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        printf("Cannot open file \n");
        exit(0);
    }
 
    // Read contents from file
    c = fgetc(fptr);
    while (c != EOF)
    {
        printf ("%c", c);
        c = fgetc(fptr);
    }
 
    fclose(fptr);




}