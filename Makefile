GCC=gcc
CFLAGS=-Wall -g -c 
INCLUDES=-I
LIBS=-l

BIN_DIR=bin
SRC_DIR=src
BUILD_DIR=build
INCLUDE_DIR=include
MAIN=$(BIN_DIR)/main
SRCS = $(wildcard $(SRC_DIR)/*.c)
SRC_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

#tests config
TEST_DIR=tests
TEST_SRCS= $(wildcard $(TEST_DIR)/*.c)
TEST_BIN=$(patsubst $(TEST_DIR)/%.c, $(TEST_DIR)/bin/%, $(TEST_SRCS))
TEST_OBJS=$(patsubst $(TEST_DIR)/%.c, $(TEST_DIR)/%.o, $(TEST_SRCS))

tests: $(SRC_OBJS) $(TEST_BIN)
	for test in $(TEST_BIN); do ./$$test ; done

$(TEST_DIR)/bin/%: $(TEST_DIR)/%.o $(SRC_OBJS)
	$(GCC) -o $@ $< $(SRC_OBJS)

$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	$(GCC) -c $< -o $@

all: $(MAIN)

$(MAIN): $(SRC_OBJS) $(BUILD_DIR)/main.o
	$(GCC) -o $(MAIN) $(BUILD_DIR)/main.o $(SRC_OBJS)


$(BUILD_DIR)/main.o: main.c
	$(GCC) $(CFLAGS) main.c -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(GCC) $(CFLAGS) $< -o $@

show:
	@echo $(TEST_SRCS)
	@echo $(TEST_BIN)

clean:
	rm $(BUILD_DIR)/*.o $(BIN_DIR)/*

clean_test:
	rm $(TEST_DIR)/bin/*



