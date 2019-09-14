  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
  #include "TRIE.h"
  #include <math.h>
  

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
    size_t bufsize = 10000;
    int i=0;
    buffer = (char *)malloc(bufsize * sizeof(char));
    if( buffer == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }

    for(i=0;i<arraysize;i++){
      getline(&buffer,&bufsize,fptr);
      printf("%s \n",buffer );
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
      
		  
  

int words( char *text)
{
    int count=0,i;
  
    for(i=0; i<strlen(text); i++)
    {
        if((text[i]!=' ' && text[i] !='\t') && i!=(strlen(text)-1))
        {
            count++;
            
          while(text[i]!=' ' &&  text[i]!='\t'){
            
            
            i++;
          }
        }
    }    
    return count-1;
}

double averagecount(char **array,int arraysize){
  double sum=0;
  double average=0;
  for(int i=0;i<arraysize;i++){
    sum=sum + (double)words(array[i]);

  }
  average=sum/arraysize;

  return average;
}

double idf(double n,double qi){
  double result;
  result=log(((n -qi)+0.5)/(qi+0.5));
  return result;

}


double scorefunc(double n,double qi,const char *string,trieNode_t *root,int id,double k,char *text,char **array,double b){
  double result,idfresult,wordsresult,average;
  int fqiresult;
  fqiresult=(string,root,id);
  idfresult=idf(n,qi);
  wordsresult=words(text);
  average=(array,n);


  result=(double)fqiresult*(k+1)/((double)fqiresult + k*(1-b +b*(double)wordsresult));
  return result;

}


