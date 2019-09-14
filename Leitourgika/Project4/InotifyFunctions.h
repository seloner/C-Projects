//InotifyFunctions.h
#include <sys/inotify.h>
typedef struct dirListNode dirListNode_t;
//queue-----------------//
typedef struct dirListNode{		//anaparastash queue me lista
	char *path;
	int wd;
	treeNodeList_t *fatherList;
	treeNode_t *treeNode;
	dirListNode_t *next;
}dirListNode_t;

typedef struct dirList{
	int numOfNodes;	
	dirListNode_t *head;
	dirListNode_t *last;

}dirList_t;


//The fixed size of the event buffer:
#define EVENT_SIZE  ( sizeof (struct inotify_event) )

//The size of the read buffer: estimate 1024 events with 16 bytes per name over and above the fixed size above
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

dirList_t *dirNodeListInit(void);
dirListNode_t *dirNodeInit(char *path, treeNode_t *treeNode, treeNodeList_t *fatherList);
void dirNodeInsert(dirList_t *nodelist, char *path, treeNode_t *treeNode, treeNodeList_t *fatherList);
void fillDirList(treeNode_t *root,int level, dirList_t *nodelist);
dirListNode_t * dirNodeFind(dirList_t *nodelist, int wd);
dirListNode_t * dirNodeFindByPath(dirList_t *nodelist, char *path);
void deletedirNodeList(dirList_t *nodelist);

void removeWatch(int fd, dirList_t *nodelist);

char *pathCreator(char *name, char *pathToFather);

void event_name(int fd, struct inotify_event *event, dirList_t *nodelist, dirList_t *nodelistB,inodeList_t *backUpinodeList,inodeList_t *sourceinodeList);
void fun(dirList_t *nodelist, dirList_t *nodelistB, treeNode_t* root,treeNode_t *backupRoot,inodeList_t *backUpinodeList,inodeList_t *sourceinodeList);
void fail(const char *message);
const char * target_type(struct inotify_event *event);
const char * target_name(struct inotify_event *event);
// const char * event_name(struct inotify_event *event);