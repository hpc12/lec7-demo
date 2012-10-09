EXECUTABLES = broken \
	      mpi-hello mpi-hello-soln \
	      mpi-send-soln mpi-2send-soln \
	      mpi-nonblock-soln mpi-neighbor \
	      mpi-periodic-send2-soln

all: $(EXECUTABLES)

broken: broken.c
	gcc $(DEBUG_FLAGS) -std=gnu99 -lrt -o$@ $^

mpi%: mpi%.c
	mpicc -std=gnu99 -lrt -o$@ $^

%.o : %.c %.h
	mpicc -c -std=gnu99 $<

clean:
	rm -f $(EXECUTABLES) *.o
