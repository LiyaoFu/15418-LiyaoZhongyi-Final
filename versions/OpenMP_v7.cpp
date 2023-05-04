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
#define IS_PRINT false

using namespace std;

clock_t startTime, endTime;

//vector<vector<vector<int> > > ans;

vector<vector<vector<int> > > matrix;

vector<vector<vector<int> > > inCol;
vector<vector<vector<int> > > inRow;
vector<vector<vector<int> > > inMat;

void print_grid(int grid[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            printf("%2d", grid[row][col]);
        }
        printf("\n");
    }
}

void print_vecgrid(vector<vector<int> > grid) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            printf("%2d", grid[row][col]);
        }
        printf("\n");
    }
}

int solve(int idx, int row, int col) {

	while (true) {
		if(matrix[idx][row][col] == 0) {
			break;
		}

		if (row == SIZE - 1 && col == SIZE - 1) {
	    	return 1;
		}
		
		col++;
		if (col == SIZE) {
			col = 0;
			row++;
		}

	}
	
	int curMat = ((int)(row/sqrt(SIZE)))*sqrt(SIZE) + col/sqrt(SIZE);
	
	omp_set_dynamic(0); //Explicitly turn off dynamic threads
	bool hasSol = false;
	
	#pragma omp parallel for schedule(static)
    for (int num = 1; num <= SIZE; num++ ) {
    	//std::cerr << __LINE__ << row<< " " << col << std::endl;
        if (!(inCol[idx][row][num] || inRow[idx][col][num] || inMat[idx][curMat][num])) {
            int val = 0;
            
            matrix[idx][row][col] = num;
			inCol[idx][row][num] = 1;
			inRow[idx][col][num] = 1;
			inMat[idx][curMat][num] = 1;
			
			//std::cerr << __LINE__ << row<< " " << col << std::endl;
            //val = solve(grid, inCol, inRow, inMat, idx, row, col, toSolve - 1);
            
            val = solve(idx, row, col);
            
            if(val == 1) {
            	/*
                vector<vector<int> > tmp;
                vector<int> tmp1;
                tmp.clear();
                for(int i = 0; i < SIZE; i++) {
                	tmp1.clear();
                	for(int j = 0; j < SIZE; j++) {
                		tmp1.push_back(grid[i][j]);
					}
					tmp.push_back(tmp1);
				}
				ans[idx] = tmp;*/
				hasSol = true;
                //return 2;
            } else if (val == 2) {
            	hasSol = true;
            	//return 2;
			} else {
	            matrix[idx][row][col] = UNASSIGNED;
	            inCol[idx][row][num] = 0;
				inRow[idx][col][num] = 0;
				inMat[idx][curMat][num] = 0;
			}
        }
    }
    
    if(hasSol) return 2;
    
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
	
	/*
	for(int idx = 0; idx < number; idx++) {
		vector<vector<int> > tmp;
		ans.push_back(tmp);
	}*/
	
    int tmp;
    
    int original[number][SIZE][SIZE];
    
    startTime = clock();
    
    //cout<<"hello"<<endl;
    
    for(int idx = 0; idx < number; idx++) {
    	int i = 0;
    	int j = 0;
    	vector<vector<int> > sudoku;
    	sudoku.clear();
		vector<int> tmpVec;
		tmpVec.clear();
	    while(fin>>tmp) {
	        tmpVec.push_back(tmp);
	        original[idx][i][j] = tmp;
	        j++;
	        if (j == SIZE) {
	        	j = 0;
	        	i++;
			}
			if (tmpVec.size() == SIZE) {
				sudoku.push_back(tmpVec);
				tmpVec.clear();
			}
			if (sudoku.size() == SIZE) {
				break;
			}
	    }
	    matrix.push_back(sudoku);
	}
    
    //std::cerr << matrix.size() << " " << matrix[1].size() << " " << matrix[1][1].size() <<__LINE__ << std::endl;
    
    vector<int> tmpVec(SIZE+1, 0);
    
    //#pragma omp parallel for
    for(int idx = 0; idx < number; idx++) {
    	vector<vector<int> > localInCol;
		vector<vector<int> > localInRow;
		vector<vector<int> > localInMat;
		
		localInCol.clear();
		localInRow.clear();
		localInMat.clear();
		
		for(int i=0; i<SIZE; i++) {
			localInCol.push_back(tmpVec);
			localInRow.push_back(tmpVec);
			localInMat.push_back(tmpVec);
		}
	
    	//std::cerr << __LINE__ << std::endl;
    	
    	/*
    	for(i=0; i<SIZE; i++) {
			for(j=0; j<SIZE; j++) {
				localInCol[i][j+1] = 0;
				localInRow[i][j+1] = 0;
				localInMat[i][j+1] = 0;
			}
		}
		*/
		
		for(int i = 0; i < SIZE; i++) {
	    	for(int j = 0; j < SIZE; j++) {
	    		
	    		//std::cerr << __LINE__ << " " << i << " " << j << std::endl;
	    		
	    		int curVal = matrix[idx][i][j];
	    		int curMat = ((int)(i/sqrt(SIZE)))*sqrt(SIZE) + j/sqrt(SIZE);
	    		
	    		/*if (curVal == 0) {
	    			toSolve ++;
				}*/
				
				//std::cerr << __LINE__ << std::endl;
				
				localInCol[i][curVal] = 1;
				
				//std::cerr << __LINE__ << std::endl;
				
				localInRow[j][curVal] = 1;
				
				//std::cerr << __LINE__ << std::endl;
				
				localInMat[curMat][curVal] = 1;
				
				//std::cerr << __LINE__ << std::endl;
			}
		}
		
		inCol.push_back(localInCol);
		inRow.push_back(localInRow);
		inMat.push_back(localInMat);
	}
    
    #pragma omp parallel for
    for(int idx = 0; idx < number; idx++) {
    	
    	//std::cerr << __LINE__ << std::endl;
    	
    	int i,j;
	    
	    //int toSolve = 0;
	    
	    //std::cerr << __LINE__ << std::endl;
	    
	    solve(idx, 0, 0);

		//std::cerr << __LINE__ << std::endl;

		/*for(i = 0; i < SIZE; i++) {
			for(j = 0; j < SIZE; j++) {
				//std::cerr << i <<" " << j << std::endl;
				sudoku[idx][i][j] = ans[idx][i][j];
			}
		}*/
		
		//std::cerr << __LINE__ << std::endl;
		
		//cout<<"hello"<<endl;
	}
	
	if (IS_PRINT) {
		printf("Size: %d", SIZE);   
	    printf("\n");
		for (int idx = 0; idx < number ; idx++) {
			cout<<"Sudoku ID:"<<idx<<endl;
			print_grid(original[idx]);
			printf("---------------------\n");
			//print_grid(sudoku[idx]);
			print_vecgrid(matrix[idx]);
			//if (idx != number - 1) {
		    printf("---------------------\n");
			//}
		}
		printf("\n");
	}
	
	endTime = clock();
	double time_spent = (double)(endTime - startTime) / CLOCKS_PER_SEC;  
	printf("Solved in %f s\n",time_spent); 
	
    //exit(EXIT_SUCCESS);

	return 0;
}
