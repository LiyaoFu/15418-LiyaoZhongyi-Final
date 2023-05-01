
//https://stackoverflow.com/questions/68320530/openmp-sudoku-solver-parallelize-algorithm-with-openmp-and-or-mpi

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#define SIZE 9
#define UNASSIGNED 0

clock_t start, end;

void print_grid(int grid[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            printf("%2d", grid[row][col]);
        }
        printf("\n");
    }
}

//https://stackoverflow.com/questions/1726302/removing-spaces-from-a-string-in-c
void remove_spaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}

int is_exist_row(int grid[SIZE][SIZE], int row, int num){
    for (int col = 0; col < SIZE; col++) {
        if (grid[row][col] == num) {
            return 1;
        }
    }
    return 0;
}

int is_exist_col(int grid[SIZE][SIZE], int col, int num) {
    for (int row = 0; row < SIZE; row++) {
        if (grid[row][col] == num) {
            return 1;
        }
    }
    return 0;
}

int is_exist_box(int grid[SIZE][SIZE], int startRow, int startCol, int num) {
    for (int row = 0; row < sqrt(SIZE); row++) {
        for (int col = 0; col < sqrt(SIZE); col++) {
            if (grid[row + startRow][col + startCol] == num) {
                return 1;
            } 
        }
    }
    return 0;
}

int is_safe_num(int grid[SIZE][SIZE], int row, int col, int num) {
    return !is_exist_row(grid, row, num) 
            && !is_exist_col(grid, col, num) 
            && !is_exist_box(grid, row - (row % 3), col - (col %3), num);
}

int find_unassigned(int grid[SIZE][SIZE], int *row, int *col) {
    for (*row = 0; *row < SIZE; (*row)++) {
        for (*col = 0; *col < SIZE; (*col)++) {
            if (grid[*row][*col] == 0) {
                return 1;
            }
        }
    }
    return 0;
}


int solve(int grid[SIZE][SIZE]) {
    
    int row = 0;
    int col = 0;
    
    if (!find_unassigned(grid, &row, &col)){
        return 1;
    }
    
    for (int num = 1; num <= SIZE; num++ ) {        
        if (is_safe_num(grid, row, col, num)) {
            int val = 0;
            int copy_grid[SIZE][SIZE];
            for (int row = 0; row < SIZE; row++) {
                for (int col = 0; col < SIZE; col++) {                      
                    copy_grid[row][col] = grid[row][col];
                }                   
            }
            
            copy_grid[row][col] = num;              
            val = solve(copy_grid);
            
            if(val) {
                print_grid(copy_grid);
                end = clock();
                double time_spent = (double)(end - start) / CLOCKS_PER_SEC;  
                printf("\nSolved in %f s\n",time_spent);                    
                exit(0);                    
            }
            
            grid[row][col] = UNASSIGNED;
        }
    }
    
    return 0;
}

int main(int argc, char** argv) {
    
    int sudoku[SIZE][SIZE] = 
    {{0, 0, 0, 0, 0, 0, 0, 1, 0}, 
    {4, 0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 2, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 5, 0, 4, 0, 7},
    {0, 0, 8, 0, 0, 0, 3, 0, 0}, 
    {0, 0, 1, 0, 9, 0, 0, 0, 0}, 
    {3, 0, 0, 4, 0, 0, 2, 0, 0}, 
    {0, 5, 0, 1, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 8, 0, 6, 0, 0, 0} };
    
    int i,j;
    i = 0;
    j = 0;
    
    std::ifstream fin("input.txt");

    int tmp;
    
    while(fin>>tmp) {
        sudoku[i][j] = tmp;
        j++;
        if (j == SIZE) {
        	j = 0;
        	i++;
		}
    }
    
    printf("Size: %d", SIZE);   
    printf("\n");
    
    start = clock();
    printf("Solving Sudoku: \n");
    print_grid(sudoku);
    printf("---------------------\n");
    solve(sudoku);   
    exit(EXIT_SUCCESS);

}
