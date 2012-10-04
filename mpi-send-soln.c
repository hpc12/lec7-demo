#include <mpi.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char** argv) 
{
  int rank, size;
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (rank == 0)
  {
    char buf[1024] = "I'm sorry Dave.";
    MPI_Send(buf, strlen(buf)+1, MPI_CHAR, 1, /* tag */ 15, MPI_COMM_WORLD);
  }
  else if (rank == 1)
  {
    MPI_Status stat;

    char buf[1024];
    MPI_Recv(buf, sizeof(buf), MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);

    int count;
    MPI_Get_count(&stat, MPI_CHAR, &count);

    printf("Rank %d (tag %d) said: '%s' (%d chars).\n",
        stat.MPI_SOURCE, stat.MPI_TAG, buf, count);
  }

  MPI_Finalize();
  return 0;
}
