#include <algorithm>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <utility>
#include <math.h>
#include <time.h>
using namespace std;

const int parallelChunkSize = 32;
#define mySize 9
#define number 10000
#define empty 30

clock_t startTime, endTime;

int matrix[number][mySize][mySize];
int outMatrix[number][mySize][mySize];
int inCol[number][mySize][mySize+1];
int inRow[number][mySize][mySize+1];
int inMat[number][mySize][mySize+1];

// assign value to an index
void assignValue(int idx, int i, int j, int curVal)
{	
	int curMat = ((int)(i/sqrt(mySize)))*sqrt(mySize) + j/sqrt(mySize);
	
	matrix[idx][i][j] = curVal;
	inCol[idx][i][curVal] = 1;
	inRow[idx][j][curVal] = 1;
	inMat[idx][curMat][curVal] = 1;
	
	return;
}

// remove value from an index
void removeValue(int idx, int i, int j, int curVal)
{
	int curMat = ((int)(i/sqrt(mySize)))*sqrt(mySize) + j/sqrt(mySize);
	
	matrix[idx][i][j] = 0;
	inCol[idx][i][curVal] = 0;
	inRow[idx][j][curVal] = 0;
	inMat[idx][curMat][curVal] = 0;
	
	return;
}

// assigning value function with backtracking
int solve(int idx, int i, int j)
{
	//if(i==1) cout<<i<<j<<endl;
	//if(i==2) cout<<i<<j<<endl;
	//if(i==mySize - 1) cout<<i<<j<<endl;
	
	vector<int> curPossible;
	curPossible.clear();
	int curMat = ((int)(i/sqrt(mySize)))*sqrt(mySize) + j/sqrt(mySize);

	// check possible values for current index
	for(int curVal=1; curVal<=mySize; curVal++) {
		if (!(inCol[idx][i][curVal] || inRow[idx][j][curVal] || inMat[idx][curMat][curVal])) {
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
	assignValue(idx, i, j, curVal);
	
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
	if(solve(idx, nextI, nextJ) == 1) {
		return 1;
	}
	
	// try all other solutions
	for(int tryID = (curID+1)%curSize; tryID != curID; tryID = (tryID+1)%curSize) {
		removeValue(idx, i, j, curVal);
		curVal = curPossible[tryID];
		assignValue(idx, i, j, curVal);
		// there is a solution with current value
		if(solve(idx, nextI, nextJ) == 1) {
			return 1;
		}
	}
	
	// no possible solution
	removeValue(idx, i, j, curVal);
	return -1;
}

int main()
{
	std::srand(std::time(nullptr));
	ofstream fout;
	fout.open("input.txt");
	
	startTime = clock();
	
	fout<<number<<endl;
	
	#pragma omp parallel for schedule(dynamic)
	for(int bigI = 0; bigI < number; bigI++) {
		
		// initialize
		vector<pair<int, int> > selectMat;
		selectMat.clear();
		for(int i=0; i<mySize; i++) {
			for(int j=0; j<mySize; j++) {
				matrix[bigI][i][j] = 0;
				outMatrix[bigI][i][j] = 0;
				inCol[bigI][i][j+1] = 0;
				inRow[bigI][i][j+1] = 0;
				inMat[bigI][i][j+1] = 0;
				selectMat.push_back(make_pair(i, j));
			}
		}
		
		// assign sudoku solution
		solve(bigI, 0, 0);
		
		// create empty space
		for(int i=0; i<empty; i++) {
			int curSize = selectMat.size();
			int curVal = random()%curSize;
			selectMat.erase(selectMat.begin()+curVal);
		}
		
		// add to matrix
		for(int i=0; i<selectMat.size(); i++) {
			outMatrix[bigI][selectMat[i].first][selectMat[i].second] = matrix[bigI][selectMat[i].first][selectMat[i].second];
		}
		
	}
	
	// print to file
	for(int idx = 0; idx < number; idx++){
		for(int i=0; i<mySize; i++) {
			for(int j=0; j<mySize; j++) {
				fout<<outMatrix[idx][i][j]<<" ";
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
