EXECUTABLES = broken \
	      mpi-hello mpi-hello-soln \
	      mpi-send-soln mpi-2send-soln \
	      mpi-nonblock-soln mpi-neighbor \
	      mpi-periodic-send2-soln \
	      simple-cl

all: $(EXECUTABLES)

broken: broken.c
	gcc $(DEBUG_FLAGS) -fopenmp -std=gnu99 -lrt -o$@ $^

mpi%: mpi%.c
	mpicc -std=gnu99 $(DEBUG_FLAGS) -lrt -o$@ $^

%.o : %.c %.h
	mpicc $(DEBUG_FLAGS) -c -std=gnu99 $<

clean:
	rm -f $(EXECUTABLES) *.o

# OpenCL business -------------------------------------------------------------

ifdef OPENCL_INC
  CL_CFLAGS = -I$(OPENCL_INC)
endif

ifdef OPENCL_LIB
  CL_LDFLAGS = -L$(OPENCL_LIB)
endif

simple-cl: simple-cl.c cl-helper.c
	gcc $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -lrt -lOpenCL -o$@ $^
