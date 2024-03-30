#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <algorithm>
#include <iostream>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int height(Node * root);

/**
 * @brief Returns true if all paths from leaves to root are the same length (height),
 *        and false otherwise
 *
 *        Note: this doesn't mean we are checking if the tree is full, but just that
 *        any leaf node (wherever it may exist) has the same length path to the root 
 *        as all others.
 * 
 * @param root Pointer to the root of the tree to check for equal paths
 */

bool equalPaths(Node * root)
{
    // Add your code below
    if (root == NULL) {
        return true;
    }
    if (root->left != NULL && root->right != NULL) {
        if (height(root->left) != height(root->right)) {
            return false;
        }
    }
    
    return equalPaths(root->left) && equalPaths(root->right);
}

int height(Node * root) {
    if (root == NULL) {
        return 0;
    }
    return 1 + max(height(root->left), height(root->right));
}

