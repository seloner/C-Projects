#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TRIE.h"
#include "DIAVASMA.h"

int main(int argc, char *argv[]) {
	FILE *fp,*temp;
	int k,firstwordflag=1,seachflag=0,exitflag=0,dflag=0,argumentsread=0,buffer2flag=0,arraysize=0;
	char **array,buffer[100],buffer2[100],**constarray;
	double average=0;
  trieNode_t *root;

	if(argc<5){
		printf("Error not enough arguments,\n");
		return -1;
	
	}
		/*elegxos orismatwn */
     if(strcmp(argv[1],"-i")!=0){
        printf("Wrong argument\n");
        return -1;
	}

	if( strcmp(argv[3],"-k")!=0){
		printf("Wrong argument\n");
		return -1;
	}		
			
	k=atoi(argv[4]);	
    if ((fp= fopen(argv[2], "r")) == NULL)
      {
          printf("Error0! opening file");
          // Program exits if file pointer returns NULL.
          return(-1);         
      }

      arraysize=diavasma(fp);

      fclose (fp);
      array=(char**)malloc(arraysize*(sizeof(char*)));
      
      constarray=(char**)malloc(arraysize*(sizeof(char*)));

      if ((fp= fopen(argv[2], "r")) == NULL)
      {
          printf("Error1! opening file");
          // Program exits if file pointer returns NULL.
          return(-1);         
      }

      eisagwgh(fp,array,arraysize); 
         
      fclose(fp);
      if ((fp= fopen(argv[2], "r")) == NULL)
      {
          printf("Error2! opening file");
          // Program exits if file pointer returns NULL.
          return(-1);         
      }
      eisagwgh(fp,constarray,arraysize); //vale sto map ta keimena
      fclose(fp);
      TrieCreate(&root);
       
       
      for(int i=0;i<arraysize;i++){
            
           char *token = strtok(array[i], " \t");        //pairne leksi leksi to perioxomeno toy map
           while (token) {
           if(firstwordflag!=1) {
           	                char *text;
                             
                
              text=(char*)malloc((strlen(token)+1)*sizeof(char));
              
              strcpy(text,token);
              
              TrieAdd(&root,token,i,text);

              }
                            

              
              token = strtok(NULL, " \n \t");
              firstwordflag=0;
              
            }
            firstwordflag=1;

      }


        
              scanf("%s",buffer);
              scanf("%[^\n]s",buffer2);
              trieNode_t *found;
                while(strcmp(buffer,"/exit")!=0){

                    if(strcmp(buffer,"/df")==0){
                        char *token = strtok(buffer2,"  ");
                        while(token ){
                          dflag=1;

                          found=TrieSearch(root->children,token);
                          token = strtok(NULL, " ");
                          if(found){
                            printf("%s %d \n",found->value->text,plithoskeimenon(found->value));


                          }

                        }
                        if(dflag==0){
                          
                          printall(root); //dietrekse olo to dentro
                          
                        }

                        dflag=0;

                    }
                    
                    if(strcmp(buffer,"/tf")==0){

                      int currentid=-1,counter=0;
                       char *token = strtok(buffer2,"  ");
                        while(token ){
                            if(counter==0){
                            currentid=atoi(token);
                              counter++;
                              

                            }
                            else{
                              
                              found=TrieSearch(root->children,token);   

                              if(found){

                                  
                          
                                  tf(found->value,currentid);

                                }

                              }      
                            
                          token = strtok(NULL, " ");

                      }
                    }
                     if(strcmp(buffer,"/search")==0){
                      trieVal_t current;
                      int counter=0,id;
                      double qi,k=1.2,b=0.75,scoreresult=0;
                       char *token = strtok(buffer2,"  ");
                        while(token ){
                            if(counter<10){       
                              
                              found=TrieSearch(root->children,token);
                              if(found){
                                current=found->value;
                                while(current){
                                  qi=(double)plithoskeimenon(current);
                                  id=current->id;
                                  scoreresult=scoreresult+scorefunc(arraysize,qi,token,root,id,k,constarray[id],constarray,b);

                                  current=current->next;
                                }
                                    int i=0;
                                    printf("%d.(%d)[%f] %s \n ",i,(found->value->id),scoreresult,constarray[found->value->id]);


                                }

                              }   
                                  token = strtok(NULL, " ");
                                 counter++;
                            }
                          

                      }
                    
                    strcpy(buffer2,"");
                  scanf("%s",buffer);
                  scanf("%[^\n]s",buffer2);

                    
                   

                }

            



              
          

       
     
      listdestroy(root);    //apodesmeusi
      TrieDestroy(root);

      free (array);
      free(constarray);

    return 0;
}
