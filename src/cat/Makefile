CC = gcc
CFLAGS = -Wall -Werror -Wextra
all: s21_cat

s21_cat: s21_cat.o
	$(CC) $(CFLAGS) s21_cat.o -o s21_cat

s21_cat.o:
	$(CC) $(CFLAGS) -c s21_cat.c

test: rebuild
	sh test_func_cat.sh

leak: rebuild
	sh test_leak_cat.sh

clean:
	rm -rf *.o *.out s21_cat

rebuild: clean all
