EXECUTABLES = tree-sort matmul cl-demo tree-sort-wrong tree-sort-critical tree-sort

all: $(EXECUTABLES)

matmul: matmul.c
	gcc -std=gnu99 $(DEBUG_FLAGS) -lrt -o$@ $^

tree-sort-wrong: tree-sort.c
	gcc -std=gnu99 -DUSE_WRONG -fopenmp $(DEBUG_FLAGS) -lrt -o$@ $^

tree-sort-critical: tree-sort.c
	gcc -std=gnu99 -DUSE_CRITICAL_SECTION -fopenmp $(DEBUG_FLAGS) -lrt -o$@ $^

tree-sort: tree-sort.c
	gcc -std=gnu99 -DUSE_SEQUENTIAL $(DEBUG_FLAGS) -lrt -o$@ $^

ifdef OPENCL_INC
  CL_CFLAGS = -I$(OPENCL_INC)
endif

ifdef OPENCL_LIB
  CL_LDFLAGS = -L$(OPENCL_LIB)
endif

cl-demo: cl-demo.c cl-helper.c
	gcc $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -lrt -lOpenCL -o$@ $^

clean:
	rm -f $(EXECUTABLES) *.o mpe-*
