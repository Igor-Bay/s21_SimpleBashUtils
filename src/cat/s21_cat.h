#ifndef SRC_S21_CAT_H_
#define SRC_S21_CAT_H_

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 4096

typedef struct args {
  int arg_b;
  int arg_e;
  int arg_n;
  int arg_s;
  int arg_tab;
  int arg_u;
  int arg_v;
  int arg_count;
} args;


void file_processor(char *filename, args *arg_set);
void logic(args *arg_set, char *input_string, int *empty_str_count,
           int *str_count);

void check_args(int argc, char **argv, args *arg_set);
void s21_cat_s(char *input_string, int *empty_str_count);
void s21_cat_tab(char *input_string);
void s21_cat_b(int *empty_str_count, int *str_count);
void s21_cat_n(char *input_string, int *str_count);
void s21_cat_e(char *input_string);
void s21_cat_v(char *input_string);

#endif  // SRC_S21_CAT_H_
