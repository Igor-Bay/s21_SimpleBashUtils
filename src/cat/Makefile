CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -Werror
BUILD_PATH=./

all: s21_cat 

rebuild: clean all

clean: 
	rm -rf $(BUILD_PATH)s21_cat $(BUILD_PATH)*.o

s21_cat: s21_cat_funcs
	$(CC) $(CFLAGS) $(BUILD_PATH)s21_cat_funcs.o -o $(BUILD_PATH)s21_cat $(BUILD_PATH)s21_cat.c

s21_cat_funcs:
	$(CC) -c $(CFLAGS) $(BUILD_PATH)s21_cat_funcs.c -o $(BUILD_PATH)s21_cat_funcs.o 
