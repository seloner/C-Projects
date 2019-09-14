OBJS 	= main.o Ht.o Sht.o
SOURCE	= main.c Ht.c Sht.c
HEADER  = BF.h 	Ht.h Sht.h
OUT  	= askisi
CC	= gcc
FLAGS   = -g -c 
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS) 
	$(CC) -g -no-pie $(OBJS) BF_64.a -o $(OUT)


# create/compile the individual files >>separately<< 
main.o: main.c
	$(CC) $(FLAGS)  main.c

Ht.o: Ht.c
	$(CC) $(FLAGS) Ht.c

Sht.o: Sht.c
	$(CC) $(FLAGS) Sht.c


# clean house
clean:
	rm -f $(OBJS) $(OUT) 


# do a bit of accounting
count:
	wc -l $(SOURCE) $(HEADER) 