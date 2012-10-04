#include <mpi.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char** argv) 
{
  int rank, size;
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  char sbuf[16384] = "I have a present for you.";
  char rbuf[16384];

  if (rank >= 1)
  {
    MPI_Status rstat;
    MPI_Recv(rbuf, sizeof(rbuf), MPI_CHAR,
        MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &rstat);

    printf("I'm rank %d and rank %d (tag %d) said: '%s'.\n",
        rank, rstat.MPI_SOURCE, rstat.MPI_TAG, rbuf);
  }

  if (rank < size-1)
  {
    MPI_Send(sbuf, sizeof(sbuf), MPI_CHAR, (rank+1) % size, 15, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}

