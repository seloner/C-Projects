#ifndef read_H_
#define read_H_
#define SIZEofBUFF 20
#define SSizeofBUFF 6
#include <time.h>

typedef struct record{
	long  	custid;
	char 	FirstName[SIZEofBUFF];
	char 	LastName[SIZEofBUFF];
	char	Street[SIZEofBUFF];
	int 	HouseID;
	char	City[SIZEofBUFF];
	char	postcode[SSizeofBUFF];
	float  	amount;
	
}record_t;

typedef struct piperecord{
	long  	custid;
	char 	FirstName[SIZEofBUFF];
	char 	LastName[SIZEofBUFF];
	char	Street[SIZEofBUFF];
	int 	HouseID;
	char	City[SIZEofBUFF];
	char	postcode[SSizeofBUFF];
	float  	amount;
	int 	flaglast;
	
}pipe_record_t;

typedef struct info{
	float totalsearchtime;
	float time;
	float searcherstime;
	float splitNmergetime;
	float maxsearcher;
	float minsearcher;
	float maxsplitmerge;
	float minsplitmerge;

}info_t;

int searchforsubstring(record_t rec,char *substring);

void copyrecord(record_t *record,pipe_record_t *piperecord,int flaglast);

size_t writetopipe(int fd,pipe_record_t *piperec,size_t structsize);

#endif