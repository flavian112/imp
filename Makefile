CC        := gcc
CFLAGS    := -Wall -g -I.
BISON     := bison
FLEX      := flex

BUILD_DIR := build

PARSER_Y  := parser.y
LEXER_L   := lexer.l
DRIVER_C  := driver.c
AST_C     := ast.c
AST_H     := ast.h

PARSER_C  := $(BUILD_DIR)/parser.tab.c
PARSER_H  := $(BUILD_DIR)/parser.tab.h
LEXER_C   := $(BUILD_DIR)/lex.yy.c
OBJS      := $(BUILD_DIR)/parser.tab.o $(BUILD_DIR)/lex.yy.o $(BUILD_DIR)/driver.o $(BUILD_DIR)/ast.o

TARGET    := $(BUILD_DIR)/imp

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(PARSER_C) $(PARSER_H): $(PARSER_Y) | $(BUILD_DIR)
	$(BISON) -d --defines=$(PARSER_H) -o $(PARSER_C) $(PARSER_Y)

$(LEXER_C): $(LEXER_L) $(PARSER_H) | $(BUILD_DIR)
	$(FLEX) -o $@ $<

$(BUILD_DIR)/imp_parser.tab.o: $(PARSER_C) $(PARSER_H)
	$(CC) $(CFLAGS) -c $(PARSER_C) -o $@

$(BUILD_DIR)/lex.yy.o: $(LEXER_C) $(PARSER_H)
	$(CC) $(CFLAGS) -c $(LEXER_C) -o $@

$(BUILD_DIR)/driver.o: $(DRIVER_C) $(PARSER_H) $(AST_H) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(DRIVER_C) -o $@

$(BUILD_DIR)/ast.o: $(AST_C) $(AST_H) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(AST_C) -o $@

run: $(TARGET)
	./$(TARGET) example.imp

clean:
	rm -rf $(BUILD_DIR)

