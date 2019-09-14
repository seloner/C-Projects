OBJS 	= root.o 
SOURCE	= root.c 
HEADER  = read.h 
OUT  	= myfind
OBJS2 	= leaf.o read.o 
SOURCE2	= leaf.c read.c
HEADER2  = read.h
OUT2  	= leaf
OBJS3 	= splitNmerge.o 
SOURCE3	= splitNmerge.c 
HEADER3  = read.h
OUT3  	= splitNmerge
CC	= gcc
FLAGS   = -g -c 
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS) $(OBJS2) $(OBJS3)
	$(CC) -g $(OBJS) -o $(OUT) -lm
	$(CC) -g $(OBJS2) -o $(OUT2)
	$(CC) -g $(OBJS3) -o $(OUT3)

# create/compile the individual files >>separately<< 
root.o: root.c
	$(CC) $(FLAGS)  root.c

leaf.o: leaf.c
	$(CC) $(FLAGS) leaf.c

splitNmerge.o: splitNmerge.c
	$(CC) $(FLAGS) splitNmerge.c
read.o: read.c
	$(CC) $(FLAGS) read.c		


# clean house
clean:
	rm -f $(OBJS) $(OUT) $(OBJS2) $(OUT2) $(OBJS3) $(OUT3)


# do a bit of accounting
count:
	wc -l $(SOURCE) $(HEADER) $(SOURCE2) $(HEADER2) $(SOURCE3) $(HEADER3)