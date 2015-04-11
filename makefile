CC = gcc

all: sudoku

sudoku: sudoku.c
	$(CC) -o sudoku sudoku.c

clean:
	$(RM) *.exe