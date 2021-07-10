CC=gcc
PROGRAMS = server client 
NAME=socket
FLAGS=-Wextra -Wall

all: $(PROGRAMS)

out: $(PROGRAMS)
	for PROGRAM in ${PROGRAMS} ; do \
		cp $$PROGRAM "$(BIN)/$(NAME)-$$PROGRAM"; \
	done

server: utils.o server.c
	$(CC) -o server server.c utils.o -lpthread $(FLAGS)

client: utils.o client.c
	$(CC) -o client client.c utils.o $(FLAGS)

utils.o: utils.c
	$(CC) -c utils.c -o utils.o $(FLAGS)

clean:
	-rm server client
	-rm utils.o