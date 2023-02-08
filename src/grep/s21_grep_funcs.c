/*
Необходимо разработать утилиту s21_grep:
  "grep prints lines that contain a match for one or more patterns."
                                                    (c) GNU Grep

  Поддержка флагов:
    -e Шаблон;
    -i Игнорирует различия регистра;
    -v Инвертирует смысл поиска соответствий;
    -c Выводит только количество совпадающих строк;
    -l Выводит только совпадающие файлы;
    -n Предваряет каждую строку вывода номером строки из файла ввода;
    -h Выводит совпадающие строки, не предваряя их именами файлов;
    -s Подавляет сообщения об ошибках о несуществующих или нечитаемых файлах;
    -f file Получает регулярные выражения из файла;
    -o Печатает только совпадающие (непустые) части совпавшей строки;

  включая их парные комбинации (например, -iv, -in);

  Ввод через stdin обрабатывать не обязательно.

  В файле s21_grep.с написаны ф-ии, отвечающие за формирование логики
  работы программы, в s21_grep_funcs.с ф-и, обслуживающие эту логику.
*/

#include "s21_grep.h"

/*
  Ф-я usage_message_exit() выводит на экран сообщение о допустимых
  аргументах командной строки и завершает программу.
*/
void usage_message_exit() {
  fprintf(stderr, "usage: s21_grep [-chilnsv]\n");
  fprintf(stderr, "\t[-e pattern] [-f file]\n");
  fprintf(stderr, "\tpattern [file ...]\n");
  exit(1);
}

/*
  Ф-я check_cmd_line(...) запускает парсер введённой командной строки
  и если обнаружены только флаги (считаем "флаговые строки" arg_set->arg_count)
  без шаблонов и имён файлов, вызывает ф-ю, печатающую "правила ввода"
*/
void check_cmd_line(int argc, char **argv, args *arg_set, char ***patterns,
                    char *filenames[]) {
  check_args(argc, argv, arg_set, patterns, filenames);
  if (argc == 1 ||
      arg_set->arg_count + 1 == argc) {  // в строке только аргументы
    usage_message_exit();
  }
}

/*
  Ф-я check_args(...) обеспечивает следующую логику:
- предварительная проверка на наличие флагов -e -f с помощью ф-ии
check_if_ef(...);

- цикл двигается по argv для анализа каждой строки с аргументом;
- если строка начинается с '-', то запускаем анализ флагов check_arg_symbol(...)
  и после увеличиваем счётчик кол-ва "флаговых строк" (arg_set->arg_count);
- если нет, то это
  - либо шаблон (если на первом шаге не обнаружено флага -e),
  - либо имя файла;

  flag_e и flag_f - это вспомогательные флаги, которые поднимаются, когда
  ф-я check_arg_symbol(...) обнаружит -e или -f.
- если на предыдущей строке флаг был поднят, то текущая строка игнорируется,
  чтобы она лишний раз не попала в шаблоны или имена файлов;
*/
void check_args(int argc, char **argv, args *arg_set, char ***patterns,
                char *filenames[]) {
  int flag_e = 0, flag_f = 0;
  arg_set->ef_flag = check_if_ef(argc, argv);

  for (; argc > 1; --argc, ++argv) {
    if (!flag_e && !flag_f) {
      if (argv[1][0] == '-') {
        check_arg_symbol(argc, argv, arg_set, patterns, &flag_e, &flag_f);
        ++(arg_set->arg_count);
      } else if (!(arg_set->ef_flag)++) {
        pat_from_str(patterns, argv[1], arg_set);
      } else
        filenames[(arg_set->file_args)++] = argv[1];
    } else
      flag_e = flag_f = 0;
  }
}

/*
  Ф-я check_if_e(...) проверяет, есть ли в строке флаги -e, чтобы
  вызывающая ф-я определила своё поведение в отношении первой встреченной
  строки без '-'.
*/
int check_if_ef(int argc, char **argv) {
  int ef_flag = 0;
  for (int i = 1; ef_flag == 0 && i < argc; ++i)
    if (argv[i][0] == '-')
      for (size_t j = 1; j < strlen(argv[i]); ++j)
        if (argv[i][j] == 'e' || argv[i][j] == 'f') {
          ef_flag = 1;
          break;
        }

  return ef_flag;
}

/*
  Ф-я check_arg_symbol(...) работает со строками, начинающимися на
  '-' и занимается:
- анализом флагов, обновляя значения в структур arg_set;
- если встретился флаг -e, вызывая ф-ю pat_from_str(...), передавая ей
  - либо остаток строки, в которой встртился флаг 'e',
  - либо следующую строку из argv, если 'e', был последним символом или,
  - или пустую строку, если строка с 'e' была последней в argv;
- аналогично действует при вхождении флага 'f', вызывая ф-ю pat_from_ file(...);
    если оба флага 'f' и 'e' входят в одну и ту же строку,
    приоретет у первого обнаруженного;
- если попался символ, не являющийся допустимым флагом,
  вызываем ф-ю с сообщением о дупустимых символах.
*/
void check_arg_symbol(int argc, char **argv, args *arg_set, char ***patterns,
                      int *flag_e, int *flag_f) {
  char empty[] = "";
  for (size_t i = 1; i < strlen(argv[1]); ++i) {
    if (argv[1][i] == 'e') {
      if (argv[1][i + 1] != '\0') {
        pat_from_str(patterns, argv[1] + i + 1, arg_set);
        break;
      } else if (argc > 2) {
        ++(*flag_e);
        pat_from_str(patterns, *(argv + 2), arg_set);
      } else
        pat_from_str(patterns, empty, arg_set);
    } else if (argv[1][i] == 'f') {
      arg_set->arg_f = 1;
      if (argv[1][i + 1] != '\0') {
        pat_from_file(patterns, argv[1] + i + 1, arg_set);
        break;
      } else if (argc > 2) {
        ++(*flag_f);
        pat_from_file(patterns, *(argv + 2), arg_set);
      } else
        pat_from_str(patterns, empty, arg_set);
    } else if (argv[1][i] == 'i')
      arg_set->arg_i = 1;
    else if (argv[1][i] == 'v')
      arg_set->arg_v = 1;
    else if (argv[1][i] == 'c')
      arg_set->arg_c = 1;
    else if (argv[1][i] == 'l')
      arg_set->arg_l = 1;
    else if (argv[1][i] == 'n')
      arg_set->arg_n = 1;
    else if (argv[1][i] == 'h')
      arg_set->arg_h = 1;
    else if (argv[1][i] == 's')
      arg_set->arg_s = 1;
    else if (argv[1][i] == 'o')
      arg_set->arg_o = 1;
    else {
      fprintf(stderr, "s21_cat: invalid option -- %c\n", argv[1][i]);
      usage_message_exit();
    }
  }
}

/*
  Ф-я pat_from_str(...) вызывает realloc для массива строк patterns и
  записывает принятую строку в строку patterns[arg_set->arg_e].
*/
void pat_from_str(char ***patterns, char *string, args *arg_set) {
  realloc_str_arr(patterns, arg_set);
  strcpy((*patterns)[(arg_set->arg_e)++], string);
}

/*
  Ф-я pat_from_file(...):
- принимает имя файла с шаблонами;
- проверяет доступен ли файла с этим именем, иначе ошибка и выход из программы;
- построково считывает файл;
- после кажого успешного считывания строки вызывает realloc для patterns;
- если строка пуста, записывает шаблон для поиска произвольного кол-ва
  "пустых" символов (пробел, табуляция, перенос строки);
- если строка не пуста, посимвольно заполняет  patterns[arg_set->arg_e],
  исключая символы переноса строки и нуль-терминатор;
*/
void pat_from_file(char ***patterns, char *filename, args *arg_set) {
  FILE *pattern_file = fopen(filename, "r");
  if (pattern_file != NULL) {
    char file_string[MAX] = {'\0'};
    while (fgets(file_string, MAX, pattern_file) != NULL) {
      realloc_str_arr(patterns, arg_set);
      if (file_string[0] == '\n')
        strcpy((*patterns)[(arg_set->arg_e)++], " *");
      else {
        for (int i = 0; file_string[i] != '\n' && file_string[i] != '\0'; ++i)
          (*patterns)[arg_set->arg_e][i] = file_string[i];
        (arg_set->arg_e)++;
      }
    }
    fclose(pattern_file);
  } else {
    fprintf(stderr, "s21_grep: %s: No such file or directory\n", filename);
    exit(1);
  }
}

/*
  Ф-я pat2ex(...) компилирует регулярное выражение из полученной строки,
  в случае флага -i нечувствиельное к регистру.
*/
int pat2ex(char *pattern, regex_t *regex, args *arg_set) {
  return !regcomp(regex, pattern, arg_set->arg_i ? REG_ICASE : REG_NEWLINE);
}

/*
  Ф-я find_string(...) ищет входжение полученноого реш выражения
  в полученной строке и возращает !0 в случае успеха.
*/
int find_string(regex_t *regex, char *input_string) {
  return !regexec(regex, input_string, 0, NULL, 0);
}

/*
  Ф-я print_match(...) служит для печати всех вхождений рег выражения
  в полученную строку.

  https://www.gnu.org/software/libc/manual/html_node/Regexp-Subexpressions.html
  https://man7.org/linux/man-pages/man3/regcomp.3.html
*/
void print_match(regex_t *regex, char *input_string) {
  char *s = input_string;
  regmatch_t pmatch[1];
  regoff_t len;
  while ((regexec(regex, s, ARRAY_SIZE(pmatch), pmatch, 0)) == 0) {
    // regoff_t off = pmatch[0].rm_so + (s - input_string);
    len = pmatch[0].rm_eo - pmatch[0].rm_so;
    printf("%.*s\n", (int)len, s + pmatch[0].rm_so);
    s += pmatch[0].rm_eo;
  }
}

/*
  Ф-и realloc_str_arr(...) и free_str_arr(...)
  принимают адрес массива строк и служат для выделения, перевыделения
  и очищения памяти для этого массива.
  Являются обёртками ф-й realloc и free с целью декомпозиции.
*/
void realloc_str_arr(char ***str_arr, args *arg_set) {
  char **temp_arr = NULL;
  temp_arr = (char **)realloc(*str_arr, sizeof(char *) * (arg_set->arg_e + 1));
  temp_arr[arg_set->arg_e] = NULL;
  if (temp_arr != NULL) {
    *str_arr = temp_arr;

    if (((*str_arr)[arg_set->arg_e] = (char *)calloc(sizeof(char), MAX)) ==
        NULL) {
      fprintf(stderr, "Mem realloc failed for the %d string", arg_set->arg_e);
      exit(1);
    }
  } else {
    fprintf(stderr, "Mem realloc failed for the string arr with size %d",
            arg_set->arg_e);
    exit(1);
  }
}

void free_str_arr(char ***str_arr, const int size) {
  for (int i = 0; i < size; ++i) free((*str_arr)[i]);
  free(*str_arr);
}

/*
  Ф-я print_str(...) служит для печати принятой строки с одним и только одним
  переносом строки в конце.
*/
void print_str(char *str) {
  for (int i = 0, c = 32; (c = str[i++]) != '\0' && c != '\n';) putchar(c);
  putchar('\n');
}
