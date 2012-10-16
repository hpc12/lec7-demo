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
