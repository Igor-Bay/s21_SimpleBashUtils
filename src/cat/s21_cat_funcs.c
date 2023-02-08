#include "s21_cat.h"

void check_args(int argc, char **argv, args *arg_set) {
  for (; argc > 1 && argv[1][0] == '-'; --argc, ++argv) {
    for (size_t i = 1; i < strlen(argv[1]); ++i) {
      if (argv[1][i] == 'b') {
        arg_set->arg_b = 1;
      } else if (argv[1][i] == 'e') {
        arg_set->arg_e = arg_set->arg_v = 1;
      } else if (argv[1][i] == 'n') {
        arg_set->arg_n = 1;
      } else if (argv[1][i] == 's') {
        arg_set->arg_s = 1;
      } else if (argv[1][i] == 't') {
        arg_set->arg_tab = arg_set->arg_v = 1;
      } else if (argv[1][i] == 'u') {
        arg_set->arg_u = 1;
      } else if (argv[1][i] == 'v') {
        arg_set->arg_v = 1;
      } else {
        fprintf(stderr, "s21_cat: illegal option -- %c\n", argv[1][i]);
        fprintf(stderr, "usage: s21_cat [-benstuv] [file ...]\n");
        exit(1);
      }
    }
    ++(arg_set->arg_count);
  }
}

void s21_cat_s(char *input_string, int *empty_str_count) {
  if ((*empty_str_count) > 1) {
    input_string[0] = '\0';
  }
}

void s21_cat_tab(char *input_string) {
  char *tab = NULL;
  while ((tab = strchr(input_string, '\t')) != NULL) {
    int len = strlen(tab);
    memmove(tab + 2, tab + 1, len);
    tab[0] = '^';
    tab[1] = 'I';
  }
}

void s21_cat_b(int *empty_str_count, int *str_count) {
  if ((*empty_str_count) == 0) {
    printf("%6d\t", (*str_count)++);
  }
}

void s21_cat_n(char *input_string, int *str_count) {
  if (input_string[0] != '\0') {
    printf("%6d\t", (*str_count)++);
  }
}

void s21_cat_e(char *input_string) {
  char *e = NULL;
  if ((e = strchr(input_string, '\n')) != NULL) {
    e[0] = '$';
    e[1] = '\n';
    e[2] = '\0';
  }
}

void s21_cat_v(char *input_string) {
  unsigned char c = 32;
  int i = 0;
  while ((c = input_string[i++]) != '\0') {
    if (c < 32) {
      if (c == '\n' || c == '\t') {
        putchar(c);
      } else {
        putchar('^');
        putchar(c + 64);
      }
    } else {
      if (c > 0177) {
        printf("M-");
        c &= 0177;
      }
      if (c < ' ') {
        printf("^%c", c + '@');
      } else if (c == 0177) {
        printf("^?");
      } else {
        putchar(c);
      }
    }
  }
}
