echo "Compiling MPI Solver with no work stealing"
mpic++ MPI_nosteal.cpp -o MPI_nosteal
echo "Testing MPI Solver with no work stealing on 1 worker"
mpirun -n 1 MPI_nosteal
echo "Testing MPI Solver with no work stealing on 8 workers"
mpirun -n 8 MPI_nosteal
echo "---------------------------------------------"
echo "Compiling MPI Solver"
mpic++ MPI_sudoku_solver.cpp -o MPI_sudoku_solver
echo "Testing MPI Solver on 1 worker"
mpirun -n 1 MPI_sudoku_solver
echo "Testing MPI Solver on 8 workers"
mpirun -n 8 MPI_sudoku_solver