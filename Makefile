CC = gcc
CFLAGS = -Wall -g -I include/
LDFLAGS = 
FILES = obj/Orchestrator/lib.o obj/Orchestrator/FCFS.o obj/Orchestrator/SJF.o 

all: folders server client

server: orchestrator

client: client

folders:
	@mkdir -p src include obj tmp obj/Orchestrator obj/Client

orchestrator: obj/Orchestrator/orchestrator.o $(FILES)
	$(CC) $(LDFLAGS) $^ -o $@

client: obj/Client/client.o $(FILES)
	$(CC) $(LDFLAGS) $^ -o $@

# Regra para arquivos .c no diretório src
obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para arquivos .c em diretórios de segundo nível de src
obj/Orchestrator/%.o: src/Orchestrator/%.c
	$(CC) $(CFLAGS) -c $< -o $@

obj/Client/%.o: src/Client/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	find obj/ -type f -delete
	find obj/ -type d -empty -delete
	rm -rf tmp/ client orchestrator fifo_client fifo_server 