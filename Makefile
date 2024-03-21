#================================================
CFLAGS = -Wall -Wextra -pedantic -O2
LIBS = -lm
CC = gcc
#------------------------------------------------
BIN_DIR = bin
BUILD_DIR = build
INCLUDES_DIR = includes
SRC_DIR = src
#------------------------------------------------
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
# src/main.c -> build/main.o
#------------------------------------------------
PROGRAM = orchestrator
#================================================
.DEFAULT_GOAL = build

build: setup $(PROGRAM) state

# This will create the ./game in the project root folder
$(PROGRAM): $(OBJS) $(MAP_OBJ)
	$(CC) -I $(INCLUDES_DIR) $(CFLAGS) -o $@ $^ $(LIBS)

# This will create the .o files in SO/build
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c -I $(INCLUDES_DIR) $(CFLAGS) -o $@ $^

# Quando o target de uma função é só um target e não um ficheiro, utilizamos o .PHONY
.PHONY: setup
setup:
	@echo "Building ..."
	mkdir -p $(BUILD_DIR)

.PHONY: state
state:
	@echo "Done!"

.PHONY: clean
clean:
	@echo "Removing everything..."
	-rm -r $(BUILD_DIR)
	-rm $(PROGRAM)
	@echo "The executable was deleted successfully"
