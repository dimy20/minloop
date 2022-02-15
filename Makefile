GCC=gcc
CFLAGS= -Wall -g -c 
INCLUDES=-I
LIBS=-l

BIN_DIR=bin
SRC_DIR=src
BUILD_DIR=build
INCLUDE_DIR=include

MAIN=$(BIN_DIR)/main

SRCS = $(wildcard $(SRC_DIR)/*.c)
SRC_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

all: $(MAIN)

$(MAIN): $(SRC_OBJS) $(BUILD_DIR)/main.o
	$(GCC) -o $(MAIN) $(BUILD_DIR)/main.o $(SRC_OBJS)
	@echo "[x] building done."

$(BUILD_DIR)/main.o: main.c
	$(GCC) $(CFLAGS) main.c -o $@

$(SRC_OBJS): $(SRCS)
	$(GCC) $(CFLAGS) $(SRCS) $@


show:
	@echo $(SRCS)
	@echo $(SRC_OBJS)

clean:
	rm $(BUILD_DIR)/*.o $(BIN_DIR)/*




