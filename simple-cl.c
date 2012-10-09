#include "timing.h"
#include "cl-helper.h"




int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "need an argument!\n");
    abort();
  }

  const long n = atol(argv[1]);

  cl_context ctx;
  cl_command_queue queue;
  create_context_on(CHOOSE_INTERACTIVELY, CHOOSE_INTERACTIVELY, 0, &ctx, &queue, 0);

  // --------------------------------------------------------------------------
  // load kernels 
  // --------------------------------------------------------------------------
  char *knl_text = read_file("simple.cl");
  cl_kernel knl = kernel_from_string(ctx, knl_text, "sum", NULL);
  free(knl_text);

  // --------------------------------------------------------------------------
  // allocate and initialize CPU memory
  // --------------------------------------------------------------------------
  float *a = (float *) malloc(sizeof(float) * n);
  if (!a) { perror("alloc x"); abort(); }

  // --------------------------------------------------------------------------
  // allocate device memory
  // --------------------------------------------------------------------------
  cl_int status;
  cl_mem buf_a = clCreateBuffer(ctx, CL_MEM_READ_WRITE, 
      sizeof(float) * n, 0, &status);
  CHECK_CL_ERROR(status, "clCreateBuffer");

  // --------------------------------------------------------------------------
  // run code on device
  // --------------------------------------------------------------------------

  SET_2_KERNEL_ARGS(knl, buf_a, n);
  size_t ldim[] = { 128 };
  size_t gdim[] = { ((n + ldim[0] - 1)/ldim[0])*ldim[0] };
  CALL_CL_GUARDED(clEnqueueNDRangeKernel,
      (queue, knl,
       /*dimensions*/ 1, NULL, gdim, ldim,
       0, NULL, NULL));

  // --------------------------------------------------------------------------
  // transfer back & check
  // --------------------------------------------------------------------------
  CALL_CL_GUARDED(clEnqueueReadBuffer, (
        queue, buf_a, /*blocking*/ CL_TRUE, /*offset*/ 0,
        n * sizeof(float), a,
        0, NULL, NULL));

  for (size_t i = 0; i < n; ++i)
    printf("%f ", a[i]);
  printf("\n");

  // --------------------------------------------------------------------------
  // clean up
  // --------------------------------------------------------------------------
  CALL_CL_GUARDED(clReleaseMemObject, (buf_a));
  CALL_CL_GUARDED(clReleaseKernel, (knl));
  CALL_CL_GUARDED(clReleaseCommandQueue, (queue));
  CALL_CL_GUARDED(clReleaseContext, (ctx));

  return 0;
}
