EXECUTABLES = mpi-periodic-send2-soln

all: $(EXECUTABLES)

mpi%: mpi%.c
	mpicc -std=gnu99 $(DEBUG_FLAGS) -lrt -o$@ $^

mpe%: mpi%.c
	mpecc -mpilog -std=gnu99 $(DEBUG_FLAGS) -lrt -o$@ $^

clean:
	rm -f $(EXECUTABLES) *.o mpe-*
