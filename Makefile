CC ?= clang
CFLAGS ?= -Wall -Wextra -g
LDFLAGS ?= -lreadline
BISON ?= bison
FLEX ?= flex

SRC_DIR := src
INC_DIR := include
BUILD_DIR := build

PARSER_Y := $(SRC_DIR)/parser.y
LEXER_L := $(SRC_DIR)/lexer.l

C_SRC := $(filter-out $(PARSER_Y:.y=.c) $(LEXER_L:.l=.c), $(wildcard $(SRC_DIR)/*.c))

PARSER_C := $(BUILD_DIR)/parser.tab.c
PARSER_H := $(BUILD_DIR)/parser.tab.h
LEXER_C := $(BUILD_DIR)/lex.yy.c

C_OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SRC))
PARSER_O := $(BUILD_DIR)/parser.tab.o
LEXER_O := $(BUILD_DIR)/lex.yy.o
OBJS := $(C_OBJS) $(PARSER_O) $(LEXER_O)

TARGET := $(BUILD_DIR)/imp

CFLAGS += -I$(INC_DIR) -MMD -MP
DEPS := $(OBJS:.o=.d)

.PHONY: all clean example repl

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR):
	@mkdir -p $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PARSER_C) $(PARSER_H): $(PARSER_Y) | $(BUILD_DIR)
	$(BISON) -d --defines=$(PARSER_H) -o $(PARSER_C) $<

$(PARSER_O): $(PARSER_C)
	$(CC) $(CFLAGS) -c $< -o $@

$(LEXER_C): $(LEXER_L) $(PARSER_H) | $(BUILD_DIR)
	$(FLEX) -o $@ $<

$(LEXER_O): $(LEXER_C)
	$(CC) $(CFLAGS) -c $< -o $@

example: $(TARGET)
	./$(TARGET) -i examples/example.imp

repl: $(TARGET)
	./$(TARGET)

clean:
	@rm -rf $(BUILD_DIR)

-include $(DEPS)
