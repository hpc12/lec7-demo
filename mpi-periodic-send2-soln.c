#include <mpi.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char** argv) 
{
  int rank, size;
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  char srbuf[16384] = "msg1 right";
  char srbuf2[16384] = "msg2 right";
  char slbuf[16384] = "msg1 left";
  char slbuf2[16384] = "msg2 left";

  MPI_Request reqs[1024];
  int nreq = 0;
  int left = rank - 1;
  if (left < 0) left+=size;

  MPI_Isend(srbuf, sizeof(srbuf), MPI_CHAR, (rank+1) % size, 15, MPI_COMM_WORLD, reqs + nreq++);
  MPI_Isend(srbuf2, sizeof(srbuf2), MPI_CHAR, (rank+1) % size, 15, MPI_COMM_WORLD, reqs + nreq++);
  MPI_Isend(slbuf, sizeof(slbuf), MPI_CHAR, left, 15, MPI_COMM_WORLD, reqs + nreq++);
  MPI_Isend(slbuf2, sizeof(slbuf2), MPI_CHAR, left, 15, MPI_COMM_WORLD, reqs + nreq++);

  for (int j = 0; j < 4; ++j)
  {
    char recvbuf[16384];
    MPI_Status rstat;
    MPI_Recv(recvbuf, sizeof(recvbuf), MPI_CHAR,
        MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &rstat);

    printf("dest-rank %d src-rank %d: %s.\n",
        rank, rstat.MPI_SOURCE, recvbuf);
  }

  MPI_Waitall(nreq, reqs, MPI_STATUSES_IGNORE);

  MPI_Finalize();
  return 0;
}

