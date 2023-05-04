
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
#include <iostream>

#define SIZE 9
#define UNASSIGNED 0
#define parallelTaskThreshold SIZE*5

using namespace std;

clock_t startTime, endTime;

vector<vector<vector<int> > > ans;

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

int is_exist_box(int grid[SIZE][SIZE], int startTimeRow, int startTimeCol, int num) {
    for (int row = 0; row < sqrt(SIZE); row++) {
        for (int col = 0; col < sqrt(SIZE); col++) {
            if (grid[row + startTimeRow][col + startTimeCol] == num) {
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


int solve(int grid[SIZE][SIZE], int idx, int toSolve) {
    
    int row = 0;
    int col = 0;
    
    if (!find_unassigned(grid, &row, &col)){
        return 1;
    }
    
    //#pragma omp task final (toSolve < parallelTaskThreshold)
    //{
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
	            val = solve(copy_grid, idx, toSolve - 1);
	            
	            if(val == 1) {
	                //print_grid(copy_grid);
	                vector<vector<int> > tmp;
	                vector<int> tmp1;
	                tmp.clear();
	                for(int i = 0; i < SIZE; i++) {
	                	tmp1.clear();
	                	for(int j = 0; j < SIZE; j++) {
	                		tmp1.push_back(copy_grid[i][j]);
						}
						tmp.push_back(tmp1);
					}
					ans[idx] = tmp;
	                //endTime = clock();
	                //double time_spent = (double)(endTime - startTime) / CLOCKS_PER_SEC;  
	                //printf("\nSolved in %f s\n",time_spent);                    
	                //exit(0);
	                return 2;
	            } else if (val == 2) {
	            	return 2;
				}
	            
	            grid[row][col] = UNASSIGNED;
	        }
	    }
	//}
    
    return 0;
}

int main(int argc, char** argv) {
    
    /*int sudoku[SIZE][SIZE] = 
    {{0, 0, 0, 0, 0, 0, 0, 1, 0}, 
    {4, 0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 2, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 5, 0, 4, 0, 7},
    {0, 0, 8, 0, 0, 0, 3, 0, 0}, 
    {0, 0, 1, 0, 9, 0, 0, 0, 0}, 
    {3, 0, 0, 4, 0, 0, 2, 0, 0}, 
    {0, 5, 0, 1, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 8, 0, 6, 0, 0, 0} };*/
    
    //std::cerr << __LINE__ << std::endl;
    
    std::ifstream fin("input.txt");
	int number;
	fin>>number;
	
	for(int idx = 0; idx < number; idx++) {
		vector<vector<int> > tmp;
		ans.push_back(tmp);
	}
	
    int tmp;
    
    int sudoku[number][SIZE][SIZE];
    int original[number][SIZE][SIZE];
    
    startTime = clock();
    
    //cout<<"hello"<<endl;
    
    for(int idx = 0; idx < number; idx++) {
    	int i = 0;
    	int j = 0;
	    while(fin>>tmp) {
	        sudoku[idx][i][j] = tmp;
	        original[idx][i][j] = tmp;
	        j++;
	        if (j == SIZE) {
	        	j = 0;
	        	i++;
			}
	    }
	}
    
    #pragma omp parallel for
    for(int idx = 0; idx < number; idx++) {
    	
    	//std::cerr << idx << std::endl;
    	
    	int i,j;
	    
	    int toSolve = 0;
	    for(i = 0; i < SIZE; i++) {
	    	for(j = 0; j < SIZE; j++) {
	    		if (sudoku[idx][i][j] == 0) {
	    			toSolve ++;
				}
			}
		}
	    
	    solve(sudoku[idx], idx, toSolve);

		for(i = 0; i < SIZE; i++) {
			for(j = 0; j < SIZE; j++) {
				sudoku[idx][i][j] = ans[idx][i][j];
			}
		}
		
		//cout<<"hello"<<endl;
	}
	printf("Size: %d", SIZE);   
    printf("\n");
	for (int idx = 0; idx < number ; idx++) {
		cout<<"Sudoku ID:"<<idx<<endl;
		print_grid(original[idx]);
		printf("---------------------\n");
		print_grid(sudoku[idx]);
		//if (idx != number - 1) {
	    printf("---------------------\n");
		//}
	}
	
	endTime = clock();
	double time_spent = (double)(endTime - startTime) / CLOCKS_PER_SEC;  
	printf("\nSolved in %f s\n",time_spent); 
	
    //exit(EXIT_SUCCESS);

	return 0;
}
