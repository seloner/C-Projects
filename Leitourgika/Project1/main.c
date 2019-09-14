#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "graph.h"
#include "trie.h"
#include "helpfuncts.h"

int main(int argc, char const *argv[])
{   
    FILE *fp;
    int flagexit=0,counter=0,value=0,newvalue=0,outflag=0,inflag=0;
    char buffer[100],buffer2[100],inpath[100],outpath[100],*token;
    list *thelist;
    graph *graphfound,*destination,*source,*pointer;
    edge *edgetoadd,*edgetodelete,*edgetomodify;
    trienode *root,*trienodefound,*trienodefound2;
    for(int i=1; i<argc;i++){               //check args
        if(strcmp(argv[i],"-i")==0){                //if input file
            if(i+1<argc)
                strcpy(inpath,argv[i+1]);
                inflag=1;
        }
        if(strcmp(argv[i],"-o")==0){        //if output
            if(i+1<argc)
            strcpy(outpath,argv[i+1]);
            outflag=1;
        }
    }
    
    thelist=initgraphlist();                //init list for graph
    trieinit(&root);
    if(inflag)                        //init trie
        feed(thelist,root,inpath);              //feed graph with data
    while(flagexit==0){
        scanf("%s",buffer);
        scanf("%[^\n]s",buffer2);
        if(strcmp(buffer,"e")==0){
            
            fprintf(stdout,"exit program\n");
            flagexit=1;
        }else if(strcmp(buffer,"i")==0){            //insert command
             if(strcmp(buffer2,"")==0){
                printf("Give the name of the graph node\n");
             }else{
                 token=strtok(buffer2," \n");
                 trienodefound=triesearch(root->children,token);
                 if(trienodefound==NULL || trienodefound->pointer==NULL){
                      trieadd(&root,token,graphaddtolist(thelist,token));
                      printf("Inserted |%s|\n",token);
                 }else{
                    printf("Node |%s| Exists; \n",token);
                }
            }
             
        }
        else if(strcmp(buffer,"d")==0){         //delete command
            if(strcmp(buffer2,"")==0){
                printf("Give the name of the graph node\n");
             }else{
                 token=strtok(buffer2," \n");
                 trienodefound=triesearch(root->children,token);
                 if(trienodefound==NULL){
                     printf("Node |%s| does not exist - abort-d\n",token);
                 }else{
                     if(trienodefound->pointer!=NULL){
                        deletealledgesfromnode(trienodefound->pointer);
                        deletenode(&thelist,token);
                        triespecial(&root,token);
                        printf("Deleted |%s|\n",token);
                     }   
                 }
             }


        }
        else if(strcmp(buffer,"n")==0){            //insert with weight
            
            if(strcmp(buffer2,"")==0){
                printf("Give information about nodes and weight\n");
             }else{
                 token=strtok(buffer2," ");
                 while(token){              
                     if(counter==0){;                           //first name of node
                        trienodefound=triesearch(root->children,token);
                        if(trienodefound==NULL  || trienodefound->pointer==NULL){
                            source=graphaddtolist(thelist,token);
                            trieadd(&root,token,source);
                        }else{
                            source=trienodefound->pointer;
                        }
                     }
                     if(counter==1){                                   //second name of node
                         trienodefound=triesearch(root->children,token);
                          if(trienodefound==NULL  || trienodefound->pointer==NULL ){
                            destination=graphaddtolist(thelist,token);
                            trieadd(&root,token,destination);

                            }else{
                            destination=trienodefound->pointer;
                            
                         }
                     } 
                     if(counter==2){                        //value of edge
                         value=atoi(token);
                     }
                      counter++;  
                      token = strtok(NULL, "  \n ");
                 }
                 edgetoadd=createdge(source,destination,value);
                 if(edgetoadd){
                     addedgetographnode(source,destination,edgetoadd);
                     printf("Inserted |%s|->%d->|%s|\n",source->string,edgetoadd->value,destination->string);
                 }
                 
             }
             counter=0;
        }
        else if(strcmp(buffer,"l")==0){               //delete with weight
            
            if(strcmp(buffer2,"")==0){
                printf("Give information about nodes and weight\n");
             }else{
                 token=strtok(buffer2," ");
                 while(token){              
                     if(counter==0){;
                        trienodefound=triesearch(root->children,token);         //name of first node
                        if(!trienodefound   || trienodefound->pointer==NULL){
                            printf("|%s| does not exist - abort-l\n",token);
                        }
                     }
                     if(counter==1){
                         trienodefound2=triesearch(root->children,token);
                         if(!trienodefound2 || trienodefound2->pointer==NULL){
                            printf("|%s| does not exist - abort-l\n",token);      //name of second node
                        }
                        
                     } 
                     if(counter==2){                                        //value of edge
                         value=atoi(token);
                     }
                      counter++;  
                      token = strtok(NULL, "  \n ");
                 }
                    if(counter==3 && trienodefound->pointer && trienodefound2->pointer){
                       edgetodelete=searchedgebyvalue(trienodefound->pointer,trienodefound2->pointer,value);
                       if(edgetodelete){
                           deletededge (edgetodelete);
                           printf("Del-vertex |%s|->%d->|%s|\n",trienodefound->pointer->string,value,trienodefound2->pointer->string);
   
                       }else{
                           printf("|%s|->%d->|%s| does not exist - abort-l;\n",trienodefound->pointer->string,value,trienodefound2->pointer->string);
                       }
                    }
                 
             }
             counter=0;
        }
         else if(strcmp(buffer,"m")==0){        //modify edge
            
            if(strcmp(buffer2,"")==0){
                printf("Give information about nodes and weight\n");
             }else{
                 token=strtok(buffer2," ");
                 while(token){              
                     if(counter==0){;
                        trienodefound=triesearch(root->children,token);
                         if(!trienodefound || trienodefound->pointer==NULL){
                            printf("|%s| does not exist - abort-m\n",token);
                        }
                     }
                     if(counter==1){
                         trienodefound2=triesearch(root->children,token);
                          if(!trienodefound2    || trienodefound->pointer==NULL){
                            printf("|%s| does not exist - abort-m\n",token);
                        }
                        
                     } 
                     if(counter==2){
                         value=atoi(token);
                     }
                     if(counter==3){
                         newvalue=atoi(token);
                     }
                      counter++;  
                      token = strtok(NULL, "  \n ");
                 }
                    if(counter==4 && trienodefound->pointer && trienodefound2->pointer){
                       edgetomodify=searchedgebyvalue(trienodefound->pointer,trienodefound2->pointer,value);
                       if(edgetomodify){
                        modify(edgetomodify,newvalue);
                        printf("Mod-vertex |%s|->%d->|%s|\n",trienodefound->pointer->string,newvalue,trienodefound2->pointer->string);
                       }else{
                           printf("|%s|->%d->|%s| does not exist - abort-m\n",trienodefound->pointer->string,value,trienodefound2->pointer->string);
                       }
                    }
                 
             }
             counter=0;
        }else if(strcmp(buffer,"r")==0){            //print all incoming edges
            token=strtok(buffer2," \n");
            if(trienodefound=triesearch(root->children,token)){
                if(trienodefound->pointer)
                    recievingprint(trienodefound->pointer);
                else   
                    printf("|%s| does not exist - abort-r\n",token);   
            }else{
                printf("|%s| does not exist - abort-r\n",token);
            }

        } else if(strcmp(buffer,"c")==0){           //circlefind
            token=strtok(buffer2," \n");
            if(trienodefound=triesearch(root->children,token)){
                if(trienodefound->pointer)
                    circlefind(trienodefound->pointer);
                 else   
                    printf("|%s| does not exist - abort-c\n",token);   
            }else{
                printf("|%s| does not exist - abort-c\n",token);
            }

        }  
        else if(strcmp(buffer,"f")==0){         //findcircles
            token=strtok(buffer2," ");
            while(token){
                if(counter==0){
                    trienodefound=triesearch(root->children,token);
                    if(!trienodefound || trienodefound->pointer==NULL)
                        printf("|%s| does not exist - abort-f\n",token);
                    
                }
                if(counter==1){
                   if(trienodefound && trienodefound->pointer){
                       findcircles(trienodefound->pointer,atoi(token));
                   }
                }    
                token=strtok(NULL,"\n");
                counter++;
            }
            counter=0;
        } 
         else if(strcmp(buffer,"t")==0){         //findcircles
            token=strtok(buffer2," ");
            while(token){
                if(counter==0){
                    trienodefound=triesearch(root->children,token);
                    if(!trienodefound || trienodefound->pointer==NULL)
                        printf("|%s| does not exist - abort-f\n",token);
                    
                }
                if(counter==1){
                    trienodefound2=triesearch(root->children,token);
                    if(!trienodefound2 || trienodefound2->pointer==NULL)
                        printf("|%s| does not exist - abort-f\n",token);
                }
                if(counter==2){
                   if(trienodefound && trienodefound2  && trienodefound->pointer && trienodefound2->pointer){
                       traceflow(trienodefound->pointer,trienodefound2->pointer,atoi(token));
                   }
                }    
                token=strtok(NULL,"\n");
                counter++;
            }
            counter=0;
        }
        strcpy(buffer,"");
        strcpy(buffer2,"");
    }

    if(outflag){
        graphprintf(thelist,outpath);   //if output file is requested
    }    
    destroy(thelist);               //free data
    triedestroy(root);
  
}
