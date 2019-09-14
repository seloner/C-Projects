OBJS 	= main.o Structures.o CreateTree.o InotifyFunctions.o
SOURCE	= main.c Structures.c CreateTree.c InotifyFunctions.c
HEADER  = Structures.h 	CreateTree.h InotifyFunctions.h
OUT  	= mirr
CC	= gcc
FLAGS   = -g -c 
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS) 
	$(CC) -g $(OBJS) -o $(OUT)


# create/compile the individual files >>separately<< 
main.o: main.c
	$(CC) $(FLAGS)  main.c

Structures.o: Structures.c
	$(CC) $(FLAGS) Structures.c

CreateTree.o: CreateTree.c
	$(CC) $(FLAGS) CreateTree.c	

InotifyFunctions.o: InotifyFunctions.c
	$(CC) $(FLAGS) InotifyFunctions.c


# clean house
clean:
	rm -f $(OBJS) $(OUT) 


# do a bit of accounting
count:
	wc -l $(SOURCE) $(HEADER) 
