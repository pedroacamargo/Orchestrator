CC = gcc
CFLAGS = -Wall -g -I include/
LDFLAGS = 
FILES = obj/Orchestrator/lib.o obj/Orchestrator/FCFS.o obj/Orchestrator/SJF.o obj/Orchestrator/handleFiles.o

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
	rm -f tmp/* client orchestrator fifo



# 	#================================================
# CFLAGS = -Wall -Wextra -pedantic -O2
# LIBS = -lm
# CC = gcc
# #------------------------------------------------
# BIN_DIR = bin
# BUILD_DIR = build
# INCLUDES_DIR = includes
# SRC_DIR = src
# #------------------------------------------------
# SRC = $(wildcard $(SRC_DIR)/*.c)
# OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
# # src/main.c -> build/main.o
# #================================================
# .DEFAULT_GOAL = build

# build: setup all state

# # This will create the ./game in the project root folder
# all: $(OBJS) $(MAP_OBJ)
# 	$(CC) -I $(INCLUDES_DIR) $(CFLAGS) -o $@ $^ $(LIBS)

# # This will create the .o files in SO/build
# $(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
# 	$(CC) -c -I $(INCLUDES_DIR) $(CFLAGS) -o $@ $^

# # Quando o target de uma função é só um target e não um ficheiro, utilizamos o .PHONY
# .PHONY: setup
# setup:
# 	@echo "Building ..."
# 	mkdir -p $(BUILD_DIR)

# .PHONY: state
# state:
# 	@echo "Done!"

# .PHONY: clean
# clean:
# 	@echo "Removing everything..."
# 	-rm -r $(BUILD_DIR)
# 	-rm all
# 	@echo "The executable was deleted successfully"