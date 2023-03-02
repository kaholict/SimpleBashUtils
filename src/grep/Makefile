CC = gcc
CFLAGS = -Wall -Werror -Wextra
all: s21_grep

s21_grep: s21_grep.o
	$(CC) $(CFLAGS) s21_grep.o -o s21_grep

s21_grep.o:
	$(CC) $(CFLAGS) -c s21_grep.c

test: rebuild
	sh test_func_grep.sh

leak: rebuild
	sh test_leak_grep.sh

clean:
	rm -rf *.o *.out s21_grep

rebuild: clean all
