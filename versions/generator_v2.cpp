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
#define number 1
#define empty 60

int matrix[mySize][mySize];
int outMatrix[mySize][mySize];
int inCol[mySize][mySize+1];
int inRow[mySize][mySize+1];
int inMat[mySize][mySize+1];
vector<pair<int, int> > selectMat;

// assign value to an index
void assignValue(int i, int j, int curVal)
{	
	int curMat = ((int)(i/sqrt(mySize)))*sqrt(mySize) + j/sqrt(mySize);
	
	matrix[i][j] = curVal;
	inCol[i][curVal] = 1;
	inRow[j][curVal] = 1;
	inMat[curMat][curVal] = 1;
	
	return;
}

// remove value from an index
void removeValue(int i, int j, int curVal)
{
	int curMat = ((int)(i/sqrt(mySize)))*sqrt(mySize) + j/sqrt(mySize);
	
	matrix[i][j] = 0;
	inCol[i][curVal] = 0;
	inRow[j][curVal] = 0;
	inMat[curMat][curVal] = 0;
	
	return;
}

// assigning value function with backtracking
int solve(int selectMatID)
{
	//if(i==1) cout<<i<<j<<endl;
	//if(i==2) cout<<i<<j<<endl;
	//if(i==mySize - 1) cout<<i<<j<<endl;
	
	int i = selectMat[selectMatID].first;
	int j = selectMat[selectMatID].second;
	
	vector<int> curPossible;
	curPossible.clear();
	int curMat = ((int)(i/sqrt(mySize)))*sqrt(mySize) + j/sqrt(mySize);

	//cout<<i<<" "<<j<<" "<<curMat<<endl;

	// check possible values for current index
	for(int curVal=1; curVal<=mySize; curVal++) {
		if (!(inCol[i][curVal] || inRow[j][curVal] || inMat[curMat][curVal])) {
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
	assignValue(i, j, curVal);
	
	int nextID = selectMatID+1;
	
	// finished
	if (nextID == selectMat.size()) {
		return 1;
	}
	
	// there is a solution with current value
	if(solve(nextID) == 1) {
		return 1;
	}
	
	// try all other solutions
	for(int tryID = (curID+1)%curSize; tryID != curID; tryID = (tryID+1)%curSize) {
		removeValue(i, j, curVal);
		curVal = curPossible[tryID];
		assignValue(i, j, curVal);
		// there is a solution with current value
		if(solve(nextID) == 1) {
			return 1;
		}
	}
	
	// no possible solution
	removeValue(i, j, curVal);
	return -1;
}

int main()
{
	std::srand(std::time(nullptr));
	ofstream fout;
	fout.open("input.txt");
	
	for(int bigI = 0; bigI < number; bigI++) {
		
		// initialize
		selectMat.clear();
		for(int i=0; i<mySize; i++) {
			for(int j=0; j<mySize; j++) {
				matrix[i][j] = 0;
				outMatrix[i][j] = 0;
				inCol[i][j+1] = 0;
				inRow[i][j+1] = 0;
				inMat[i][j+1] = 0;
				selectMat.push_back(make_pair(i, j));
			}
		}
		
		// create empty space
		for(int i=0; i<empty; i++) {
			int curSize = selectMat.size();
			int curVal = random()%curSize;
			selectMat.erase(selectMat.begin()+curVal);
		}
		
		// assign sudoku solution
		solve(0);
		
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
		
		/*
		for(int i=0; i<mySize; i++) {
			for(int j=0; j<mySize; j++) {
				cout<<inCol[i][j+1]<<" ";
			}cout<<endl;
		}cout<<endl;
		
		for(int i=0; i<mySize; i++) {
			for(int j=0; j<mySize; j++) {
				cout<<inRow[i][j+1]<<" ";
			}cout<<endl;
		}cout<<endl;
		
		for(int i=0; i<mySize; i++) {
			for(int j=0; j<mySize; j++) {
				cout<<inMat[i][j+1]<<" ";
			}cout<<endl;
		}
		*/
	}
	
	fout.close();
	
	return 0;
}
