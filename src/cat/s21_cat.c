#include "s21_cat.h"

int main(int argc, char **argv) {
  args arg_set = {0, 0, 0, 0, 0, 0, 0, 0};
  check_args(argc, argv, &arg_set);

  for (int i = arg_set.arg_count + 1; i < argc; ++i) {
    file_processor(argv[i], &arg_set);
  }
  return 0;
}

void file_processor(char *filename, args *arg_set) {
  char input_string[MAX] = {'\0'};
  int empty_str_count = 0;
  int str_count = 1;

  FILE *input_file = fopen(filename, "r");
  if (input_file != NULL) {
    while (fgets(input_string, MAX, input_file) != NULL) {
      logic(arg_set, input_string, &empty_str_count, &str_count);
    }
    fclose(input_file);
  } else {
    fprintf(stderr, "s21_cat: %s: No such file or directory\n", filename);
  }
}

void logic(args *arg_set, char *input_string, int *empty_str_count,
           int *str_count) {
  if (input_string[0] == '\n') {
    (*empty_str_count)++;
  } else {
    (*empty_str_count) = 0;
  }

  if (arg_set->arg_s > 0) {
    s21_cat_s(input_string, empty_str_count);
  }

  if (arg_set->arg_tab > 0) {
    s21_cat_tab(input_string);
  }

  if (arg_set->arg_b > 0) {
    s21_cat_b(empty_str_count, str_count);
  } else if (arg_set->arg_n > 0) {
    s21_cat_n(input_string, str_count);
  }

  if (arg_set->arg_v > 0) {
    if (arg_set->arg_e > 0) {
      s21_cat_e(input_string);
    }
    s21_cat_v(input_string);
  } else {
    printf("%s", input_string);
  }
}
