#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

// Implementation for AVLNode
template<typename Key, typename Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

template<typename Key, typename Value>
AVLNode<Key, Value>::~AVLNode()
{

}

template<typename Key, typename Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

template<typename Key, typename Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

template<typename Key, typename Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}

// Declaration for AVLTree
template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert(const std::pair<const Key, Value>& new_item);
    virtual void remove(const Key& key);

protected:
    AVLNode<Key, Value>* insertNode(AVLNode<Key, Value>* node, AVLNode<Key, Value>* parent, const std::pair<const Key, Value>& new_item);
    void removeNode(AVLNode<Key, Value>* nodeToRemove);
    void rebalance(AVLNode<Key, Value>* node);
    void updateBalance(AVLNode<Key, Value>* node);

    AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rotateLeftRight(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rotateRightLeft(AVLNode<Key, Value>* node);

    int height(const AVLNode<Key, Value>* node) const {
        if (node == nullptr) {
            return -1;
        } 
        else {
            return node->getBalance(); 
        }
    }
};

// Now you would implement the methods like insert, remove, rotateLeft, rotateRight, etc.,
// for the AVLTree class. Since the full implementation is quite lengthy, I’ll outline the structure:

template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value>& new_item) {
    AVLNode<Key, Value>* insertedNode = insertNode(static_cast<AVLNode<Key, Value>*>(this->root_), nullptr, new_item);
    rebalance(insertedNode);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::insertNode(AVLNode<Key, Value>* node, AVLNode<Key, Value>* parent, const std::pair<const Key, Value>& new_item) {
    if (node == nullptr) {
        node = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
        if (parent == nullptr) {
            this->root_ = node;
        } else if (new_item.first < parent->getKey()) {
            parent->setLeft(node);
        } else {
            parent->setRight(node);
        }
        return node;
    } else if (new_item.first < node->getKey()) {
        return insertNode(static_cast<AVLNode<Key, Value>*>(node->getLeft()), node, new_item);
    } else if (new_item.first > node->getKey()) {
        return insertNode(static_cast<AVLNode<Key, Value>*>(node->getRight()), node, new_item);
    } else {
        node->setValue(new_item.second);  // Update the value if key already exists
        return nullptr;  // No need to rebalance since tree structure didn't change
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key) {
    AVLNode<Key, Value>* nodeToRemove = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (nodeToRemove != nullptr) {
        removeNode(nodeToRemove);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeNode(AVLNode<Key, Value>* nodeToRemove) {
    if (nodeToRemove == nullptr) return;

    if (nodeToRemove->getLeft() != nullptr && nodeToRemove->getRight() != nullptr) {
        // Find successor (the smallest node in the right subtree)
        AVLNode<Key, Value>* successor = static_cast<AVLNode<Key, Value>*>(nodeToRemove->getRight());
        while (successor->getLeft() != nullptr) {
            successor = static_cast<AVLNode<Key, Value>*>(successor->getLeft());
        }

        // Swap the node to remove with its successor
        this->nodeSwap(nodeToRemove, successor);

        // Now, successor is in place of nodeToRemove and nodeToRemove is where successor was
        nodeToRemove = successor;
    }

    // Proceed with the removal process, now nodeToRemove has at most one child
    AVLNode<Key, Value>* child = static_cast<AVLNode<Key, Value>*>(nodeToRemove->getLeft() != nullptr ? nodeToRemove->getLeft() : nodeToRemove->getRight());
    
    if (child != nullptr) {
        child->setParent(nodeToRemove->getParent());
    }

    if (nodeToRemove->getParent() == nullptr) {
        this->root_ = child;
    } else if (nodeToRemove == nodeToRemove->getParent()->getLeft()) {
        nodeToRemove->getParent()->setLeft(child);
    } else {
        nodeToRemove->getParent()->setRight(child);
    }

    // Rebalance from the parent of the original position of nodeToRemove
    if (nodeToRemove->getParent()) {
        rebalance(static_cast<AVLNode<Key, Value>*>(nodeToRemove->getParent()));
    }

    delete nodeToRemove;
}

// Add definitions for rotateLeft, rotateRight, rotateLeftRight, rotateRightLeft, rebalance, removeNode, updateHeight here
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* x) {
    // Check if x or its right child is nullptr, indicating that rotation is not possible
    if (x == nullptr || x->getRight() == nullptr) {
        return x; // Return x without performing rotation
    }

    AVLNode<Key, Value>* y = x->getRight(); // Store the right child of x as y

    // Update left child of x and its parent pointer
    x->setRight(y->getLeft());
    if (y->getLeft() != nullptr) {
        y->getLeft()->setParent(x);
    }

    // Update parent pointers to link y properly after rotation
    y->setParent(x->getParent());
    if (x->getParent() == nullptr) {
        this->root_ = y; // If x is the root, y becomes the new root
    } else if (x == x->getParent()->getLeft()) {
        x->getParent()->setLeft(y);
    } else {
        x->getParent()->setRight(y);
    }

    // Perform rotation: x becomes the left child of y
    y->setLeft(x);
    x->setParent(y);

    // Update balance factors after rotation
    updateBalance(x);
    updateBalance(y);

    return y; // Return y, which is now the root of the rotated subtree
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* y) {
    if (y == nullptr || y->getLeft() == nullptr) return y;

    AVLNode<Key, Value>* x = y->getLeft();
    y->setLeft(x->getRight());
    if (x->getRight() != nullptr) {
        x->getRight()->setParent(y);
    }
    x->setParent(y->getParent());
    if (y->getParent() == nullptr) {
        this->root_ = x;
    } else if (y == y->getParent()->getLeft()) {
        y->getParent()->setLeft(x);
    } else {
        y->getParent()->setRight(x);
    }
    x->setRight(y);
    y->setParent(x);

    // Update balance factors
    updateBalance(y);
    updateBalance(x);

    return x;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeftRight(AVLNode<Key, Value>* node) {
    if (node == nullptr || node->getLeft() == nullptr) return node;

    node->setLeft(rotateLeft(node->getLeft()));
    return rotateRight(node);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRightLeft(AVLNode<Key, Value>* node) {
    if (node == nullptr || node->getRight() == nullptr) return node;

    node->setRight(rotateRight(node->getRight()));
    return rotateLeft(node);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* node) {
    while (node != nullptr) {
        updateBalance(node);  // Update balance factor of the current node

        if (node->getBalance() < -1) {  // Right heavy
            if (node->getRight() != nullptr && node->getRight()->getBalance() > 0) {
                // Right-Left Case: Rotate right child right, then rotate node left
                node->setRight(rotateRight(node->getRight()));
            }
            // Rotate left (ZigZigRight case)
            node = rotateLeft(node);
        } else if (node->getBalance() > 1) {  // Left heavy
            if (node->getLeft() != nullptr && node->getLeft()->getBalance() < 0) {
                // Left-Right Case: Rotate left child left, then rotate node right
                node->setLeft(rotateLeft(node->getLeft()));
            }
            // Rotate right
            node = rotateRight(node);
        }

        node = node->getParent();  // Move up the tree
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::updateBalance(AVLNode<Key, Value>* node) {
    if (node != nullptr) {
        int leftHeight = (node->getLeft() != nullptr) ? height(node->getLeft()) : -1;
        int rightHeight = (node->getRight() != nullptr) ? height(node->getRight()) : -1;
        node->setBalance(leftHeight - rightHeight);
    }
}

#endif // AVLBST_H
