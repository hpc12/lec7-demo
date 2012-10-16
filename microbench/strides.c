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

