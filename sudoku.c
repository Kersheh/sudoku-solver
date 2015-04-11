/*
 * Author: Matthew Breckon
 *
 * A heuristic approach at solving sudoku puzzles.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SUDOKU_SIZE 9
#define empty(s) (!(s))

/* boolean ADT */
typedef enum bool_t {
	false = 0,
	true = 1
} Boolean;

int n[SUDOKU_SIZE][SUDOKU_SIZE]; //current working sudoku
int subSquares[SUDOKU_SIZE][SUDOKU_SIZE]; //subsquare within the sudoku stored based on cell(i, j) index
int row[SUDOKU_SIZE]; //current working cell(i, j) row
int col[SUDOKU_SIZE]; //current working cell(i, j) column
int current_i; //current working cell i (row) value
int current_j; //current working cell j (column) value

/* stack ADT */
typedef struct Stack {
	int n[SUDOKU_SIZE][SUDOKU_SIZE];
	int subSquares[SUDOKU_SIZE][SUDOKU_SIZE];
	int current_i, current_j;
	struct Stack *next;
} Stack;

/* stack push operation */
void push(Stack** top){
	Stack* new = malloc(sizeof(Stack)); //create a new node
	memcpy(new->n, n, sizeof(n)); //copy memory from current sudoku to stack
	memcpy(new->subSquares, subSquares, sizeof(subSquares)); //copy memory from current subSquares to stack
	new->current_i = current_i;
	new->current_j = current_j;
	new->next = *top;
	*top = new; //set up stack top
}

/* stack pop operation */
void pop(Stack** top){
	Stack* temp;
	if(empty(*top)) return;
	temp = *top; //save top node
	*top = temp->next; //set up stack top
	memcpy(n, temp->n, sizeof(n)); //copy memory from stack to current sudoku
	memcpy(subSquares, temp->subSquares, sizeof(subSquares)); //copy memory from stack to current subSquares
	current_i = temp->current_i;
	current_j = temp->current_j;
	//free(temp);
}

/* fill sudoku with blank template of 0's */
void blankSudoku(int sudoku[SUDOKU_SIZE][SUDOKU_SIZE]) {
	int i, j;
	for(i = 0; i < SUDOKU_SIZE; i++) {
		for(j = 0; j < SUDOKU_SIZE; j++) {
			sudoku[i][j] = 0;
		}
	}
}

/* print sudoku */
void printSudoku(int sudoku[SUDOKU_SIZE][SUDOKU_SIZE]) {
	int i, j;
	for(i = 0; i < SUDOKU_SIZE; i++) {
		if(i == 0) {
			printf("+-------+-------+-------+"); 
			printf("\n");
		}
		for(j = 0; j < SUDOKU_SIZE; j++) {
			if(j == 0) printf("| ");
			if(sudoku[i][j] == 0) printf("_ ");
			else printf("%d ", sudoku[i][j]);
			if((j + 1) % 3 == 0) printf("| "); 
		}
		printf("\n");
		if((i + 1) % 3 == 0) {
			printf("+-------+-------+-------+");  
			printf("\n");
		}
	}
}

/* returns index of subsquare cell(i, j) belongs to */
int getSquare(int i, int j) {
    return (i/3)*3 + j/3;
}

/* fills subSquares arrays */
void fillSubSquares() {
	int i, j;
	int position[SUDOKU_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	for(i = 0; i < SUDOKU_SIZE; i++) {
		for(j = 0; j < SUDOKU_SIZE; j++) {
			subSquares[getSquare(i, j)][position[getSquare(i, j)]] = n[i][j];
			position[getSquare(i, j)]++;
		}
	}
}

/* checks for unassigned values within the sudoku (0 value cells) and sets current cell to it */
Boolean setUnassignedCell() {
	for(current_i = 0; current_i < SUDOKU_SIZE; current_i++) {
		for(current_j = 0; current_j < SUDOKU_SIZE; current_j++) {
			if(n[current_i][current_j] == 0) return true; //if an unfilled cell is found, return true
		}
	}
	return false;
}

/* fills row array based on cell(i, j) */
void getRow(int cur_row) {
	int i;
	for(i = 0; i < SUDOKU_SIZE; i++) {
		row[i] = n[i][cur_row];
	}
}

/* fills column array based on cell(i, j) */
void getCol(int cur_col) {
	int i;
	for(i = 0; i < SUDOKU_SIZE; i++) {
		col[i] = n[cur_col][i];
	}
}

/* checks if passed value is already in row */
Boolean inRow(int num, int i) {
	getRow(i);
	int index;
	for(index = 0; index < SUDOKU_SIZE; index ++) {
		if(row[index] == num) return true;
	}
	return false;
}

/* checks if passed value is already in column */
Boolean inCol(int num, int j) {
	getCol(j);
	int index;
	for(index = 0; index < SUDOKU_SIZE; index ++) {
		if(col[index] == num) return true;
	}
	return false;
}

/* checks if passed value is already in square */
Boolean inSquare(int num, int i, int j) {
	int square = getSquare(i, j);
	int index;
	for(index = 0; index < SUDOKU_SIZE; index ++) {
		if(subSquares[square][index] == num) return true;
	}
	return false;
}

/* checks if passed value is safe within the current cell */
Boolean isSafe(int num, int i, int j) {
	if(!(inCol(num, i)) && !(inRow(num, j)) && !(inSquare(num, i, j))) return true;
	else return false;
}

/* if cell has only ONE possible number, return the number */
int solveSingleCell(int i, int j) {
	if(n[i][j] != 0) return n[i][j]; //return cell value if already solved

	Boolean possible_answer_found = false;
	int index, answer;
	for(index = 0; index < SUDOKU_SIZE; index++) {
		if(isSafe(index + 1, i, j)) {
			/* if possible answer already found, a single answer was NOT found */
			if(possible_answer_found == true) {
				return 0; //more than one answer possible
			}
			possible_answer_found = true;
			answer = index + 1;
		}
	}
	if(possible_answer_found == true) return answer;
	return 0; //no possible answers found
}

/* attempt to solve sudoku cell by cell, return false is no changes made */
Boolean solveCells() {
	fillSubSquares(); //initial setup for subsquares
	Boolean change_made = false;
	int i, j;
	for(i = 0; i < SUDOKU_SIZE; i++) {
		for(j = 0; j < SUDOKU_SIZE; j++) {
			if(n[i][j] == 0) {
				n[i][j] = solveSingleCell(i, j);
				if(n[i][j] != 0) {
					change_made = true;
					fillSubSquares(); //if change was made, reset subsquares
				}
			}
		}
	}
	return change_made;
}

/* recursively attempts to solve sudoku */
Boolean solveSudoku(Stack *stack) {
	while(solveCells()) {
		//printf("solveCells() success!\n");
		//printSudoku(n);
	}
	if(!setUnassignedCell()) {
		return true; //if no unassigned cells, sudoku is complete
	}
	push(&stack); //push current sudoku on stack
	int i;
	for(i = 1; i <= SUDOKU_SIZE; i++) {
		if(isSafe(i, current_i, current_j)) {
			n[current_i][current_j] = i; //assign value if safe
			fillSubSquares(); //reset subsquares array with new value
			if(solveSudoku(stack)) return true;
			pop(&stack); //failure, pop previous sudoku off stack
		}
	}
	return false;
}

/* parse input file for puzzle */
Boolean parsePuzzle(char *filename) {
	FILE *fp = fopen(filename, "r");
	if(fp == NULL) return false;
	int i, j, val;
	for(i = 0; i < SUDOKU_SIZE; i++) {
		for(j = 0; j < SUDOKU_SIZE; j++) {
			fscanf(fp, "%d", &val);
			n[i][j] = val;
		}
	}
	fclose(fp);
	return true;
}

int main(int argc, char *argv[]) {
	if(argc != 2) {
		printf("error: no file passed\nusage: ./sudoku puzzle.txt\n");
		return 0;
	}

	blankSudoku(n); //fill current sudoku template with 0's before parse
	if(!(parsePuzzle(argv[1]))) {
		printf("parse error: %s is not a valid puzzle file\n", argv[1]);
		return 0;
	}

	Stack *stack;

	/* timer */
	int ms;
	clock_t diff, start;

	printf("Original sudoku:\n");
	printSudoku(n);
	
	start = clock();
	if(solveSudoku(stack)) {
		printf("Completed sudoku:\n");
		printSudoku(n);
	}
	else printf("Sudoku not possible to solve.\n");

	diff = clock() - start;
	ms = diff * 1000 / CLOCKS_PER_SEC;
	printf("Runtime: %dms // %0.2fs\n", ms, (float)ms/1000);
	return 0;
}