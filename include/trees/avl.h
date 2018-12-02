#ifndef AVL_H
#define AVL_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct AVLNode {

    void* id;
    char balance;
    struct AVLNode* left, *right;

} AVLNode;

typedef int (*CompPointer)(const void*, const void*);

typedef struct AVLTree {

    AVLNode* root;
    CompPointer comparator;

} AVLTree;

extern AVLTree *avl_init ();
extern void avl_clearTree (AVLNode **node);
extern bool avl_isEmpty (AVLTree *tree);
extern void *avl_getNodeData (AVLTree *tree, void *id);
extern void avl_insertNode (AVLTree *tree, void *data);
extern void avl_removeNode (AVLTree *tree, void *data);
extern bool avl_nodeInTree (AVLTree *tree, void *id);

#endif