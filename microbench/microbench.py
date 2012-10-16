# http://igoro.com/archive/gallery-of-processor-cache-effects/

import os
from time import time
import matplotlib.pyplot as pt
import numpy as np
from codepy.toolchain import guess_toolchain
toolchain = guess_toolchain()

from codepy.libraries import add_boost_python
add_boost_python(toolchain)


MODULE_CODE = """
#include <boost/scoped_array.hpp>
#include <boost/python.hpp>

%s

BOOST_PYTHON_MODULE(module)
{
  boost::python::def("go", &go);
}
"""




def measure_strides():
    FUNC_CODE = """
    void go(unsigned count, unsigned stride)
    {
      const unsigned arr_size = 64 * 1024 * 1024;
      int *ary = (int *) malloc(sizeof(int) * arr_size);

      for (unsigned it = 0; it < count; ++it)
      {
        for (unsigned i = 0; i < arr_size; i += stride)
          ary[i] *= 17;
      }

      free(ary);
    }
    """
    from codepy.jit import extension_from_string
    cmod = extension_from_string(toolchain, "module", MODULE_CODE % FUNC_CODE)

    strides = []
    times = []

    count = 10
    for stride in [2**i for i in range(0, 11)]:
        start = time()
        cmod.go(count, stride)
        stop = time()

        strides.append(stride)
        times.append((stop-start)/count)

    pt.clf()
    pt.rc("font", size=20)
    pt.semilogx(strides, times, "o-", basex=2)
    pt.xlabel("Stride")
    pt.ylabel("Time [s]")
    pt.grid()
    pt.subplots_adjust(bottom=0.15)
    pt.savefig("strides.pdf")

    open("strides.c", "w").write(FUNC_CODE)

    os.system("pdfcrop strides.pdf")





def measure_cache_bandwidths():
    FUNC_CODE = """
    void go(unsigned array_size, unsigned steps)
    {
      int *ary = (int *) malloc(sizeof(int) * array_size);
      unsigned asm1 = array_size - 1;

      for (unsigned i = 0; i < steps; ++i)
        ary[(i*16) & asm1] ++;

      free(ary);
    }
    """
    from codepy.jit import extension_from_string
    cmod = extension_from_string(toolchain, "module", MODULE_CODE % FUNC_CODE)

    sizes = []
    bandwidths = []

    steps = 2**26
    for array_size in [2**i for i in range(10, 26)]:
        start = time()
        cmod.go(array_size, steps)
        stop = time()

        sizes.append(array_size*4)
        elapsed = stop-start

        gb_transferred = 2*steps*4/1e9 # 2 for rw, 4 for sizeof(int)
        bandwidth = gb_transferred/elapsed

        bandwidths.append(bandwidth)

        print array_size, bandwidth

    pt.clf()
    pt.rc("font", size=20)
    pt.semilogx(sizes, bandwidths, "o-", basex=2)
    pt.xlabel("Array Size [Bytes]")
    pt.ylabel("Eff. Bandwidth [GB/s]")
    pt.grid()
    pt.subplots_adjust(bottom=0.15)
    pt.savefig("bw.pdf")

    open("bw.c", "w").write(FUNC_CODE)

    os.system("pdfcrop bw.pdf")




def find_associativity():
    FUNC_CODE = """
    void go(unsigned array_size, unsigned stride, unsigned steps)
    {
      char *ary = (char *) malloc(sizeof(int) * array_size);

      unsigned p = 0;
      for (unsigned i = 0; i < steps; ++i)
      {
        ary[p] ++;
        p += stride;
        if (p >= array_size)
          p = 0;
      }

      free(ary);
    }
    """
    from codepy.jit import extension_from_string
    cmod = extension_from_string(toolchain, "module", MODULE_CODE % FUNC_CODE)

    result = {}

    steps = 2**20
    from pytools import ProgressBar
    meg_range = range(1, 25)
    stride_range = range(1, 640)
    pb = ProgressBar("bench", len(meg_range)*len(stride_range))
    for array_megs in meg_range:
        for stride in stride_range:
            start = time()
            cmod.go(array_megs<<20, stride, steps)
            stop = time()

            elapsed = stop-start
            gb_transferred = 2*steps/1e9 # 2 for rw, 4 for sizeof(int)
            bandwidth = gb_transferred/elapsed

            result[array_megs, stride] = bandwidth
            pb.progress()

    from cPickle import dump
    dump(result, open("assoc_result.dat", "w"))

    open("assoc.c", "w").write(FUNC_CODE)

def plot_associativity():
    from cPickle import load
    results = load(open("assoc_result.dat"))

    megs = sorted(set(k[0] for k in results.iterkeys()))
    strides = sorted(set(k[1] for k in results.iterkeys()))

    ary = np.zeros((len(megs), len(strides)))
    for i_m, meg in enumerate(megs):
        for i_s, stride in enumerate(strides):
            ary[i_m, i_s] = results[meg, stride]

    pt.clf()
    pt.rc("font", size=20)
    pt.imshow(
        np.log(ary), 
        origin="lower", 
        extent=(min(strides), max(strides), min(megs), max(megs)),
        interpolation="nearest", aspect="auto")
    pt.xlabel("Stride [bytes]")
    pt.ylabel("Array Size [MB]")
    pt.savefig("assoc.pdf")
    #pt.show()

    os.system("pdfcrop assoc.pdf")




if __name__ == "__main__":
    #measure_strides()
    measure_cache_bandwidths()
    #find_associativity()
    #plot_associativity()
