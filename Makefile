EXECUTABLES = \
	      mpi-hello mpi-hello-soln \
	      mpi-send-soln mpi-2send-soln \
	      mpi-nonblock-soln mpi-neighbor \
	      mpi-periodic-send2-soln

all: $(EXECUTABLES)

mpi%: mpi%.c
	mpicc -std=gnu99 -lrt -o$@ $^

%.o : %.c %.h
	mpicc -c $(CL_CFLAGS) -std=gnu99 $<

clean:
	rm -f $(EXECUTABLES) *.o
