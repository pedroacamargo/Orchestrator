CC = gcc
CFLAGS = -Wall -g -I include/
LDFLAGS = 
FILES = obj/exec.o obj/FCFS.o obj/SJF.o obj/handleFiles.o 

all: folders server client

server: orchestrator

client: client

folders:
	@mkdir -p src include obj tmp

orchestrator: obj/orchestrator.o $(FILES)
	$(CC) $(LDFLAGS) $^ -o $@

client: obj/client.o
	$(CC) $(LDFLAGS) $^ -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f obj/* tmp/* client orchestrator

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
