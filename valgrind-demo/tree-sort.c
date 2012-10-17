#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <omp.h>
#include "timing.h"




typedef int value_type;

struct tree_node
{
  value_type entry;
  struct tree_node *left, *right;
#ifdef USE_LOCKING
  omp_lock_t lock;
#endif
};

void init_node(struct tree_node *root, value_type v)
{
  root->left = 0;
  root->right = 0;
  root->entry = v;
#ifdef USE_LOCKING
  omp_init_lock(&root->lock);
#endif
}

void add_to_tree(struct tree_node **root, value_type v)
{
  bool is_done = false;

  if (!*root)
  {
    *root = (struct tree_node *) malloc(sizeof(struct tree_node));
    init_node(*root, v);
    is_done = true;
  }

  if (is_done) return;

  if (v < (*root)->entry)
    add_to_tree(&((*root)->left), v);
  else
    add_to_tree(&((*root)->right), v);
}

void add_to_tree_critical(struct tree_node **root, value_type v)
{
  value_type entry;
  bool is_done = false;
#pragma omp critical
  {
    if (!*root)
    {
      *root = (struct tree_node *) malloc(sizeof(struct tree_node));
      init_node(*root, v);
      is_done = true;
    }

    entry = (*root)->entry;
  }

  if (is_done) return;

  if (v < entry)
    add_to_tree_critical(&((*root)->left), v);
  else
    add_to_tree_critical(&((*root)->right), v);
}

#ifdef USE_LOCKING
void add_to_tree_lock(struct tree_node **root, value_type v, omp_lock_t *lock)
{
  value_type entry;
  bool is_done = false;

  omp_set_lock(lock);
  {
    if (!*root)
    {
      *root = (struct tree_node *) malloc(sizeof(struct tree_node));
      init_node(*root, v);
      is_done = true;
    }

    entry = (*root)->entry;
  }
  omp_unset_lock(lock);

  if (is_done) return;

  if (v < entry)
    add_to_tree_lock(&((*root)->left), v, &((*root)->lock));
  else
    add_to_tree_lock(&((*root)->right), v, &((*root)->lock));
}
#endif

void walk_tree(struct tree_node *root)
{
  if (root)
  {
    walk_tree(root->left);
    printf("%d\n", root->entry);
    walk_tree(root->right);
  }
}

void free_tree(struct tree_node *root)
{
  if (root)
  {
    free_tree(root->left);
    free_tree(root->right);
#ifdef USE_LOCKING
    omp_destroy_lock(&root->lock);
#endif
    free(root);
  }
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "need an argument\n");
    abort();
  }

  long n = atol(argv[1]);

  srand(17);
  value_type *a = (value_type *) malloc(sizeof(int) * n);
  if (!a) { perror("allocate a"); abort(); }

  for (long i = 0; i < n; ++i)
    a[i] = rand();

  struct tree_node *root = 0;

  timestamp_type start, end;
  get_timestamp(&start);

#if defined(USE_SEQUENTIAL)
  for (long i = 0; i < n; ++i)
    add_to_tree(&root, a[i]);

#elif defined(USE_WRONG)

# pragma omp parallel for default(none) shared(n,root,a)
  for (long i = 0; i < n; ++i)
    add_to_tree(&root, a[i]);

#elif defined(USE_CRITICAL_SECTION)

# pragma omp parallel for default(none) shared(n,root,a)
  for (long i = 0; i < n; ++i)
    add_to_tree_critical(&root, a[i]);

#elif defined(USE_LOCKING)

  omp_lock_t root_lock;
  omp_init_lock(&root_lock);

# pragma omp parallel for default(none) shared(n,root,a,root_lock)
  for (long i = 0; i < n; ++i)
    add_to_tree_lock(&root, a[i], &root_lock);

  omp_destroy_lock(&root_lock);

#else

#  error must choose variant USE_XXX

#endif

  get_timestamp(&end);
  double elapsed = timestamp_diff_in_seconds(start, end);

  fprintf(stderr, "elapsed: %f s - rate: %f M entries/s\n", elapsed, n*1e-6/elapsed);

  walk_tree(root);
  free_tree(root);
  free(a);

  return 0;
}
