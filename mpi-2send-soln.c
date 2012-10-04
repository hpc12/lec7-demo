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
    char sbuf[16384] = "I have a present for you.";
    char rbuf[16384];
    MPI_Status stat;

    MPI_Send(sbuf, sizeof(sbuf), MPI_CHAR, 1-rank, 15, MPI_COMM_WORLD);

    MPI_Recv(rbuf, sizeof(rbuf), MPI_CHAR,
        MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);

    printf("I'm rank %d and rank %d (tag %d) said: '%s'.\n",
        rank, stat.MPI_SOURCE, stat.MPI_TAG, rbuf);
  }
  else
  {
    char sbuf[16384] = "I have a present for you.";
    char rbuf[16384];
    MPI_Status stat;

    MPI_Recv(rbuf, sizeof(rbuf), MPI_CHAR,
        MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);

    MPI_Send(sbuf, sizeof(sbuf), MPI_CHAR, 1-rank, 15, MPI_COMM_WORLD);

    printf("I'm rank %d and rank %d (tag %d) said: '%s'.\n",
        rank, stat.MPI_SOURCE, stat.MPI_TAG, rbuf);
  }

  MPI_Finalize();
  return 0;
}

