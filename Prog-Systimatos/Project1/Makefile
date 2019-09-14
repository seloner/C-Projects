#

OBJS 	= main.o trie.o diavasma.o 
SOURCE	= main.c trie.c diavasma.c
HEADER  = TRIE.h  DIAVASMA.H 
OUT  	= minisearch
CC	= gcc
FLAGS   = -g -c -lm
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS)
	$(CC) -g  $(OBJS) -o $(OUT) -lm

# create/compile the individual files >>separately<< 
main.o: main.c
	$(CC)  $(FLAGS) main.c

trie.o: trie.c
	$(CC) $(FLAGS) trie.c
diavasma.o: diavasma.c
	$(CC)  $(FLAGS) diavasma.c


# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
