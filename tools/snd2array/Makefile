CC ?= cc
AR ?= ar
BUILD_DIR ?= build

CFLAGS ?= -std=c99 -Wall -Wextra -pedantic -O2 -D_DEFAULT_SOURCE
CPPFLAGS += -I.

LIB_OBJS = $(BUILD_DIR)/snd2array.o
LIB = $(BUILD_DIR)/libsnd2array.a
CLI_BIN = $(BUILD_DIR)/snd2array
MCP_BIN = $(BUILD_DIR)/snd2array_mcp

.PHONY: all lib cli mcp clean

all: lib cli mcp

lib: $(LIB)

cli: $(CLI_BIN)

mcp: $(MCP_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/snd2array.o: snd2array.c snd2array.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c snd2array.c -o $@

$(LIB): $(LIB_OBJS)
	$(AR) rcs $@ $^

$(BUILD_DIR)/snd2array_cli.o: snd2array_cli.c snd2array.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c snd2array_cli.c -o $@

$(CLI_BIN): $(BUILD_DIR)/snd2array_cli.o $(LIB)
	$(CC) $(CFLAGS) $^ -o $@ -lm

$(BUILD_DIR)/snd2array_mcp.o: snd2array_mcp.c snd2array.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c snd2array_mcp.c -o $@

$(BUILD_DIR)/jsonp.o: jsonp.c jsonp.h third_party/jsmn.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c jsonp.c -o $@

$(MCP_BIN): $(BUILD_DIR)/snd2array_mcp.o $(BUILD_DIR)/jsonp.o $(LIB)
	$(CC) $(CFLAGS) $^ -o $@ -lm

clean:
	rm -rf $(BUILD_DIR)
