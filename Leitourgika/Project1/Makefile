
OBJS 	= main.o graph.o trie.o helpfuncts.o
SOURCE	= main.c graph.c trie.c helpfuncts.c
HEADER  = graph.h trie.h helpfuncts.h
OUT  	= mygraph
CC	= gcc
FLAGS   = -g -c 
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

# create/compile the individual files >>separately<< 
main.o: main.c
	$(CC) $(FLAGS) main.c

graph.o: graph.c
	$(CC) $(FLAGS) graph.c

trie.o: trie.c
	$(CC) $(FLAGS) trie.c

helpfuncts.o: helpfuncts.c
	$(CC) $(FLAGS) helpfuncts.c	

# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc -l $(SOURCE) $(HEADER)
