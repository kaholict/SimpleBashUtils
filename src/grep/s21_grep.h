#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_

#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int regex_flag;
  bool flagE;
  bool memoryError;
  bool invert;
  bool count;
  bool filesMatch;
  bool numberLine;
  bool printMatched;
  bool hideFile;
  bool fromFile;
} Flags;

Flags GrepReadFlags(int argc, char *argv[], char ***templates,
                    int *count_templates, char **f_file_name);
void print_error();
void print_str(int sameFiles, Flags flags, int current_file_position,
               int *printOnce, char *line, char *argv[], int *printed,
               int str_count);
void print_invert_o(int count_match, int str_count, int *printOnce,
                    int *printOnceFile, int sameFiles,
                    int current_file_position, char *line, int *printed,
                    Flags flags, char *argv[]);
int flag_e(char *line, char **templates, int *count_templates, Flags flags,
           int sameFiles, char *argv[], int current_file_position,
           int str_count);
void print_o(int *printed, int sameFiles, int *printOnceFile, char *argv[],
             int current_file_position, Flags flags);
int flag_o(char *line, char **templates, int *count_templates, Flags flags,
           int sameFiles, char *argv[], int current_file_position,
           int str_count);
int flag_c(char *line, char **templates, int *count_templates, Flags flags);
Flags new_templates(char *argv[], char ***templates, int *count_templates,
                    Flags flags);
// int reader(FILE *file_name);
void func_to_getline(char *argv[], char **templates, Flags *flags,
                     size_t *length, int current_file_position, int sameFiles,
                     FILE *file, int *count_templates, int *printLastStr,
                     char **line, int *str_count, int *count);
void func_to_read_file(char *argv[], char **templates, size_t *length,
                       int current_file_position, int sameFiles, FILE *file,
                       int *count_templates, Flags *flags, int *printLastStr);
int func_to_comb_files(int argc, char *argv[], char **templates,
                       int *count_templates, Flags *flags, size_t *length,
                       int current_file_position, int sameFiles);
void file_templates(char *line, char ***templates, int *count_templates,
                    Flags *flags);
void Grep(int argc, char *argv[]);

#endif  // SRC_GREP_S21_GREP_H_
