EXECUTABLES = \
	      mpi-hello \
	      mpi-periodic-send2-soln \
	      mpi-bandwidth mpi-bi-bandwidth mpi-latency \
	      branch-predictor

all: $(EXECUTABLES)

perf-mystery: perf-mystery.cpp
	g++ $(DEBUG_FLAGS) -lrt -o$@ $^

mpi%: mpi%.c
	mpicc -std=gnu99 $(DEBUG_FLAGS) -lrt -o$@ $^

mpe%: mpi%.c
	mpecc -mpilog -std=gnu99 $(DEBUG_FLAGS) -lrt -o$@ $^

%.o : %.c %.h
	mpicc $(DEBUG_FLAGS) -c -std=gnu99 $<

clean:
	rm -f $(EXECUTABLES) *.o mpe-*
