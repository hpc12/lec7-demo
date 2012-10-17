/*********************************************************************
 * File: matmul.c
 * Author: Travis Askham, created 9/10/2012
 * Description: reads in an integer n, creates two random matrices of
 * doubles of size nxn, and computes their product via 1 of 2
 * subroutines.
 ********************************************************************/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "timing.h"


double timespec_diff_in_seconds(struct timespec start, struct timespec end);
double rand_d(double a, double b);
void fill_array(double *A, long len, double a, double b);
void print_matrix(const double * A, long n, char * message);
void transpose_matrix(double * At, double * A, long n);
void part_a(long n, bool give_result, double * result, bool verbose);
void part_b(long n, bool give_result, double * result, bool verbose);
void compare_a_and_b(long n);

// returns a random double in the interval [a,b]
double rand_d(double a, double b)
{
  double c;
  c = rand()/(1.0*RAND_MAX);
  c = a + c*(b-a);
  return c;
}

// fills an array with random doubles in [a,b] using the function rand_d
void fill_array(double *A, long len, double a, double b)
{
  for (int i=0; i<len; i++)
    A[i] = rand_d(a,b);
  return;
}

// prints a matrix (stored as an array in column-major order)
void print_matrix(const double * A, long n, char * message)
{

  printf("%s \n",message);
  for (int i=0; i<n; i++)
  {
    for (int j=0; j<n; j++)
      printf("%11.3E",A[i+n*j]);
    printf("\n");
  }
  return;
}

// copies the transpose of the matrix A into At
void transpose_matrix(double * At, double * A, long n)
{
  for (int i=0; i<n; i++)
  {
    for(int j=0; j<n; j++)
      At[i+n*j] = A[j+n*i];
  }

  return;
}


void part_a(long n, bool give_result, double * result, bool verbose)
{
  srand(4);
  double a,b;
  a = .5;
  b=2;
  long ns = n*n;

  // allocate 3 arrays of size nxn
  double *A = (double *) malloc(sizeof(double) *ns);
  if(!A) { fprintf(stderr,"in part_a: failed to allocate A\n"); abort();}
  double *B = (double *) malloc(sizeof(double) *ns);
  if(!B) { fprintf(stderr,"in part_a: failed to allocate B\n"); abort();}
  double *C = (double *) malloc(sizeof(double) *ns);
  if(!C) { fprintf(stderr,"in part_a: failed to allocate C\n"); abort();}

  // timing
  timestamp_type time1, time2;
  get_timestamp(&time1);

  // fill arrays with random doubles in [a,b]
  fill_array(A,n*n,a,b);
  fill_array(B,n*n,a,b);
  fill_array(C,n*n,a,b);

  double sum;

  // multiply the arrays as matrices stored in column-major order
  for (int i=0; i<n; i++)
  {
    for (int j=0; j<n; j++)
    {
      sum = 0;
      for (int k=0; k<=n; k++)
      {
        // sum = sum + A[i,k]*B[k,j]
        sum = sum + A[i+n*k]*B[k+n*j];
      }
      C[i+n*j] = sum;
    }
  }

  //timing
  get_timestamp(&time2);
  printf("that took %f seconds\n",
      timestamp_diff_in_seconds(time1,time2));

  // if give_result is true, we write the matrix C into the
  // result vector (which is assumed to have been allocated
  // to the correct size)
  if (give_result){
    for(int i=0; i<ns; i++){
      result[i] = C[i];
    }
  }

  if (verbose){
    print_matrix(A,n,"A = ");
    print_matrix(B,n,"B = ");
    print_matrix(C,n,"C = ");
  }

  free(B);
  free(C);

  return;

}


int main(int argc, char ** argv)
{
  long n;

  if (argc >= 2)
    n = atol(argv[1]);

  part_a(n,false,NULL,false);
  return 0;
}
