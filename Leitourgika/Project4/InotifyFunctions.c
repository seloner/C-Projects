//InotifyFunctions.c
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include "CreateTree.h"
#include "Structures.h"
#include "InotifyFunctions.h"

volatile int endflag = 0;
extern volatile int fd;

void my_handler(int s){
          endflag=1;

}
//queue functions//
dirList_t *dirNodeListInit(void){
    dirList_t *nodeListTemp;
    nodeListTemp = malloc(sizeof(dirList_t));

    nodeListTemp->numOfNodes = 0;
    nodeListTemp->head = NULL;
    nodeListTemp->last = NULL;
    return nodeListTemp;
}


dirListNode_t *dirNodeInit(char *path, treeNode_t *treeNode, treeNodeList_t *fatherList){

    dirListNode_t *tempNode;
    tempNode = malloc(sizeof(dirListNode_t));

    tempNode->path = malloc(strlen(path)*sizeof(char)+1);
    strcpy(tempNode->path,path);
    tempNode->wd = -1;
    tempNode->treeNode = treeNode;
    tempNode->fatherList = fatherList;
    tempNode->next = NULL;

    return tempNode;
}


void dirNodeInsert(dirList_t *nodelist, char *path, treeNode_t *treeNode, treeNodeList_t *fatherList ){
    dirListNode_t *temp;
    temp = dirNodeInit(path, treeNode, fatherList);
    if(nodelist->numOfNodes == 0){
        nodelist->head = temp;
        nodelist->last = temp;
    }
    else{
        (nodelist->last)->next = temp;
        nodelist->last = temp;
    }

    // dirListNode_t *current=NULL;
    // temp = dirNodeInit(path, treeNode, fatherList);
    // if(nodelist->head==NULL){  //first node to insert 
    //     nodelist->head=temp;
        
    // }else{
    //     current=nodelist->head;
    //     while(current->next!=NULL){
    //         current=current->next;
    //     }
    //     current->next=temp;
    // }
    nodelist->numOfNodes = nodelist->numOfNodes + 1;
    
   
}


dirListNode_t * dirNodeFind(dirList_t *nodelist, int wd){
	dirListNode_t *temp;
	temp = nodelist->head;
	//printf("psaxnw gia: %s\n", id);
	while(temp != NULL){

		if(temp->wd == wd){
			return temp;
		}
		temp = temp->next;
	}

	return NULL;
}


dirListNode_t * dirNodeFindByPath(dirList_t *nodelist, char *path){
	dirListNode_t *temp;
	temp = nodelist->head;
	//printf("psaxnw gia: %s\n", id);
	while(temp != NULL){

		if(strcmp(temp->path,path) == 0){
			return temp;
		}
		temp = temp->next;
	}

	return NULL;
}


void deletedirNodeList(dirList_t *nodelist){
	dirListNode_t *temp;
	temp = nodelist->head;

	while(temp != NULL){
		nodelist->head = temp->next;
		free(temp->path);
		free(temp);
		temp = nodelist->head;
	}
}

void deletedirNode(dirList_t *nodelist, dirListNode_t * node){
	dirListNode_t *current, *previous;
	current = nodelist->head;
	previous=current;

	while(current){
		if(current == node){
			previous->next = current->next;
			if(current==nodelist->head){
                nodelist->head=current->next;
            }
            if(current->next == NULL){
            	nodelist->last = previous;
        	}
            free(current->path);
            free(current);
            nodelist->numOfNodes = nodelist->numOfNodes - 1;
            return;
		}
		previous=current;
        current=current->next;
	}
	
}

void fillDirList(treeNode_t *root,int level, dirList_t *nodelist){
  	treeNode_t *current;
  	current = root;
	int filelevel=level+1;
	if(root==NULL){
		return;
	}

  	if(level == 0){
  		// printf("!!%s, %s\n",root->fullpath, current->Sname );
  		dirNodeInsert(nodelist,root->fullpath,current, root->list);
  	}

  	current=root->list->head;
  	if(current==NULL){
    	return;
  	}
  	while(current!=NULL){

    	if(current->type=='d'){
    		// printf("!!%s, %s\n",current->fullpath, current->Sname );
    		dirNodeInsert(nodelist,current->fullpath,current, root->list);
    	}
    	current=current->next;  
  	}

  	current=root->list->head;
  	while(current!=NULL){
  		if(current->type=='d'){
      		fillDirList(current,level+1, nodelist); 
    	} 
    	current=current->next;
  	}
}

void removeWatch(int fd, dirList_t *nodelist){
	int rm;
	dirListNode_t *temp;
	temp = nodelist->head;

	while(temp != NULL){
		rm = inotify_rm_watch(fd,temp->wd);
		if(rm == -1){
			perror("failed rm_watch");
		}
		else{
			printf("wd: %d directory %s removed from watch\n",temp->wd, temp->path );
		}
		temp = temp->next;
	}
}

// treeNode_t* findTreenodeOnSameLevel(treeNode_t *root,char *path){
//     treeNode_t *result=NULL,*current=NULL;
//     current=root;
//     if(current==NULL)           //if root is null
//         return NULL;
//     if(strcmp(current->fullpath,path)==0){  //check if it is root
//                 return current;
//     }
//     result=findTreenodeOnSameLevel(current->next,Sname); //search for the sibling and save result
//     return result;

// } 


void fun(dirList_t *nodelist, dirList_t *nodelistB, treeNode_t* root,treeNode_t *backupRoot,inodeList_t *backUpinodeList,inodeList_t *sourceinodeList){
	struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

	printf("endflag %d\n",endflag );
	fillDirList(root,0, nodelist);
	fillDirList(backupRoot,0, nodelistB);

	dirListNode_t *nodeS;
	dirListNode_t *nodeB;
	nodeS = nodelist->head;
	nodeB = nodelistB->head;
	
	if(nodeS == NULL){
		perror("list empty");
		printf("NULL!!!!!1 fix me\n");
	}
	if(nodeB == NULL){
		perror("list empty");
		printf("NULL!!!!!1 fix me\n");
	}
	


	int length, read_ptr, read_offset; //management of variable length events
	int i, watched;
	int wd;					// descriptors returned from inotify subsystem
	char buffer[EVENT_BUF_LEN];	//the buffer to use for reading the events


	
	/*creating the INOTIFY instance*/
	fd = inotify_init();
	if (fd < 0)
		fail("inotify_init");
	printf("fd fun %d\n",fd );
	watched = 0;
	
	while( nodeS != NULL ) {
		/*adding a watch for element argv[i]*/

		wd = inotify_add_watch(fd, nodeS->path,IN_ATTRIB | IN_MODIFY | IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO | IN_DELETE_SELF);
		if( wd == -1 ) {
			perror("failed to add watch");
			printf("failed to add watch %s\n", nodeS->path);
		} else {
			nodeS->wd = wd;
			printf("Watching %s as %i\n",nodeS->path , nodeS->wd);
			watched++;
		}
		nodeS = nodeS->next;
	}
		
	
	while( nodeB != NULL ) {
		/*adding a watch for element argv[i]*/
		wd = inotify_add_watch(fd, nodeB->path,IN_ATTRIB | IN_MODIFY | IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO | IN_DELETE_SELF);
		if( wd == -1 ) {
			perror("failed to add watch");
			printf("failed to add watch %s\n", nodeB->path);
		} else {
			nodeB->wd = wd;
			printf("Watching %s as %i\n",nodeB->path , nodeB->wd);
			watched++;
		}
		nodeB = nodeB->next;
		
	}
	
	if( watched == 0 )
		fail("Nothing to watch!");

	read_offset = 0; //remaining number of bytes from previous read
	int k = 0;
	while (1) {
		k++;
		

		/* read next series of events */
		length = read(fd, buffer + read_offset, sizeof(buffer) - read_offset);
		// printf("%s\n", buffer);

		if(endflag == 1){
			break;
		}

		if (length < 0)
			fail("read");
		length += read_offset; // if there was an offset, add it to the number of bytes to process
		read_ptr = 0;
		
	
		while (read_ptr + EVENT_SIZE <= length ) { 
			//point event to beginning of fixed part of next inotify_event structure
			struct inotify_event *event = (struct inotify_event *) &buffer[ read_ptr ];
			
		
			if( read_ptr + EVENT_SIZE + event->len > length ) 
				break;
			//event is fully received, process
			event_name(fd, event, nodelist, nodelistB, backUpinodeList, sourceinodeList);
			// printf("%d\n",k );
			read_ptr += EVENT_SIZE + event->len;
		}
		//check to see if a partial event remains at the end
		if( read_ptr < length ) {
			//copy the remaining bytes from the end of the buffer to the beginning of it
			memcpy(buffer, buffer + read_ptr, length - read_ptr);
			//and signal the next read to begin immediatelly after them			
			read_offset = length - read_ptr;
		} else
			read_offset = 0;
		
	}
	// typically, for each wd, need to: inotify_rm_watch(fd, wd);

	// close(fd);
}



void fail(const char *message) {
	perror(message);
	exit(1);
}
const char * target_type(struct inotify_event *event) {
	if( event->len == 0 )
		return "";
	else
		return event->mask & IN_ISDIR ? "directory" : "file";
}

const char * target_name(struct inotify_event *event) {
	return event->len > 0 ? event->name : NULL;
}



char *pathCreator(char *name, char *pathToFather){
	char *fullPath;
	fullPath = malloc(strlen(name)*sizeof(char)+strlen(pathToFather)*sizeof(char)+2);
	strcpy(fullPath,pathToFather);
	strcat(fullPath,"/");
	strcat(fullPath,name);

	return fullPath;
}


// char *createBackupPath(char *){

// }


void event_name(int fd, struct inotify_event *event, dirList_t *nodelist, dirList_t *nodelistB,inodeList_t *backUpinodeList,inodeList_t *sourceinodeList){
	if ( event->len ) {
		if(event->name[0] == '.'){
			return;
		}

		char *fOrDPathS, *fOrDPathB;
		stringNode_t *stringNode;
		treeNode_t *treeNode;
		inode_t *inode;
		struct stat buf;
		int wd;

		treeNode_t *result = NULL;

		int isSourceOrBackup = 0; //is source by default
		dirListNode_t * nodeS, *nodeB = NULL;
		nodeS = dirNodeFind(nodelist, event->wd); //vriskei ton patera ston opoio egine h allagh

		if(nodeS == NULL){
			return;
		}
		nodeB =  dirNodeFindByPath(nodelistB, nodeS->treeNode->inodeP->inodePBackUp->stringsList->head->string);
		// }
		if(nodeB == NULL){
			return;
		}
		// printf("nodeB path %s\n", nodeS->treeNode->inodeP->inodePBackUp->stringsList->head->string);

		fOrDPathS = pathCreator(event->name,nodeS->path); 

		if ( event->mask & IN_CREATE ) {

			stat(fOrDPathS,&buf);
        	inode=inodeCreate(buf.st_ino,buf.st_size,buf.st_mtime,1); 
			if(inode==NULL){
	          	printf("Inode create error\n");
	          	return;/////////////////////////
	        }

	        stringNode=createStringNode(fOrDPathS);      //create the stringnode
	        if(stringNode==NULL)
	          	return ;//////////////////
	        pushToStringsList(inode->stringsList,stringNode);   //push to string list     
			
	        // printf("!!!!!!%s\n",nodeS->path );
	    	if ( event->mask & IN_ISDIR ) {

	    		treeNode=treeNodeCreate(event->name,stringNode->string,'d',inode);   //create the treenode
	      		printf( "wd: %d New directory %s created.\n",event->wd ,fOrDPathS );

	    	}
	    	else {
	    		treeNode=treeNodeCreate(event->name,stringNode->string,'f',inode);   //create the treenode
	      		printf( "wd: %d New file %s created.\n",event->wd , fOrDPathS );
	    	}

	    	if(treeNode==NULL){
	          	printf("Cant create treeNode\n");
	          	return  ;////////////////////////
	        }
	        if(isSourceOrBackup == 0){
	        	pushToInodeList(sourceinodeList,buf.st_ino,treeNode);   //push inode to the list
	        	pushToTreeNodeList((nodeS->treeNode)->list,treeNode);
	        }
	        
	        if ( event->mask & IN_ISDIR ) {

	        	////////////////////////////
	        	dirListNode_t *nodeTemp;
	        	dirNodeInsert(nodelist,fOrDPathS,treeNode,NULL); //**vazw san fatherlist to null afou den xrhsimopoieitai**//
	      		
	      		nodeTemp = nodelist->last;
	      		wd = inotify_add_watch(fd, nodelist->last->path,IN_ATTRIB | IN_MODIFY | IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO | IN_DELETE_SELF);
				if( wd == -1 ) {
					perror("failed to add watch");
				} else {
					nodelist->last->wd = wd;
					printf("Watching %s as %i\n",nodelist->last->path , nodelist->last->wd);
					
				}

				// printf("source %s\n",nodeS->treeNode->Sname);
				// printf("backup %s\n",nodeB->treeNode->Sname);
				// printf("source path %s\n", nodeS->path);
				// printf("source path %s\n",nodeS->treeNode->inodeP->inodePBackUp->stringsList->head->string);
				
				//////////////////////////////

	        	syncLevel_2a(nodeS->treeNode,nodeB->treeNode, nodeS->path, nodeS->treeNode->inodeP->inodePBackUp->stringsList->head->string,backUpinodeList );

	        	////////////////////////////////////
	        	fOrDPathB = pathCreator(event->name,nodeS->treeNode->inodeP->inodePBackUp->stringsList->head->string);
				// printf("fOrDPathB: %s\n", fOrDPathB);
				treeNode = findTreenodeOnSameLevel(nodeB->treeNode->list->head,event->name);
				dirNodeInsert(nodelistB,fOrDPathB,treeNode,NULL);
				nodeTemp = nodelistB->last;
	      		wd = inotify_add_watch(fd, nodelistB->last->path,IN_ATTRIB | IN_MODIFY | IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO | IN_DELETE_SELF);
				if( wd == -1 ) {
					perror("failed to add watch");
				} else {
					nodelistB->last->wd = wd;
					printf("Watching %s as %i\n",nodelistB->last->path , nodelistB->last->wd);
					
				}
				///////////////////////////////
	        }
	        else{
	        	syncLevel_2c(nodeS->treeNode,nodeB->treeNode, nodeS->path, nodeS->treeNode->inodeP->inodePBackUp->stringsList->head->string,backUpinodeList );
	        }
	        // printf("%d\n",nodeS->treeNode->inodeP->inodePBackUp->inodeId);
	        // printf("%s\n",nodeS->treeNode->inodeP->inodePBackUp->stringsList->head->string );
	         // printf("%s\n",fOrDPathB );
	        
	        // printf("!!!!!!!\n");
	  	}
	  	else if ( event->mask & IN_DELETE ) {


	  		// printf("%s\n",nodeS->treeNode->Sname );
	  		// printf("%s\n",event->name );
	  		result = findTreenodeOnSameLevel(nodeS->treeNode->list->head,event->name);
	  		if(result == NULL){
	  			perror("findTreenodeOnSameLevel,event_name");
	  			return;
	  		}
			int rm = -10;
			dirListNode_t *nodeSD;
	  		fOrDPathB = pathCreator(event->name,nodeS->treeNode->inodeP->inodePBackUp->stringsList->head->string);

	  		if ( event->mask & IN_ISDIR ) {
	  			printf( "wd: %d Directory %s deleted.\n",event->wd , event->name );
	  			//////////////////////////
	      		
	      		nodeSD = dirNodeFindByPath(nodelist, fOrDPathS);
	      		printf("%s %d\n",nodeSD->path, nodeSD->wd);
	   //    		rm = inotify_rm_watch(fd,nodeSD->wd);
	   //    		if(rm == -1){
				// 	perror("failed rm_watch");
				// }
				// else{
				// 	printf("wd: %d directory %s removed from watch\n",nodeSD->wd, nodeSD->path );
				// }
				/////////////////////
			}


	  		deletefromTreeNodeList(result,nodeS->treeNode->list);

	  		if ( event->mask & IN_ISDIR ) {
	  			///////////////////////
	      		dirListNode_t *nodeBD;
	      		
	      		nodeBD = dirNodeFindByPath(nodelistB, fOrDPathB);
	   //    		rm = inotify_rm_watch(fd,nodeBD->wd);
	   //    		if(rm == -1){
				// 	perror("failed rm_watch");
				// }
				// else{
				// 	printf("wd: %d directory %s removed from watch\n",nodeBD->wd, nodeBD->path );
				// }
				deletedirNode(nodelist, nodeSD);
	      		deletedirNode(nodelistB, nodeBD);
	      		/////////////////////////
	  		}

	    	if ( event->mask & IN_ISDIR ) {
	    		syncLevel_2a(nodeS->treeNode,nodeB->treeNode, nodeS->path, nodeS->treeNode->inodeP->inodePBackUp->stringsList->head->string,backUpinodeList );
	      		
	    	}
	    	else {
	    		syncLevel_2d(nodeS->treeNode,nodeB->treeNode, nodeS->path, nodeS->treeNode->inodeP->inodePBackUp->stringsList->head->string,backUpinodeList );
	      		printf( "wd: %d File %s deleted.\n",event->wd , event->name );
	    	}


	  	}
	  	else if ( event->mask & IN_DELETE_SELF ) {
	    	if ( event->mask & IN_ISDIR ) {
	      		printf( "wd: %d Directory %s deleted self.\n",event->wd , event->name );
	    	}
	    	else {
	      		printf( "wd: %d File %s deleted self.\n",event->wd , event->name );
	    	}
	  	}
	  	else if ( event->mask & IN_MODIFY ) { //modify
	    	if ( event->mask & IN_ISDIR ) {
	      		printf( "wd: %d Directory %s modified.\n",event->wd , event->name );

	    	}
	    	else {
	    		stat(fOrDPathS,&buf);
	    		
	    		result = findTreenodeOnSameLevel(nodeS->treeNode->list->head,event->name);
	    		if(result == NULL){
		  			perror("findTreenodeOnSameLevel,event_name");
		  			return;
		  		}
		  		result->inodeP->lastChange = buf.st_mtime;
	      		printf( "wd: %d File %s modified.\n", event->wd ,event->name );
	    	}
	  	}
	  	else if ( event->mask & IN_MOVED_FROM ) {
	    	if ( event->mask & IN_ISDIR ) {
	    		printf( "wd: %d Directory %s moved from %s\n",event->wd , fOrDPathS ,nodeS->path);
	      		// printf( "wd: %d Directory %s moved from %s\n",event->wd , event->name ,nodeS->path);
	    	}
	    	else {
	    		printf( "wd: %d File %s moved from %s.\n",event->wd , fOrDPathS ,nodeS->path);
	      		// printf( "wd: %d File %s moved from %s.\n",event->wd , event->name ,nodeS->path);
	    	}
	  	}
	  	else if ( event->mask & IN_MOVED_TO ) {
	  		if ( event->mask & IN_ISDIR ) {
	    		printf( "wd: %d Directory %s MOVED_TO %s.\n",event->wd , fOrDPathS, nodeS->path );
	      		// printf( "wd: %d Directory %s MOVED_TO %s.\n",event->wd , event->name, nodeS->path );
	    	}
	    	else {
	    		printf( "wd: %d File %s MOVED_TO %s.\n",event->wd , fOrDPathS , nodeS->path);
	      		// printf( "wd: %d File %s MOVED_TO %s.\n",event->wd , event->name , nodeS->path);
	    	}
	  	}
	  	free(fOrDPathS);
	  	free(fOrDPathB);
	}

}



// void prntTree(treeNode_t *root,int level, dirList_t *nodelist){
//   	treeNode_t *current;
// 	int filelevel=level+1;
// 	if(root==NULL)
// 		return;
//   	if(level>0){
// 		printf("%*s",level, "\t");
//   	}
//   	printf("\n%s------>\n",root->Sname);

//   	if(level == 0){
//   		dirNodeInsert(nodelist,root->fullpath,current);
//   	}

//   	current=root->list->head;
//   	if(current==NULL){
//     	return;
//   	}
//   	while(current!=NULL){
//       	printf("%*s", filelevel, "\t");
//       	printf("%s(%c)",current->Sname,current->type);

//     	if(current->type=='d'){
//     		dirNodeInsert(nodelist,current->fullpath,current);
//     	}
//     	current=current->next;  
//   	}

//   	printf("\n");
//   	current=root->list->head;
//   	while(current!=NULL){
//   		if(current->type=='d'){
//       		prntTree(current,level+1, nodelist); 
//     	} 
//     	current=current->next;
//   	}
// }