#include <algorithm>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <utility>
#include <math.h>
using namespace std;

#define mySize 9
#define number 2000
#define empty 30

clock_t startTime, endTime;

int matrix[mySize][mySize];
int outMatrix[mySize][mySize];

vector<pair<int, int> > selectMat;

// in column
bool inCol(int col, int curVal) {
	for(int i=0; i<mySize; i++) {
		if(matrix[col][i] == curVal) {
			return true;
		}
	}
	return false;
}

// in row
bool inRow(int row, int curVal) {
	for(int i=0; i<mySize; i++) {
		if(matrix[i][row] == curVal) {
			return true;
		}
	}
	return false;
}

// in matrix
bool inMat(int mat, int curVal) {
	int iMin = ((int)mat/(sqrt(mySize))) * (sqrt(mySize));
	int jMin = ((int)mat%((int)(sqrt(mySize)))) * (sqrt(mySize));
	for(int i = iMin; i < iMin + (sqrt(mySize)); i++) {
		for(int j = jMin; j < jMin + (sqrt(mySize)); j++) {
			if (matrix[i][j] == curVal) {
				return true;
			}
		}
	}
	return false;
}

// assigning value function with backtracking
int solve(int i, int j)
{
	//if(i==1) cout<<i<<j<<endl;
	//if(i==2) cout<<i<<j<<endl;
	//if(i==mySize - 1) cout<<i<<j<<endl;
	
	vector<int> curPossible;
	curPossible.clear();
	int curMat = ((int)(i/sqrt(mySize)))*sqrt(mySize) + j/sqrt(mySize);

	// check possible values for current index
	for(int curVal=1; curVal<=mySize; curVal++) {
		if (!(inCol(i, curVal) || inRow(j, curVal) || inMat(curMat, curVal))) {
			curPossible.push_back(curVal);
		}
	}
	
	// no choice, impossible
	if(curPossible.size() == 0) {
		return -1;
	}
	
	int curSize = curPossible.size();
	int curID = rand()%curSize;
	int curVal = curPossible[curID];
	
	// assign the value to index
	matrix[i][j] = curVal;
	
	int nextI = i, nextJ = j;
	
	// finished
	if (nextI == mySize-1 && nextJ == mySize-1) {
		return 1;
	}
	
	// move to next index
	nextJ++;
	if(nextJ == mySize) {
		nextJ = 0;
		nextI++;
	}
	
	// there is a solution with current value
	if(solve(nextI, nextJ) == 1) {
		return 1;
	}
	
	// try all other solutions
	for(int tryID = (curID+1)%curSize; tryID != curID; tryID = (tryID+1)%curSize) {
		curVal = curPossible[tryID];
		matrix[i][j] = curVal;
		// there is a solution with current value
		if(solve(nextI, nextJ) == 1) {
			return 1;
		}
	}
	
	// no possible solution
	matrix[i][j] = 0;
	return -1;
}

int main()
{
	std::srand(std::time(nullptr));
	ofstream fout;
	fout.open("input.txt");
	
	startTime = clock();
	
	fout<<number<<endl;
	
	for(int bigI = 0; bigI < number; bigI++) {
		
		// initialize
		selectMat.clear();
		for(int i=0; i<mySize; i++) {
			for(int j=0; j<mySize; j++) {
				matrix[i][j] = 0;
				outMatrix[i][j] = 0;
				selectMat.push_back(make_pair(i, j));
			}
		}
		
		// assign sudoku solution
		solve(0, 0);
		
		// create empty space
		for(int i=0; i<empty; i++) {
			int curSize = selectMat.size();
			int curVal = random()%curSize;
			selectMat.erase(selectMat.begin()+curVal);
		}
		
		// add to matrix
		for(int i=0; i<selectMat.size(); i++) {
			outMatrix[selectMat[i].first][selectMat[i].second] = matrix[selectMat[i].first][selectMat[i].second];
		}
		
		// print to file
		for(int i=0; i<mySize; i++) {
			for(int j=0; j<mySize; j++) {
				fout<<outMatrix[i][j]<<" ";
			}
			fout<<endl;
		}
	}
	
	endTime = clock();
	double time_spent = (double)(endTime - startTime) / CLOCKS_PER_SEC;  
	cout<<number<<" Sudokus generated in "<<time_spent<<" s"<<endl;      
	
	fout.close();
	
	return 0;
}
