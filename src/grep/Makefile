CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -Werror
BUILD_PATH=./

all: s21_grep

rebuild: clean all

clean: 
	rm -rf $(BUILD_PATH)s21_grep $(BUILD_PATH)*.o

s21_grep: s21_grep_funcs
	$(CC) $(CFLAGS) $(BUILD_PATH)s21_grep_funcs.o -o $(BUILD_PATH)s21_grep $(BUILD_PATH)s21_grep.c

s21_grep_funcs:
	$(CC) -c $(CFLAGS) $(BUILD_PATH)s21_grep_funcs.c -o $(BUILD_PATH)s21_grep_funcs.o