#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

bool checkEqualPaths(Node *node, int depth, int &leafDepth) {
    if (node == nullptr) {
        return true;
    }

    if (node->left == nullptr && node->right == nullptr) {
        if (leafDepth == -1) {
            leafDepth = depth;
        } else if (depth != leafDepth) {
            return false;
        }
    }

    return checkEqualPaths(node->left, depth + 1, leafDepth) &&
           checkEqualPaths(node->right, depth + 1, leafDepth);
}

bool equalPaths(Node *root)
{
    // Add your code below
    if (root == nullptr) {
        return true;
    }

    int leafDepth = -1;
    return checkEqualPaths(root, 0, leafDepth);
}
