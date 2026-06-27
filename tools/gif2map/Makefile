CC ?= cc
AR ?= ar
UNITY_DIR ?= /home/lgms/code/research/Unity
BUILD_DIR ?= build

CFLAGS ?= -std=c99 -Wall -Wextra -pedantic -O2
CPPFLAGS += -I. -I$(UNITY_DIR)/src

LIB_OBJS = $(BUILD_DIR)/gif2map.o
LIB = $(BUILD_DIR)/libgif2map.a
TEST_BIN = $(BUILD_DIR)/test_gif2map
CLI_BIN = $(BUILD_DIR)/gif2map
MCP_BIN = $(BUILD_DIR)/gif2map_mcp

.PHONY: all lib cli mcp test clean

all: lib cli mcp

lib: $(LIB)

cli: $(CLI_BIN)

mcp: $(MCP_BIN)

test: $(TEST_BIN)
	./$(TEST_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/gif2map.o: gif2map.c gif2map.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c gif2map.c -o $@

$(LIB): $(LIB_OBJS)
	$(AR) rcs $@ $^

$(BUILD_DIR)/gif2map_cli.o: gif2map_cli.c gif2map.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c gif2map_cli.c -o $@

$(CLI_BIN): $(BUILD_DIR)/gif2map_cli.o $(LIB)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/gif2map_mcp.o: gif2map_mcp.c gif2map.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c gif2map_mcp.c -o $@

$(BUILD_DIR)/jsonp.o: jsonp.c jsonp.h third_party/jsmn.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c jsonp.c -o $@

$(MCP_BIN): $(BUILD_DIR)/gif2map_mcp.o $(BUILD_DIR)/jsonp.o $(LIB)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/test_gif2map.o: test_gif2map.c gif2map.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c test_gif2map.c -o $@

$(BUILD_DIR)/unity.o: $(UNITY_DIR)/src/unity.c | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(TEST_BIN): $(BUILD_DIR)/test_gif2map.o $(BUILD_DIR)/unity.o $(LIB)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD_DIR)
