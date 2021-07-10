CC=gcc
PROGRAMS = server client 
NAME=shared-memory
FLAGS=-Wextra -Wall

all: $(PROGRAMS)

out: $(PROGRAMS)
	for PROGRAM in ${PROGRAMS} ; do \
		cp $$PROGRAM "$(BIN)/$(NAME)-$$PROGRAM"; \
	done

server: server.c utils.h utils.o
	$(CC) -o server server.c utils.o -lpthread -lrt $(FLAGS) -g

client: client.c utils.h utils.o
	$(CC) -o client client.c utils.o -lpthread -lrt $(FLAGS) -g
	
utils.o: utils.c
	$(CC) -c utils.c -o utils.o $(FLAGS) -g

clean:
	-rm server client utils.o