
    void go(unsigned array_size, unsigned steps)
    {
      int *ary = (int *) malloc(sizeof(int) * array_size);
      unsigned asm1 = array_size - 1;

      for (unsigned i = 0; i < steps; ++i)
        ary[(i*16) & asm1] ++;

      free(ary);
    }
    