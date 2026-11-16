#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int pathHeight(Node* node);


bool equalPaths(Node * root)
{
    // Add your code below
    return pathHeight(root) != -1;
}

// Returns the length (in nodes) of every leaf-to-node path in this subtree,
// or -1 if the leaves in this subtree are at different depths.
int pathHeight(Node* node)
{
    if (node == nullptr) {
        return 0;                       // empty subtree: no leaves, no conflict
    }
    if (node->left == nullptr && node->right == nullptr) {
        return 1;                       // leaf
    }

    int leftH  = pathHeight(node->left);
    int rightH = pathHeight(node->right);

    if (leftH == -1 || rightH == -1) {
        return -1;                      // a mismatch was already found below
    }

    // Only compare when BOTH children exist; a missing child is not a leaf.
    if (node->left != nullptr && node->right != nullptr && leftH != rightH) {
        return -1;
    }

    // If one child is missing its height is 0, so this picks the existing one.
    return 1 + (leftH > rightH ? leftH : rightH);
}