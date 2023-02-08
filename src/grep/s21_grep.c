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
  Ф-я main(...) обеспечивает:
- объявление главных переменных;
- вызов ф-ии по их наполнению на основе командной строки;
- цикл вызовы логики поиска для каждого из заданных файлов.

  В main(...) обяъвляются переменные:
- структура args arg_set для хранения значения и количества флагов;
- структура regex_t regex для компиляции регулярных выражений;
- массив указателей filenames для хранения указателей на строки с именами
  файлов, полученных из командной строки;
- массив строк patterns для хранения шаблонов, полученных из командной строки
  или файлов,
  кол-во шаблонов не превышает (кол-во аргументов) / 2.

  Ф-я check_cmd_line(...) начинает работу с аргументами командной строки.
  Результатом работы каскада функций является нахождение и запись
  в соответствующие переменные аргументов, шаблов и имён файлов.

  Для каждого из имён файлов вызывается ф-я processor(...), в которой будет
  совершаться основная логика поиска входжений шаблонов в строки.
*/
int main(int argc, char **argv) {
  args arg_set = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  regex_t regex;
  char *filenames[argc];
  char **patterns = NULL;

  check_cmd_line(argc, argv, &arg_set, &patterns, filenames);
  for (int i = 0; (i < arg_set.file_args) && arg_set.arg_e; ++i)
    processor(filenames[i], patterns, &arg_set, &regex);

  free_str_arr(&patterns, arg_set.arg_e);
  return 0;
}

/*
  Ф-я processor(...) обеспечивает:
- открытие и построчное чтение текстового файла
  или сигнал об ошибке при попытке открыть файл;
- передачу строк с шаблонами на компиляцию регулярных выражений;
- поиск в каждой строке вхождения хотя бы одного из шаблонов;
- работу со счётчиками в зависимости от успеха поиска и параметров,
  заданных флагами;
- построчную или пофайловую передачу на вывод результат на экран.

  В processor(...) обяъвляются переменные:
- строка input_string для хранения строки, полученной из файла;
- целочисленный str_count для подсчёта строк в файле;
- целочисленный mtched_str для подсчсёта строк, удовлетворяющих
  заданным шаблонам и условиям;
- целочесленный match в качестве флага успешного поиска шаблона в строке;
- указатель на структуру FILE input_file работы с файлом,
  имя которого filename передано из main(...).

  Если файл найден и может быть прочитан, то начинаем работу,
  иначе (если флагом не задано обратное) сообщаем об ошибке с файлом.

  В цикле считываем файл построчно, пока не дойдём до EOF, обнуляем match и
  увеличиваем str_count для каждой новой строки.
  Каждую строку из массива с шаблонами patterns компилируем в рег выражение
  в ф-ии pat2ex(...) и ищем в текущей строке ф-ей find_string(...) до тех пор,
  пока не надём вхождение очередного шаблоны или пока не кончатся шаблоны.

  Если вхождение было найдено, то match станет == 1, а счётчик удовлетворяющих
  строк будет пополнен на 1, если задан -v то наоборот, счётчик таких строк
  будет пополнен, если вхождения не было.

  Результаты предудщих шагов передаются в ф-ю printer(...) построчно или,
  если заданы флаги -c и/или -l, пофайлово.
*/
void processor(char *filename, char **patterns, args *arg_set, regex_t *regex) {
  char input_string[MAX] = {'\0'};
  int str_count = 0;
  int mtched_str = 0;
  int match = 0;
  int regcomp_succ = 0;
  FILE *input_file = fopen(filename, "r");

  if (input_file != NULL) {
    while (fgets(input_string, MAX, input_file) != NULL) {
      match = 0;
      str_count++;
      for (int i = 0; i < arg_set->arg_e && !match; ++i) {
        if ((regcomp_succ = pat2ex(patterns[i], regex, arg_set)))
          if (!(match = find_string(regex, input_string))) regfree(regex);
      }
      if (match && !arg_set->arg_v)
        mtched_str++;
      else if (!match && arg_set->arg_v)
        mtched_str++;

      if (!(arg_set->arg_c || arg_set->arg_l))
        printer(arg_set, match, filename, input_string, str_count, mtched_str,
                regex, patterns);
      if (regcomp_succ) regfree(regex);
    }
    if (arg_set->arg_c) {
      if (arg_set->arg_l && mtched_str > 0)
        printer(arg_set, 2, filename, NULL, str_count, 1, NULL, NULL);
      else
        printer(arg_set, 2, filename, NULL, str_count, mtched_str, NULL, NULL);
    }
    if (arg_set->arg_l && mtched_str) print_str(filename);

    fclose(input_file);
  } else if (!arg_set->arg_s) {
    fprintf(stderr, "s21_grep: %s: No such file or directory\n", filename);
  }
}

/*
  Ф-я printer(...) обеспечивает вывод результатов работы предыдущих шагов
  на экран, формат вывода определяется аргументами командной строки.

  В данной реалищации условие (match - arg_set->arg_v) опеределяет,
  подлежит ли строка с/без вхождeния печати в зависимости от инвертированного
  смысла (-v)

- если не -h и файлов больше одного, то печатаем имя файла;
  Построчная логика (если не -с и не -l):
- если -n то печатаем номер строки строки;
- если не -o, то печатаем строку из файла в зависимости от match и -v;
- если -o и match - печатаем только совпадающие с шаблоном части строки,
  находя и печатая каждую из них с помощью ф-ии print_match(...);
- если -o и !match - печатаем строку из файла;
  Пофайловая логика:
- печатаем результат работы, принятый из processor(...).
*/
void printer(args *arg_set, int match, char *filename, char *input_string,
             int str_count, int mtched_str, regex_t *regex, char **patterns) {
  if (arg_set->file_args > 1 && !arg_set->arg_h && (match - arg_set->arg_v))
    printf("%s:", filename);

  if (!arg_set->arg_c) {
    if (arg_set->arg_n && (match - arg_set->arg_v)) printf("%d:", str_count);

    if (!arg_set->arg_o) {
      if (match - arg_set->arg_v)
        print_str(input_string);  // printf("%s", input_string);
    } else if (!match && arg_set->arg_v)
      print_str(input_string);  // printf("%s", input_string);
    else if (match && !arg_set->arg_v) {
      regfree(regex);
      for (int i = 0; i < arg_set->arg_e; ++i) {
        if (pat2ex(patterns[i], regex, arg_set)) {
          print_match(regex, input_string);
          regfree(regex);
        }
      }
    }

  } else
    printf("%d\n", mtched_str);
}
