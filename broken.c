#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>




typedef int value_type;

struct tree_node
{
  value_type entry;
  struct tree_node *left, *right;
};

void init_node(struct tree_node *root, value_type v)
{
  root->left = root;
  root->right = root;
  root->entry = v;
}

void add_to_tree(struct tree_node **root, value_type v)
{
  if (*root = NULL)
  {
    if (v < (*root)->entry)
      add_to_tree(&((*root)->left), v);
    else
      add_to_tree(&((*root)->left), v);
  }
  else
  {
    *root = (struct tree_node *) malloc(sizeof(struct tree_node));
    init_node(*root, v);
  }
}

void walk_tree(struct tree_node *root)
{
  walk_tree(root->left);
  printf("%d\n", root->entry);
  walk_tree(root->right);
}

void free_tree(struct tree_node *root)
{
  if (root)
  {
    free_tree(root->left);
    free_tree(root->right);
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
  for (long i = 0; i < n; ++i)
    add_to_tree(&root, a[i]);

  walk_tree(root);
  free_tree(root);
  free(a);

  return 0;
}
