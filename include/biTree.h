#ifndef BITREE_H
#define BITREE_H

#include <stdlib.h>

typedef struct BiTreeNode {

    void *data;
    struct BiTreeNode *left;
    struct BiTreeNode *right;

} BiTreeNode;

typedef struct BiTree {

    int size;

    int (*compare) (const void *key1, const void *key2);
    void (*destroy) (void *data);

    BiTreeNode *root;

} BiTree;

#define bitree_size(tree) ((tree)->size)
#define bitree_root(tree) ((tree)->root)
#define bitree_is_eob(node) ((node) == NULL)
#define bitree_is_leaf(node) ((node)->left == NULL && (node)->right == NULL)
#define bitree_data(node) ((node)->data)
#define bitree_left(node) ((node)->left)
#define bitree_right(node) ((node)->right)

extern BiTree *bitree_create (void (*destroy)(void *data));
extern int bitree_ins_left (BiTree *tree, BiTreeNode *node, const void *data);
extern int bitree_ins_right (BiTree *tree, BiTreeNode *node, const void *data);
extern void bitree_rem_left (BiTree *tree, BiTreeNode *node);
extern void bitree_rem_right (BiTree *tree, BiTreeNode *node);
extern void bitree_destroy (BiTree *tree);
extern BiTree *bitree_merge (BiTree *left, BiTree *right, const void *data);


#endif