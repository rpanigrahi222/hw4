#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key, Value>* x);
    void rotateRight(AVLNode<Key, Value>* x);
    void insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
    void removeFix(AVLNode<Key, Value>* n, int8_t diff);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    const Key& key = new_item.first;

    // Empty tree
    if (this->root_ == NULL) {
        this->root_ = new AVLNode<Key, Value>(key, new_item.second, NULL);
        return;
    }

    // Normal BST walk down
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = NULL;
    while (curr != NULL) {
        if (key == curr->getKey()) {
            curr->setValue(new_item.second);   // already there: overwrite
            return;
        }
        parent = curr;
        curr = (key < curr->getKey()) ? curr->getLeft() : curr->getRight();
    }

    // Attach the new node (balance starts at 0)
    AVLNode<Key, Value>* n = new AVLNode<Key, Value>(key, new_item.second, parent);
    if (key < parent->getKey()) {
        parent->setLeft(n);
    }
    else {
        parent->setRight(n);
    }

    // If parent was -1 or +1, it is now 0 and its height didn't change: done
    if (parent->getBalance() != 0) {
        parent->setBalance(0);
        return;
    }

    // Parent was 0: it leans toward n now, and its height grew by 1
    parent->updateBalance(parent->getLeft() == n ? -1 : 1);
    insertFix(parent, n);
}

// p's height just grew by 1 (n is p's child on the grown side).
// Walk up updating grandparent g; rotate at most once.
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n)
{
    if (p == NULL) return;
    AVLNode<Key, Value>* g = p->getParent();
    if (g == NULL) return;

    if (g->getLeft() == p) {                  // p is LEFT child of g
        g->updateBalance(-1);
        if (g->getBalance() == 0) return;     // g got evened out: done
        if (g->getBalance() == -1) {          // g grew too: keep going up
            insertFix(g, p);
            return;
        }
        // g is -2: out of balance
        if (p->getLeft() == n) {              // zig-zig (left-left)
            rotateRight(g);
            p->setBalance(0);
            g->setBalance(0);
        }
        else {                                // zig-zag (left-right)
            rotateLeft(p);
            rotateRight(g);
            if (n->getBalance() == -1)      { p->setBalance(0);  g->setBalance(1); }
            else if (n->getBalance() == 0)  { p->setBalance(0);  g->setBalance(0); }
            else                            { p->setBalance(-1); g->setBalance(0); }
            n->setBalance(0);
        }
    }
    else {                                    // p is RIGHT child of g (mirror)
        g->updateBalance(1);
        if (g->getBalance() == 0) return;
        if (g->getBalance() == 1) {
            insertFix(g, p);
            return;
        }
        // g is +2
        if (p->getRight() == n) {             // zig-zig (right-right)
            rotateLeft(g);
            p->setBalance(0);
            g->setBalance(0);
        }
        else {                                // zig-zag (right-left)
            rotateRight(p);
            rotateLeft(g);
            if (n->getBalance() == 1)       { p->setBalance(0); g->setBalance(-1); }
            else if (n->getBalance() == 0)  { p->setBalance(0); g->setBalance(0); }
            else                            { p->setBalance(1); g->setBalance(0); }
            n->setBalance(0);
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    AVLNode<Key, Value>* n =
        static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (n == NULL) return;

    // Two children: swap NODES with predecessor (AVL nodeSwap also swaps balances)
    if (n->getLeft() != NULL && n->getRight() != NULL) {
        AVLNode<Key, Value>* pred =
            static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(n));
        nodeSwap(n, pred);
    }

    // n now has 0 or 1 child
    AVLNode<Key, Value>* child = (n->getLeft() != NULL) ? n->getLeft() : n->getRight();
    AVLNode<Key, Value>* parent = n->getParent();

    // Which side of parent loses height? left side shrinks -> balance +1
    int8_t diff = 0;
    if (parent != NULL) {
        diff = (parent->getLeft() == n) ? 1 : -1;
    }

    // Splice n out
    if (child != NULL) {
        child->setParent(parent);
    }
    if (parent == NULL) {
        this->root_ = child;
    }
    else if (parent->getLeft() == n) {
        parent->setLeft(child);
    }
    else {
        parent->setRight(child);
    }
    delete n;

    removeFix(parent, diff);
}

// One of n's subtrees just got 1 shorter. diff = +1 if it was the left
// subtree, -1 if the right. May rotate at every level up to the root.
template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int8_t diff)
{
    if (n == NULL) return;

    // Figure out the next call's info BEFORE rotating (rotations move n)
    AVLNode<Key, Value>* p = n->getParent();
    int8_t ndiff = 0;
    if (p != NULL) {
        ndiff = (p->getLeft() == n) ? 1 : -1;
    }

    if (diff == -1) {                          // right side shrank
        if (n->getBalance() + diff == -2) {    // now too left-heavy
            AVLNode<Key, Value>* c = n->getLeft();   // taller child
            if (c->getBalance() == -1) {             // zig-zig
                rotateRight(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);                 // height shrank: keep going
            }
            else if (c->getBalance() == 0) {         // zig-zig, height unchanged
                rotateRight(n);
                n->setBalance(-1);
                c->setBalance(1);
                // done
            }
            else {                                   // zig-zag
                AVLNode<Key, Value>* g = c->getRight();
                rotateLeft(c);
                rotateRight(n);
                if (g->getBalance() == 1)       { n->setBalance(0);  c->setBalance(-1); }
                else if (g->getBalance() == 0)  { n->setBalance(0);  c->setBalance(0); }
                else                            { n->setBalance(1);  c->setBalance(0); }
                g->setBalance(0);
                removeFix(p, ndiff);
            }
        }
        else if (n->getBalance() + diff == -1) {     // was 0: height unchanged
            n->setBalance(-1);
        }
        else {                                       // was +1: now 0, height shrank
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
    else {                                     // diff == +1: left side shrank (mirror)
        if (n->getBalance() + diff == 2) {
            AVLNode<Key, Value>* c = n->getRight();
            if (c->getBalance() == 1) {
                rotateLeft(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else if (c->getBalance() == 0) {
                rotateLeft(n);
                n->setBalance(1);
                c->setBalance(-1);
            }
            else {
                AVLNode<Key, Value>* g = c->getLeft();
                rotateRight(c);
                rotateLeft(n);
                if (g->getBalance() == -1)      { n->setBalance(0);  c->setBalance(1); }
                else if (g->getBalance() == 0)  { n->setBalance(0);  c->setBalance(0); }
                else                            { n->setBalance(-1); c->setBalance(0); }
                g->setBalance(0);
                removeFix(p, ndiff);
            }
        }
        else if (n->getBalance() + diff == 1) {
            n->setBalance(1);
        }
        else {
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
}

// x's right child y moves up; x becomes y's left child.
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* x)
{
    AVLNode<Key, Value>* y = x->getRight();
    AVLNode<Key, Value>* parent = x->getParent();

    // y's left subtree becomes x's right subtree
    x->setRight(y->getLeft());
    if (y->getLeft() != NULL) {
        y->getLeft()->setParent(x);
    }

    // hook y into x's old spot
    y->setParent(parent);
    if (parent == NULL) {
        this->root_ = y;
    }
    else if (parent->getLeft() == x) {
        parent->setLeft(y);
    }
    else {
        parent->setRight(y);
    }

    y->setLeft(x);
    x->setParent(y);
}

// x's left child y moves up; x becomes y's right child.
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* x)
{
    AVLNode<Key, Value>* y = x->getLeft();
    AVLNode<Key, Value>* parent = x->getParent();

    x->setLeft(y->getRight());
    if (y->getRight() != NULL) {
        y->getRight()->setParent(x);
    }

    y->setParent(parent);
    if (parent == NULL) {
        this->root_ = y;
    }
    else if (parent->getLeft() == x) {
        parent->setLeft(y);
    }
    else {
        parent->setRight(y);
    }

    y->setRight(x);
    x->setParent(y);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif