#include <stdlib.h>
#include <mpi.h>
#include <stdio.h>


int main(int argc, char** argv) 
{
  int rank, size;
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  float mynumber = 0;
  if (rank == 0)
    mynumber = ((float) rand()) / RAND_MAX;

  printf("%d's number (before) is %f.\n", rank, mynumber);

  MPI_Bcast(&mynumber, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

  printf("%d's number (after) is %f.\n", rank, mynumber);

  MPI_Finalize();
  return 0;
}
