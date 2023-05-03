echo "Compiling Sequential Generator"
g++ seq_generator.cpp -o seq_generator
echo "Compiling Sequential Solver"
g++ seq_solver.cpp -o seq_solver
echo "Testing Sequential Generator"
./seq_generator
echo "---------------------------------------------"
export OMP_NUM_THREADS=1
echo "Thread number = 1"
echo "Compiling OpenMP Generator"
g++ -fopenmp omp_generator.cpp -o omp_generator
echo "Testing OpenMP Generator"
./omp_generator
echo "---------------------------------------------"
export OMP_NUM_THREADS=2
echo "Thread number = 2"
echo "Compiling OpenMP Generator"
g++ -fopenmp omp_generator.cpp -o omp_generator
echo "Testing OpenMP Generator"
./omp_generator
echo "---------------------------------------------"
export OMP_NUM_THREADS=4
echo "Thread number = 4"
echo "Compiling OpenMP Generator"
g++ -fopenmp omp_generator.cpp -o omp_generator
echo "Testing OpenMP Generator"
./omp_generator
echo "---------------------------------------------"
export OMP_NUM_THREADS=8
echo "Thread number = 8"
echo "Compiling OpenMP Generator"
g++ -fopenmp omp_generator.cpp -o omp_generator
echo "Testing OpenMP Generator"
./omp_generator
echo "---------------------------------------------"
echo "Testing Sequential Solver"
./seq_solver
echo "---------------------------------------------"
export OMP_NUM_THREADS=1
echo "Thread number = 1"
echo "Compiling OpenMP Solver"
g++ -fopenmp omp_solver.cpp -o omp_solver
echo "Testing OpenMP Solver"
./omp_solver
echo "---------------------------------------------"
export OMP_NUM_THREADS=2
echo "Thread number = 2"
echo "Compiling OpenMP Solver"
g++ -fopenmp omp_solver.cpp -o omp_solver
echo "Testing OpenMP Solver"
./omp_solver
echo "---------------------------------------------"
export OMP_NUM_THREADS=4
echo "Thread number = 4"
echo "Compiling OpenMP Solver"
g++ -fopenmp omp_solver.cpp -o omp_solver
echo "Testing OpenMP Solver"
./omp_solver
echo "---------------------------------------------"
export OMP_NUM_THREADS=8
echo "Thread number = 8"
echo "Compiling OpenMP Solver"
g++ -fopenmp omp_solver.cpp -o omp_solver
echo "Testing OpenMP Solver"
./omp_solver
echo "---------------------------------------------"
echo "Compiling MPI Solver"
mpic++ MPI_sudoku_solver.cpp -o MPI_sudoku_solver
echo "Testing MPI Solver on 1 worker"
mpirun -n 1 MPI_sudoku_solver
echo "Testing MPI Solver on 2 workers"
mpirun -n 2 MPI_sudoku_solver
echo "Testing MPI Solver on 4 workers"
mpirun -n 4 MPI_sudoku_solver
echo "Testing MPI Solver on 8 workers"
mpirun -n 8 MPI_sudoku_solver