#include <mpi.h>
#include <stdio.h>
#include <vector>
#include <utility>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <iostream>

#include "timing.h"

#define MASTER 0
#define FROM_MASTER 1
#define FROM_WORKER 2
#define S_SIZE 3
#define M_SIZE 9

#define MPI_TAG_DONE 1
#define MPI_TAG_STEAL 2
#define MPI_TAG_WORK 3

int pid, nproc;

void printSudoku(std::vector<int> sudoku) {
    for (int i = 0; i < M_SIZE*M_SIZE; i++) {
      if(i%M_SIZE != M_SIZE - 1)
          printf("%2d ", sudoku[i]);
      else
          printf("%2d\n", sudoku[i]);
    }
}

bool isSafe(std::vector<int> base, std::vector<int> check, int value, int node) {
    int index;
    int row = node / M_SIZE;
    int col = node % M_SIZE;
    int boxr = (row / S_SIZE) * S_SIZE;
    int boxc = (col / S_SIZE) * S_SIZE;
    
    /** check horizontal **/
    for (int c = 0; c < M_SIZE; c++) {
    index = row*M_SIZE + c;
    if (base[index] == value || check[index] == value) return false;
    }
    
    /** check vertical **/
    for (int r = 0; r < M_SIZE; r++) {
    index = r*M_SIZE + col;
    if (base[index] == value || check[index] == value) return false;
    }
    
    /** check box **/
    for (int r = boxr; r < boxr+S_SIZE; r++) {
    for (int c = boxc; c < boxc+S_SIZE; c++) {
      index = r*M_SIZE + c;
      //printf("current box r: %d, current box c: %d \n", boxr, boxc);
      if (base[index] == value || check[index] == value) return false;
    }
    }
    
    return true;
}

// need further implementation for work stealing
std::pair<int, std::vector<int>> solver(std::vector<int> baseSudoku, std::vector<int> checkSudoku,
                                  std::vector<int> nodes, std::vector<int> assignValues, int last) {
    std::vector sNodes = nodes;
    std::vector sValues = assignValues;
    std::vector base = baseSudoku;
    std::vector check = checkSudoku;
  	std::vector<int> sendCheck;
    int oldSize, start, node, value, sendNode, sendValue;
    
    int flag = 0;
    MPI_Status status;
    
    //int curMax = 0;
    
    while(1) {
        
        //std::cerr << __LINE__ << " " << pid << std::endl;
      
        while(sNodes.size() > 0) {
            
            /** get the current trial **/
            node = sNodes.back();
            value = sValues.back();
            sNodes.pop_back();
            sValues.pop_back();
            oldSize = sNodes.size();
            start = node;
            
            for (int c = start; c < M_SIZE*M_SIZE; c++) {
                if (check[c] > -1) check[c] = 0;
            }
            
            //printf("size: %d last: %d\n", oldSize, sNodes[oldSize-1]);
            
            /** check if the num is valid **/
            if (!isSafe(base, check, value, node)) {
              continue;
            }
            
            /** solve the sudoku given the num **/
            while (1) {
              
                MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
              	//std::cerr << __LINE__ << " " << pid << " " << status.MPI_SOURCE << " " << status.MPI_TAG << " " << flag << std::endl;  
              
                if (flag) {
                	//std::cerr << __LINE__ << " " << pid << std::endl;
                  	int tagmsg = 0;
                  	MPI_Recv(&tagmsg, 1, MPI_INT, status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                  
                    /** another worker finished the search **/
                    if (tagmsg == MPI_TAG_DONE) {
                        return std::make_pair(false, base);
                    }
                    
                    /** another worker wants to steal more work **/
                    if (tagmsg == MPI_TAG_STEAL) {
                    		//std::cerr << __LINE__ << " " << pid << std::endl;
                        /** have work to send **/
                        int successSend;
                        if (sNodes.size() > 1) {
                          
                            /** pop the front element on the stack **/
                          	successSend = MPI_TAG_WORK;
                          	sendNode = sNodes[0];
                          	sendValue = sValues[0];
                          	sNodes.erase(sNodes.begin());
                          	sValues.erase(sValues.begin());
                          	oldSize = sNodes.size();
                          
                            /** send success signal **/
                            MPI_Send(&successSend, 1, MPI_INT, status.MPI_SOURCE, MPI_TAG_WORK, MPI_COMM_WORLD);
                            /** send node & value **/
                            MPI_Send(&sendNode, 1, MPI_INT, status.MPI_SOURCE, MPI_TAG_WORK, MPI_COMM_WORLD);
                            MPI_Send(&sendValue, 1, MPI_INT, status.MPI_SOURCE, MPI_TAG_WORK, MPI_COMM_WORLD);
                          
                            /** clear send check **/
                          	sendCheck = check;
                          	for (int c = sendNode; c < M_SIZE*M_SIZE; c++) {
                            	if (sendCheck[c] > -1) sendCheck[c] = 0;
                            }
                          	
                          	/** send check **/
                            int checkSize = sendCheck.size();
                            MPI_Send(&checkSize , 1, MPI_INT, status.MPI_SOURCE, MPI_TAG_WORK, MPI_COMM_WORLD);
                            for(int i=0; i<checkSize ; i++) {
                                MPI_Send(&sendCheck[i], 1, MPI_INT, status.MPI_SOURCE, MPI_TAG_WORK, MPI_COMM_WORLD);
                            }
                            
                        }
                        /** no work to send **/
                        else {
                            successSend = 0;
                            MPI_Send(&successSend, 1, MPI_INT, status.MPI_SOURCE, MPI_TAG_WORK, MPI_COMM_WORLD);
                        }
                        
                        flag = 0;
                    }
                }
                
                check[node] = value;
                
                /** find next node to assign value **/
                for (int i = node+1; i < M_SIZE*M_SIZE; i++) {
                    /** fixed **/
                    if (check[i] == -1) {
                      continue;
                    }
                    
                    /** find all valid values for next node **/
                    for (int v = M_SIZE; v > 0; v--) {
                        if (!isSafe(base, check, v, i))
                            continue;
                        
                        /** the sudoku is filled. return the solution **/
                        if (i == last) {
                            check[i] = v;
                            for (int c = 0; c < M_SIZE*M_SIZE; c++) {
                                if (check[c] != -1) base[c] = check[c];
                            }
                            
                            //std::cerr << __LINE__ << " " << pid << std::endl;
                            
                            printf("Worker %d has a solution \n", pid);
                            
                            flag = MPI_TAG_DONE;
                            
                            /** ask others to finish **/
                            //MPI_Bcast(&flag, 1, MPI_INT, pid, MPI_COMM_WORLD);
                            
                            for (int c = 0; c < nproc; c++) {
                              MPI_Send(&flag, 1, MPI_INT, c, MPI_TAG_DONE, MPI_COMM_WORLD);
                            }

                            return std::make_pair(true, base);
                        }
                        
                        /*if (i > curMax) {
                        curMax = i;
                        printf("%d\n", i);
                        }*/
                        sNodes.push_back(i);
                        sValues.push_back(v);
                    }
                    break;
                }
                
                /** this path gives no solution, delete the path **/
                if (sNodes.size() <= oldSize) {
                    for (int c = start; c < M_SIZE*M_SIZE; c++) {
                        if (check[c] > -1) check[c] = 0;
                    }
                    /*
                    printf("path not valid: clear sudoku\n");
                    printf("start:%d \n", start);
                    printSudoku(check);
                    printf("_______________________\n");
                    */
                    break;
                }
                
                /** we can still go further **/
                node = sNodes.back();
                value = sValues.back();
                sNodes.pop_back();
                sValues.pop_back();
                oldSize = sNodes.size();
                start = node;
                /** clear all the cells after the start node **/
                for (int c = start; c < M_SIZE*M_SIZE; c++) {
                    if (check[c] > -1) check[c] = 0;
                }
                /*
                printf("new start: %d, new value: %d \n", start, value);
                printSudoku(check);
                printf("___________________\n");
                */
            }
        }
        
        /** finished current nodes, steal work **/
        flag = MPI_TAG_STEAL;
        
      	int hasRecv = 0;
      
        //std::cerr << __LINE__ << " " << pid << std::endl;
      	
      	//MPI_Bcast(&flag, 1, MPI_INT, pid, MPI_COMM_WORLD);
      	
      	for (int i = pid+1; i < nproc; i++) {
          if (i != pid) {
            MPI_Send(&flag, 1, MPI_INT, i, MPI_TAG_STEAL, MPI_COMM_WORLD);
            int successRecv = 0;
            //std::cerr << __LINE__ << " " << pid <<":" << i << std::endl;
            
            int recvFlag;
            MPI_Iprobe(i, MPI_TAG_WORK, MPI_COMM_WORLD, &recvFlag, &status);
            
            if (recvFlag) {
            
              MPI_Recv(&successRecv, 1, MPI_INT, i, MPI_TAG_WORK, MPI_COMM_WORLD, &status);
              //std::cerr << __LINE__ << " " << pid <<":" << i << std::endl;
              if (successRecv == MPI_TAG_WORK) {
                MPI_Recv(&node, 1, MPI_INT, i, MPI_TAG_WORK, MPI_COMM_WORLD, &status);
                MPI_Recv(&value, 1, MPI_INT, i, MPI_TAG_WORK, MPI_COMM_WORLD, &status);
                int checkSize;
                MPI_Recv(&checkSize, 1, MPI_INT, i, MPI_TAG_WORK, MPI_COMM_WORLD, &status);
                check.resize(0);
                int curCheck;
                for (int j = 0; j < checkSize; j++) {
                  MPI_Recv(&curCheck, 1, MPI_INT, i, MPI_TAG_WORK, MPI_COMM_WORLD, &status);
                  check.push_back(curCheck);
                }
                hasRecv = 1;
                break;
              } else if (successRecv == MPI_TAG_DONE) {
                return std::make_pair(false, base);
              }
            }
          }
        }
      	flag = 0;
      
        //std::cerr << __LINE__ << " " << pid << std::endl;
      
      	if (!hasRecv) break;
      
    }
    
    /** no solution **/
    return std::make_pair(false, base);
}

int main(int argc, char *argv[]) {
    
    MPI_Status status;
    MPI_Request request;
    
    int size = M_SIZE * M_SIZE;
    std::vector<int> result;
    
    /** Initialize MPI **/
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    
    printf("pid: %d, nproc: %d \n", pid, nproc);
    
    // how do we read in file here?
    std::vector<int> baseSudoku{};
    /*{
    0, 0, 0, 0, 0, 0, 0, 1, 0,
    4, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 5, 0, 4, 0, 7,
    0, 0, 8, 0, 0, 0, 3, 0, 0,
    0, 0, 1, 0, 9, 0, 0, 0, 0,
    3, 0, 0, 4, 0, 0, 2, 0, 0,
    0, 5, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 8, 0, 6, 0, 0, 0};*/
    
    if (pid == MASTER) {
    
        std::ifstream fin("mpi_input.txt");
        
        int tmp;
        
        while(fin>>tmp) {
            baseSudoku.push_back(tmp);
        }
        
        fin.close();
    }

    std::vector<int> checkSudoku;
    
    baseSudoku.resize(size);
    checkSudoku.resize(size);
    
    /*StartupOptions options = parseOptions(argc, argv);
    
    if (pid == MASTER) {
    loadFromFile(options.inputFile, baseSudoku);
    }*/
    
    MPI_Barrier(MPI_COMM_WORLD);
    Timer totalSimulationTimer;
    
    /** initialize base sudoku and soduku checker **/
    MPI_Bcast(&baseSudoku[0], size, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    
    int start, last;
    bool isFirst = true;
    for (int i = 0; i < size; i++) {
    if (baseSudoku[i] > 0) {
        checkSudoku[i] = -1;
    }
    else {
        checkSudoku[i] = 0;
        if (isFirst)
        {
            start = i;
            isFirst = false;
        }
        last = i;
    }
    }
    
    /** initialize assignValues and nodes **/
    std::vector<int> nodes;
    std::vector<int> assignValues;
    int min_choice = std::min(M_SIZE/nproc*pid+1, M_SIZE);
    int max_choice = std::min(M_SIZE/nproc*(pid+1), M_SIZE);
    for (int i = max_choice; i >= min_choice; i--) {
        nodes.push_back(start); // the last node is the head
        assignValues.push_back(i);
    }
    
    /** solve the sudoku given the assignedValues **/
    std::pair<int, std::vector<int>> sol = solver(baseSudoku, checkSudoku, nodes, assignValues, last);
    
    /** gather the valid solutions **/
    bool shouldPrint = false;
    int hasSolution = 0;
    if (sol.first) hasSolution = 1;
    std::vector<int> isValid;
    isValid.resize(nproc);
    if (pid == MASTER) {
        for (int procId = 1; procId < nproc; procId++) {
            MPI_Recv(&isValid[procId], 1, MPI_INT, procId, FROM_WORKER, MPI_COMM_WORLD, &status);
        }
    }
    
    else {
        MPI_Isend(&hasSolution, 1, MPI_INT, MASTER, FROM_WORKER, MPI_COMM_WORLD, &request);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    /** send only one valid solution **/
    bool printed = false;
    bool temp;
    if (pid == MASTER) {
        if (hasSolution == 1) {
            shouldPrint = true;
            printed = true;
        }
        
        for (int procId = 1; procId < nproc; procId++) {
            temp = isValid[procId] && !printed;
            if (temp) printed = true;
            MPI_Isend(&temp, 1, MPI_CXX_BOOL, procId, FROM_MASTER, MPI_COMM_WORLD, &request);
        }
    }
    
    else {
        MPI_Recv(&shouldPrint, 1, MPI_CXX_BOOL, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    double totalSimulationTime = totalSimulationTimer.elapsed();
    
    if (sol.first && shouldPrint) {
        printf("current pid: %d \n", pid);
        printSudoku(sol.second);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (pid == MASTER) {
        if (!printed) printf("No solution! \n");
        printf("total simulation time: %.6fs\n", totalSimulationTime);
    }
    
    MPI_Finalize();
}
