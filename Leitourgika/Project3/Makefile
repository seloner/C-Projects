OBJS 	= myport.o functs.o
SOURCE	= myport.c functs.c
HEADER  = myport.h	functs.h
OUT 	= myport 

OBJS2 	= vessel.o 
SOURCE2	= vessel.c 
HEADER2  = functs.h
OUT2 	= vessel 

OBJS3 	= port-master.o  functs.o
SOURCE3	= port-master.c 
HEADER3  = functs.h
OUT3 	= port-master 

OBJS4 	= monitor.o  functs.o
SOURCE4	= monitor.c 
HEADER4 = functs.h
OUT4 	= monitor 

CC	= gcc
FLAGS   = -c 

all: $(OBJS) $(OBJS2) $(OBJS3) $(OBJS4)
	$(CC) -g $(OBJS)  -pthread -o $(OUT)
	$(CC) -g $(OBJS2) -pthread -o $(OUT2) 
	$(CC) -g $(OBJS3) -pthread -o $(OUT3)
	$(CC) -g $(OBJS4) -pthread -o $(OUT4)

myport.o: myport.c
	$(CC) $(FLAGS)  myport.c

functs.o: functs.c
	$(CC) $(FLAGS)  functs.c

vessel.o: vessel.c
	$(CC) $(FLAGS)  vessel.c

port-master.o: port-master.c
	$(CC) $(FLAGS)  port-master.c

monitor.o: monitor.c
	$(CC) $(FLAGS) monitor.c
clean:
	rm -f $(OBJS) $(OUT) $(OBJS2) $(OUT2) $(OBJS3) $(OUT3) $(OBJS4) $(OUT4) 
count:
	wc -l $(SOURCE) $(HEADER) $(SOURCE2)  $(SOURCE3) $(SOURCE4)