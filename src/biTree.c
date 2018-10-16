#include <stdlib.h>
#include <string.h>

#include "biTree.h"

BiTree *bitree_create (void (*destroy)(void *data)) {

    BiTree *tree = (BiTree *) malloc (sizeof (BiTree));
 
    tree->size = 0;
    tree->destroy = destroy;
    tree->root = NULL;

    return tree;

}

int bitree_ins_left (BiTree *tree, BiTreeNode *node, const void *data) {

    BiTreeNode *new, **pos;

    // determine where to insert the node
    if (!node) {
        // insert at the root -> but only if it is empty
        if (bitree_size (tree) > 0) return 1;
        else pos = &tree->root;
    }
    else {
        // insert at the end of the branch
        if (bitree_left (node)) return 1;
        else pos = &node->left;
    }

    if (!(new = (BiTreeNode *) malloc (sizeof (BiTreeNode)))) return 1;

    new->data = (void *) data;
    new->left = NULL;
    new->right = NULL;
    *pos = new;

    tree->size++;

    return 0;   

}

int bitree_ins_right (BiTree *tree, BiTreeNode *node, const void *data) {

    BiTreeNode *new, **pos;

    // determine where to insert the node
    if (!node) {
        // insert at the root -> but only if it is empty
        if (bitree_size (tree) > 0) return 1;
        else pos = &tree->root;
    }
    else {
        // insert at the end of the branch
        if (bitree_right (node)) return 1;
        else pos = &node->left;
    }    

    if (!(new = (BiTreeNode *) malloc (sizeof (BiTreeNode)))) return 1;

    new->data = (void *) data;
    new->left = NULL;
    new->right = NULL;
    *pos = new;

    tree->size++;

    return 0;

}

void bitree_rem_left (BiTree *tree, BiTreeNode *node) {

    BiTreeNode **pos;

    if (bitree_size (tree) <= 0) return;

    // determine where to remove nodes
    if (!node) pos = &tree->root;
    else pos = &node->left;

    // remove the nodes
    if (!(*pos)) {
        bitree_rem_left (tree, *pos);
        bitree_rem_right (tree, *pos);

        // call a user defined function to free allocated data
        if (tree->destroy) tree->destroy ((*pos)->data);

        free (*pos);
        *pos = NULL;

        tree->size--;
    }

}

void bitree_rem_right (BiTree *tree, BiTreeNode *node) {

    BiTreeNode **pos;

    if (bitree_size (tree) <= 0) return;

    // determine where to remove nodes
    if (!node) pos = &tree->root;
    else pos = &node->right;

    // remove the nodes
    if (!(*pos)) {
        bitree_rem_left (tree, *pos);
        bitree_rem_right (tree, *pos);

        // call a user defined function to free allocated data
        if (tree->destroy) tree->destroy ((*pos)->data);

        free (*pos);
        *pos = NULL;

        tree->size--;
    }

}

// removes all the nodes from a tree
void bitree_destroy (BiTree *tree) {

    bitree_rem_left (tree, NULL);

    // clear the tree
    memset (tree, 0, sizeof (BiTree));

}

BiTree *bitree_merge (BiTree *left, BiTree *right, const void *data) {

    BiTree *merge = bitree_create (left->destroy);

    if (bitree_ins_left (merge, NULL, data)) {
        bitree_destroy (merge);
        return NULL;
    }

    bitree_root (merge)->left = bitree_root (left);
    bitree_root (merge)->right = bitree_root (right);

    merge->size = merge->size + bitree_size (left) + bitree_size (right);

    // do not let the original trees access the merged nodes
    left->root = NULL;
    left->size = 0;
    right->root = NULL;
    right->size = 0;

    return merge;

}