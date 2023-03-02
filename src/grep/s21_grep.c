#define _GNU_SOURCE
#include "s21_grep.h"

Flags GrepReadFlags(int argc, char *argv[], char ***templates,
                    int *count_templates, char **f_file_name) {
  Flags flags = {0,     false, false, false, false,
                 false, false, false, false, false};
  char *opt_str = "e:ivclnof:sh";
  int opt = 0;
  int option_index = 0;
  struct option long_options = {0};
  while ((opt = getopt_long(argc, argv, opt_str, &long_options,
                            &option_index)) != -1) {
    switch (opt) {
      case 'e':
        flags.flagE = true;
        flags.regex_flag |= REG_EXTENDED;
        (*templates) = (char **)realloc(
            (*templates), ((*count_templates) + 1) * sizeof(char *));
        if ((*templates) != NULL) {
          int len = strlen(optarg);
          (*templates)[(*count_templates)] = calloc(len + 1, sizeof(char));
          strcpy((*templates)[(*count_templates)], optarg);
        } else {
          flags.memoryError = true;
        }
        (*count_templates)++;
        break;
      case 'i':
        flags.regex_flag |= REG_ICASE;
        break;
      case 'v':
        flags.invert = true;
        break;
      case 'c':
        flags.count = true;
        break;
      case 'l':
        flags.filesMatch = true;
        break;
      case 'n':
        flags.numberLine = true;
        break;
      case 'o':
        flags.printMatched = true;
        break;
      case 'h':
        flags.hideFile = true;
        break;
      case 'f':
        flags.fromFile = true;
        int len = strlen(optarg);
        *f_file_name = calloc(len + 1, sizeof(char));
        strcpy(*f_file_name, optarg);
        break;
      default:
        print_error();
        break;
    }
  }
  return flags;
}

void print_error() {
  fprintf(stderr,
          "usage: grep [-abcDEFGHhiiJLlmnOoqRSsUVvwxZ] [-A num] [-B num] "
          "[-C[num]]\n"
          "\t[-e pattern] [-f file] [--binary-files=value] [--color=when]\n"
          "\t[--context[=num]] [--directories=action] [--label] "
          "[--line-buffered]\n"
          "\t[--null] [pattern] [file ...]\n");
}

void print_str(int sameFiles, Flags flags, int current_file_position,
               int *printOnce, char *line, char *argv[], int *printed,
               int str_count) {
  if (sameFiles && !flags.hideFile) printf("%s:", argv[current_file_position]);
  if (flags.numberLine && *printOnce == 1) printf("%d:", str_count);
  printf("%s", line);
  *printed = 1;
  *printOnce = 0;
}

void print_invert_o(int count_match, int str_count, int *printOnce,
                    int *printOnceFile, int sameFiles,
                    int current_file_position, char *line, int *printed,
                    Flags flags, char *argv[]) {
  if (flags.invert && count_match != 0 && *printOnce == 1) {
    if (sameFiles && *printOnceFile == 1 && !flags.hideFile)
      printf("%s:", argv[current_file_position]);
    *printOnceFile = 0;  // чтобы не принтить имя файла 2 раза для 1 строки
    if (flags.numberLine && *printOnce == 1) printf("%d:", str_count);
    printf("%s", line);
    *printOnce = 0;
    *printed = 1;
  }
}

int flag_e(char *line, char **templates, int *count_templates, Flags flags,
           int sameFiles, char *argv[], int current_file_position,
           int str_count) {
  regex_t regex = {0};
  regmatch_t match;
  int printed = 0;
  int i = 0;
  int printOnce = 1;
  int count_match = 0;
  int gal_flag = 0;
  for (int k = 0; k < *count_templates; k++)
    if (templates[k][0] == '^') gal_flag = 1;
  while (i < *count_templates) {
    regcomp(&regex, templates[i], flags.regex_flag);
    if (flags.invert) {
      if (regexec(&regex, line, 1, &match, 0) && printOnce == 1) {
        count_match++;
        if (templates[i][0] == '^' && templates[i][1] == line[0]) {
          int counter = 0;
          int j = 1;
          while (line[j] == templates[i][0]) {
            j++;
            counter++;
          }
          int temp_len = strlen(templates[i]) - 1;
          if (counter != temp_len && count_match == *count_templates)
            print_str(sameFiles, flags, current_file_position, &printOnce, line,
                      argv, &printed, str_count);
        } else if (gal_flag == 0 || count_match == *count_templates)
          print_str(sameFiles, flags, current_file_position, &printOnce, line,
                    argv, &printed, str_count);
      }
    } else if (!regexec(&regex, line, 1, &match, 0) && printOnce == 1)
      print_str(sameFiles, flags, current_file_position, &printOnce, line, argv,
                &printed, str_count);
    i++;
    regfree(&regex);
  }
  return printed;
}

void print_o(int *printed, int sameFiles, int *printOnceFile, char *argv[],
             int current_file_position, Flags flags) {
  *printed = 1;
  if (sameFiles && *printOnceFile == 1 && !flags.hideFile) {
    printf("%s:", argv[current_file_position]);
    *printOnceFile = 0;  // чтобы не принтить имя файла 2 раза для 1 строки
  }
}

int flag_o(char *line, char **templates, int *count_templates, Flags flags,
           int sameFiles, char *argv[], int current_file_position,
           int str_count) {
  regex_t regex = {0};
  regmatch_t match[1] = {0};
  int i = 0;
  int printed = 0;
  int printOnceFile = 1;
  int printOnce = 1;
  int count_match = 0;
  while (i < *count_templates) {
    regcomp(&regex, templates[i], flags.regex_flag);
    if (flags.invert) {
      if (regexec(&regex, line, 1, match, 0) && printOnce == 1)
        count_match++;
      else
        printOnce = 0;
    } else {
      while (!regexec(&regex, line, 1, match, 0)) {
        print_o(&printed, sameFiles, &printOnceFile, argv,
                current_file_position, flags);
        if (flags.numberLine && printOnce == 1) {
          printf("%d:", str_count);
#ifdef __APPLE__
          if (*count_templates == 1) printOnce = 0;
#endif
        }
        for (int j = match[0].rm_so; j < match[0].rm_eo; j++)
          printf("%c", line[j]);
        printf("\n");
#ifdef __APPLE__
        printOnce = 0;  // ЕСЛИ ЧТО СНОСИТЬ ЭТО
#endif
        line += match[0].rm_eo;
      }
    }
    i++;
    regfree(&regex);
  }
  print_invert_o(count_match, str_count, &printOnce, &printOnceFile, sameFiles,
                 current_file_position, line, &printed, flags, argv);
  return printed;
}

int flag_c(char *line, char **templates, int *count_templates, Flags flags) {
  regex_t regex = {0};
  regmatch_t match;
  int i = 0;
  int count = 0;
  while (i < *count_templates) {
    regcomp(&regex, templates[i], flags.regex_flag);
    if (!regexec(&regex, line, 1, &match, 0) && line[0] != '\n') count = 1;
    i++;
    regfree(&regex);
  }
  return count;
}

Flags new_templates(char *argv[], char ***templates, int *count_templates,
                    Flags flags) {
  flags.regex_flag |= REG_EXTENDED;
  (*templates) =
      (char **)realloc((*templates), ((*count_templates) + 1) * sizeof(char *));
  if ((*templates) != NULL) {
    int len = strlen(argv[optind]);
    (*templates)[(*count_templates)] = calloc(len + 1, sizeof(char));
    strcpy((*templates)[(*count_templates)], argv[optind]);
  } else
    flags.memoryError = true;
  (*count_templates)++;
  return flags;
}

// int reader(FILE *file_name) {
//   char cur = ' ';   // текущий символ
//   char prev = ' ';  // предыдущий символ
//   int result = 0;
//   while ((cur = fgetc(file_name)) != EOF) prev = cur;
//   if (prev != '\n') result = 1;
//   return result;
// }

void func_to_getline(char *argv[], char **templates, Flags *flags,
                     size_t *length, int current_file_position, int sameFiles,
                     FILE *file, int *count_templates, int *printLastStr,
                     char **line, int *str_count, int *count) {
  while (getline(line, length, file) > 0) {
    (*printLastStr) = 0;
    (*str_count)++;
    (*count) += flag_c((*line), templates, count_templates, (*flags));
    if (!(*flags).count && !(*flags).filesMatch) {
      if ((*flags).printMatched) {
        (*printLastStr) =
            flag_o((*line), templates, count_templates, (*flags), sameFiles,
                   argv, current_file_position, (*str_count));
      } else
        (*printLastStr) =
            flag_e((*line), templates, count_templates, (*flags), sameFiles,
                   argv, current_file_position, (*str_count));
    }
  }
}

void func_to_read_file(char *argv[], char **templates, size_t *length,
                       int current_file_position, int sameFiles, FILE *file,
                       int *count_templates, Flags *flags, int *printLastStr) {
  if (file) {
    char *line = 0;
    int str_count = 0;
    int count = 0;
    func_to_getline(argv, templates, flags, length, current_file_position,
                    sameFiles, file, count_templates, printLastStr, &line,
                    &str_count, &count);
    free(line);
    line = 0;
    int count_result = 0;
    if ((*flags).invert)
      count_result = str_count - count;
    else
      count_result = count;
    if ((*flags).count) {
      if ((*flags).filesMatch && count_result != 0) count_result = 1;
      if (sameFiles && !(*flags).hideFile)
        printf("%s:", argv[current_file_position]);
      printf("%d\n", count_result);
    }
    if ((*flags).filesMatch && count_result != 0)
      printf("%s\n", argv[current_file_position]);
  }
}

int func_to_comb_files(int argc, char *argv[], char **templates,
                       int *count_templates, Flags *flags, size_t *length,
                       int current_file_position, int sameFiles) {
  while (current_file_position != argc) {
    int printLastStr = 0;
    FILE *file = NULL;
    if ((file = fopen(argv[current_file_position], "r"))) {
      func_to_read_file(argv, templates, length, current_file_position,
                        sameFiles, file, count_templates, flags, &printLastStr);
      char cur = ' ';   // текущий символ
      char prev = ' ';  // предыдущий символ
      int result = 0;
      fclose(file);
      file = fopen(argv[current_file_position], "r");
      while ((cur = fgetc(file)) != EOF) prev = cur;
      if (prev != '\n') result = 1;
      if ((result == 1 && printLastStr == 1) &&
          !(*flags).filesMatch /*&& !flags.printMatched */ && !(*flags).count &&
          !(*flags).flagE)
        printf("\n");
      fclose(file);
    } else if (current_file_position != optind ||
               current_file_position == argc - 1)
      fprintf(stderr, "grep: %s: No such file or directory\n",
              argv[current_file_position]);
    current_file_position++;
  }
  return (*count_templates);
}

void file_templates(char *line, char ***templates, int *count_templates,
                    Flags *flags) {
  (*templates) =
      (char **)realloc((*templates), ((*count_templates) + 1) * sizeof(char *));
  if ((*templates) != NULL) {
    int len = strlen(line);
    (*templates)[(*count_templates)] = calloc(len + 1, sizeof(char));
    strcpy((*templates)[(*count_templates)], line);
  } else
    flags->memoryError = true;
  (*count_templates)++;
}

void Grep(int argc, char *argv[]) {
  char **templates = NULL;
  int count_templates = 0;
  char *f_file_name = NULL;
  Flags flags =
      GrepReadFlags(argc, argv, &templates, &count_templates, &f_file_name);
  if (argc == 1)
    print_error();
  else if (flags.memoryError)
    fprintf(stderr, "memory error\n");
  else if (argc != 2) {
    size_t length = 0;
    int current_file_position = optind;
    int sameFiles = 1;
    if (flags.fromFile) {
      FILE *f_file;
      if ((f_file = fopen(f_file_name, "r"))) {
        char *f_line = 0;
        size_t f_length = 0;
        while (getline(&f_line, &f_length, f_file) > 0) {
          flags.regex_flag |= REG_EXTENDED;
          file_templates(f_line, &templates, &count_templates, &flags);
        }
        free(f_line);
        f_line = 0;
        fclose(f_file);
      } else
        fprintf(stderr, "grep: %s: No such file or directory\n", f_file_name);
      if (f_file_name != NULL) free(f_file_name);
    } else if (!flags.flagE) {
      flags = new_templates(argv, &templates, &count_templates, flags);
      current_file_position++;
    }
    if (current_file_position == argc - 1) sameFiles = 0;
    count_templates =
        func_to_comb_files(argc, argv, templates, &count_templates, &flags,
                           &length, current_file_position, sameFiles);
    for (int j = 0; j < count_templates; j++) {
      free(templates[j]);
      templates[j] = 0;
    }
    free(templates);
    templates = 0;
  }
}

int main(int argc, char *argv[]) {
  Grep(argc, argv);
  return 0;
}
