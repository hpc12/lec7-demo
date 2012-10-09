__kernel void sum(__global float *a, long n)
{
  int gid = get_global_id(0);
  int lid = get_local_id(0);

  local float a_loc[128];

  float v = 0;
  for (int i = 0; i < 10; ++i)
    v += gid * i;

  a_loc[lid] = v;

  barrier(CLK_LOCAL_MEM_FENCE);

  if (gid < n)
    a[gid] = a_loc[lid];
}
