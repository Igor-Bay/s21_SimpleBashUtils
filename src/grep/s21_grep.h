#ifndef SRC_S21_GREP_H_
#define SRC_S21_GREP_H_

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 4096
#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))

typedef struct {
  int arg_e;    // флаг и счётчик шаблонов
  int ef_flag;  //флаг наличия -e -f
  int arg_i;  // флаг игнора различий регистра в шаблоне
  int arg_v;  // флаг инверсии смысла поиска соответствий
  int arg_c;  // флаг вывода только совпадающих строк
  int arg_l;  // флаг вывода только совпадающих файлов
  int arg_n;  // флаг вывода номера строки перед каждой строкой
  int arg_h;  // флаг отключения имени файлов при выводе строк
  int arg_s;  // флаг подавления сообщения об ошибках файлов
  int arg_f;  // флаг получения рег выражений из файла
  int arg_o;  // флаг печати только совпадающих частей совпавшей строки
  int file_args;  // счётчик имён файлов в командной строке
  int arg_count;  // счётчик строк, начинающихся с '-' в командной строке
} args;


void processor(char *filename, char **patterns, args *arg_set, regex_t *regex);
void printer(args *arg_set, int match, char *filename, char *input_string,
             int str_count, int mtched_str, regex_t *regex, char **patterns);

void usage_message_exit();
void check_cmd_line(int argc, char **argv, args *arg_set, char ***patterns,
                    char *filenames[]);
int check_if_ef(int argc, char **argv);
void check_args(int argc, char **argv, args *arg_set, char ***patterns,
                char *filenames[]);
void check_arg_symbol(int argc, char **argv, args *arg_set, char ***patterns,
                      int *flag_e, int *flag_f);
void pat_from_str(char ***patterns, char *string, args *arg_set);
void pat_from_file(char ***patterns, char *filename, args *arg_set);
int pat2ex(char *pattern, regex_t *regex, args *arg_set);
int find_string(regex_t *regex, char *input_string);
void print_match(regex_t *regex, char *input_string);

void free_str_arr(char ***str_arr, const int size);
void realloc_str_arr(char ***str_arr, args *arg_set);
void print_str(char *str);

#endif  // SRC_S21_GREP_H_
