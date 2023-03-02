#ifndef SRC_CAT_CAT_H_
#define SRC_CAT_CAT_H_
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} opt;

static struct option const long_options[] = {
    {"number-nonblank", 0, 0, 'b'},  {"number", 0, 0, 'n'},
    {"squeeze-blank", 0, 0, 's'},    {"show-ends", 0, 0, 'E'},
    {"show-nonprinting", 0, 0, 'v'}, {NULL, 0, NULL, 0}};

void s21_cat(int argc, char *argv[]);
int file_check(int argc, char *argv[], int ind);
void parser(int argc, char *argv[], opt *options);
int is_file(int argc, char *argv[], int ind);
// int is_opts(opt* options);
void reader(opt *options, FILE *file_name);
void option_s(int *s_flag, char cur, char prev, char preprev, opt *options,
              int str_flag, int str_flag_0, int str_flag_1);
void option_e(int *s_flag /*, char prev*/, char cur,
              opt *options /*, int str_count*/);
void option_v(int *tv_flag, char cur, opt *options);
void option_tt(int *t_flag, char cur, opt *options);
void option_b(int *s_flag, char cur, opt *options, int *counter,
              int *str_count);
void option_n(int *s_flag, char cur, char prev, opt *options, int *counter,
              int *str_count);

#endif  // SRC_CAT_CAT_H_
