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
    void insertFix (AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
    void rotateRight (AVLNode<Key, Value>* z);
    void rotateLeft (AVLNode<Key, Value>* z);
    void removeFix(AVLNode<Key, Value>* n, int8_t diff);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    AVLNode<Key, Value>* current_node = static_cast<AVLNode<Key, Value>*>(this->root_);
    if (BinarySearchTree<Key, Value>::empty()) {
        AVLNode<Key, Value>* new_node = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        this->root_ = new_node;
        new_node->setBalance(0);
        return;
    }
    else {
        //BinarySearchTree<Key, Value>::insert(new_item);
        AVLNode<Key, Value>* parent_node = NULL;
        while (current_node != NULL) {
            parent_node = current_node;
            if (new_item.first == current_node->getKey()) { //key are same
                current_node->setValue(new_item.second); //update value
                return;
            }
            else if (new_item.first < current_node->getKey()) {
                current_node = current_node->getLeft();
            }
            else if (new_item.first > current_node->getKey()) {
                current_node = current_node->getRight();
            }
            
        }
        
        //update parent node's left or right child
        if (new_item.first < parent_node->getKey()) {
            AVLNode<Key, Value>* new_node = new AVLNode<Key, Value>(new_item.first, new_item.second, parent_node); //dynamically create new node
            parent_node->setLeft(new_node);
        }
        else if (new_item.first > parent_node->getKey()) {
            AVLNode<Key, Value>* new_node = new AVLNode<Key, Value>(new_item.first, new_item.second, parent_node); //dynamically create new node
            parent_node->setRight(new_node); 
        }
        //finish pasted code
        AVLNode<Key, Value> *new_node = static_cast<AVLNode<Key, Value>*>(this->internalFind(new_item.first));
        new_node->setBalance(0);
        if(new_node->getParent()->getBalance() == 1 || new_node->getParent()->getBalance() == -1) { //parent balance was +- 1
            new_node->getParent()->setBalance(0);
            return;
        }
        else if (new_node->getParent()->getBalance() == 0) { //parent balance was 0
            //update parent balance
            if (new_node->getParent()->getLeft() == NULL) { //new node was right
                new_node->getParent()->setBalance(1);
            }
            else { //new node was left
                new_node->getParent()->setBalance(-1);
            }
            //call insert fix
            insertFix(new_node->getParent(), new_node);
        } 
    }
}

//insert fix helper function
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix (AVLNode<Key, Value>* p, AVLNode<Key, Value>* n) {
    if (p == NULL || p->getParent() == NULL) {
        return;
    }
    AVLNode<Key, Value> *g = p->getParent();
    if (g->getLeft() == p) { //p is left child of g
        g->updateBalance(-1);
        if (g->getBalance() == 0) { //case 1: balance is 0
            return;
        }
        else if (g->getBalance() == -1) { //case 2: balance is -1
            insertFix(g, p);
        }
        else { //case 3:balance is -2
            if (p->getLeft() == n) { //zig zig
                rotateRight(g);
                p->setBalance(0);
                g->setBalance(0);
            }
            else { //zig zag
                rotateLeft(p);
                rotateRight(g);
                if (n->getBalance() == -1) { //case 3a
                    p->setBalance(0);
                    g->setBalance(1);
                    n->setBalance(0);
                }
                else if (n->getBalance() == 0) { //case 3b
                    p->setBalance(0);
                    g->setBalance(0);
                    n->setBalance(0);
                }
                else { //case 3c
                    p->setBalance(-1);
                    g->setBalance(0);
                    n->setBalance(0);
                }
            }
        }   
    }
    else { //p is right child of g
        g->updateBalance(1);
        if (g->getBalance() == 0) { //case 1: balance is 0
            return;
        }
        else if (g->getBalance() == 1) { //case 2: balance is 1
            insertFix(g, p);
        }
        else { //case 3:balance is 2
            if (p->getRight() == n) { //zig zig
                rotateLeft(g);
                p->setBalance(0);
                g->setBalance(0);
            }
            else { //zig zag
                rotateRight(p);
                rotateLeft(g);
                if (n->getBalance() == 1) { //case 3a
                    p->setBalance(0);
                    g->setBalance(-1);
                    n->setBalance(0);
                }
                else if (n->getBalance() == 0) { //case 3b
                    p->setBalance(0);
                    g->setBalance(0);
                    n->setBalance(0);
                }
                else { //case 3c
                    p->setBalance(1);
                    g->setBalance(0);
                    n->setBalance(0);
                }
            }
            return;
        }
    }
}

//helper function
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight (AVLNode<Key, Value>* z) {
    if (z == NULL || z->getLeft() == NULL) {
        return;
    }
    
    AVLNode<Key, Value> *y = z->getLeft();
    AVLNode<Key, Value> *p = z->getParent();
    AVLNode<Key, Value> *c = NULL;

    if (y != NULL) {
        c = y->getRight();
        z->setParent(y);
        y->setRight(z);
    }

    z->setLeft(c);
    if (c != NULL) {
        c->setParent(z);
    }

    //change p's child from z to y
    if (p != NULL) {
        y->setParent(p);
        if (p->getRight() == z) {
            p->setRight(y);
        }
        else {
            p->setLeft(y);
        }
    }
    else {
        y->setParent(NULL);
        this->root_ = y;
    }

}

//helper function
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft (AVLNode<Key, Value>* z) {
    if (z == NULL || z->getRight() == NULL) {
        return;
    }
    
    AVLNode<Key, Value> *y = z->getRight();
    AVLNode<Key, Value> *p = z->getParent();
    AVLNode<Key, Value> *c = NULL;

    if (y != NULL) {
        c = y->getLeft();
        z->setParent(y);
        y->setLeft(z);
    }

    z->setRight(c);
    if (c != NULL) {
        c->setParent(z);
    }

    //change p's child from z to y
    if (p != NULL) {
        y->setParent(p);
        if (p->getLeft() == z) {
            p->setLeft(y);
        }
        else {
            p->setRight(y);
        }
    }
    else {
        y->setParent(NULL);
        this->root_ = y;
    }

    /*
    if (x == NULL) {
        return;
    }
    AVLNode<Key, Value> *y = x->getRight();
    AVLNode<Key, Value> *b = y->getLeft();
    AVLNode<Key, Value> *p = y->getParent();
    
    //change p's child from x to y
    if (p->getRight() == x) {
        p->setRight(y);
    }
    else {
        p->setLeft(y);
    }
    y->setParent(p);
    x->setParent(y);
    x->setRight(b);
    y->setLeft(x);
    b->setParent(x);
    */
}

/*
 * Recall: The writeup specifies that if a node has 2 children you 
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    if (this->root_ == NULL) {
        return;
    }
    AVLNode<Key, Value> *removed_node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key)); 
    if (removed_node == NULL) {
        return;
    }
    int8_t diff;
    AVLNode<Key, Value> *removed_node_parent;

    //BinarySearchTree<Key, Value>::remove(key);
    AVLNode<Key, Value>* nodeToRemove = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (nodeToRemove->getLeft() == NULL && nodeToRemove->getRight() == NULL) { //0 children
        removed_node_parent = removed_node->getParent(); 
        if (removed_node_parent != NULL) { //parent of removed node exists    
            if (removed_node_parent->getLeft() == removed_node) { //removed node is left child
                diff = 1;
            }
            else { //removed node is right child
                diff = -1;
            }
        }
        if (nodeToRemove == this->root_) {
            this->root_ = nullptr; 
        } 
        else {
            if (nodeToRemove->getParent()->getLeft() == nodeToRemove) {
                nodeToRemove->getParent()->setLeft(nullptr);
            } 
            else {
                nodeToRemove->getParent()->setRight(nullptr);
            }
        }
        delete nodeToRemove;
    }
    else if (nodeToRemove->getLeft() == NULL || nodeToRemove->getRight() == NULL){ //1 child
        removed_node_parent = removed_node->getParent(); 
        if (removed_node_parent != NULL) { //parent of removed node exists    
            if (removed_node_parent->getLeft() == removed_node) { //removed node is left child
                diff = 1;
            }
            else { //removed node is right child
                diff = -1;
            }
        }
        AVLNode<Key, Value>* child;
        if (nodeToRemove->getLeft() == nullptr) {
            child = nodeToRemove->getRight();
        } 
        else {
            child = nodeToRemove->getLeft();
        }

        if (nodeToRemove == this->root_) {
            this->root_ = child; 
            child->setParent(nullptr);
        } 
        else {
            if (nodeToRemove->getParent()->getLeft() == nodeToRemove) {
                nodeToRemove->getParent()->setLeft(child);
            } else {
                nodeToRemove->getParent()->setRight(child);
            }
            child->setParent(nodeToRemove->getParent());
        }
        delete nodeToRemove;
    }
    else { //2 children
        nodeSwap(nodeToRemove,static_cast<AVLNode<Key,Value>*>(this->predecessor(nodeToRemove))); //swap node with predecessor
        //then it will fall in one of the above 2 cases
        removed_node_parent = removed_node->getParent(); 
        if (removed_node_parent != NULL) { //parent of removed node exists    
            if (removed_node_parent->getLeft() == removed_node) { //removed node is left child
                diff = 1;
            }
            else { //removed node is right child
                diff = -1;
            }
        }
        if (nodeToRemove->getLeft() == NULL && nodeToRemove->getRight() == NULL) { //0 children
            if (nodeToRemove == this->root_) {
                this->root_ = nullptr; 
            } 
            else {
                if (nodeToRemove->getParent()->getLeft() == nodeToRemove) {
                    nodeToRemove->getParent()->setLeft(nullptr);
                } 
                else {
                    nodeToRemove->getParent()->setRight(nullptr);
                }
            }
            delete nodeToRemove;
        }
        else if (nodeToRemove->getLeft() == NULL || nodeToRemove->getRight() == NULL){ //1 child
            Node<Key, Value>* child;
            if (nodeToRemove->getLeft() == nullptr) {
                child = nodeToRemove->getRight();
            } 
            else {
                child = nodeToRemove->getLeft();
            }

            if (nodeToRemove == this->root_) {
                this->root_ = child; 
                child->setParent(nullptr);
            } 
            else {
                if (nodeToRemove->getParent()->getLeft() == nodeToRemove) {
                    nodeToRemove->getParent()->setLeft(child);
                } else {
                    nodeToRemove->getParent()->setRight(child);
                }
                child->setParent(nodeToRemove->getParent());
            }
            delete nodeToRemove;
        }
    }


    std::cout << "removing " << key << std::endl;
    std::cout << "before fix" << std::endl;
    this->print();
    removeFix(removed_node_parent, diff);

}

//remove helper function
template<class Key, class Value>
void AVLTree<Key, Value>:: removeFix(AVLNode<Key, Value>* n, int8_t diff) {
    //if n is null, return
    if (n == NULL) {
        return;
    }
    //compute next recursive call's arguments now before altering tree
    AVLNode<Key, Value>* p = n->getParent();
    int8_t nextdiff;
    if (p != NULL) {
        if (p->getLeft() == n) { //if n is left child next diff = 1
            nextdiff = 1;
        }
        else { //if n is right child next diff = -1
            nextdiff = -1;
        }
    }
    //diff = -1
    if (diff == -1) {
        std::cout << n->getKey() << std::endl;
        if (n->getBalance() + diff == -2) { //case 1
            std::cout << "-2 case" << std::endl;
            AVLNode<Key, Value>* c = n->getLeft();
            if (c->getBalance() == -1) { //case 1a
                rotateRight(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, nextdiff);
            }
            else if (c->getBalance() == 0) { //case 1b
                rotateRight(n);
                n->setBalance(-1);
                c->setBalance(1);
                return;
            }
            else { //case 1c
                AVLNode<Key, Value>* g = c->getRight();
                rotateLeft(c);
                rotateRight(n);
                if (g->getBalance() == 1) {
                    n->setBalance(0);
                    c->setBalance(-1);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 0) {
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else {
                    n->setBalance(1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(p, nextdiff);
            }
        }
        else if (n->getBalance() + diff == -1) { //case 2
            std::cout << "-1 case" << std::endl;
            n->setBalance(-1);
            return;
        }
        else { //case 3
            std::cout << "0 case" << std::endl;
            n->setBalance(0);
            removeFix(p, nextdiff);
        }
    }
    //diff = 1
    else{
        if (n->getBalance() + diff == 2) { //case 1
            std::cout << "2 case" << std::endl;
            AVLNode<Key, Value>* c = n->getRight();
            if (c->getBalance() == 1) { //case 1a
                rotateLeft(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, nextdiff);
            }
            else if (c->getBalance() == 0) { //case 1b
                rotateLeft(n);
                n->setBalance(1);
                c->setBalance(-1);
                return;
            }
            else { //case 1c
                AVLNode<Key, Value>* g = c->getLeft();
                rotateRight(c);
                rotateLeft(n);
                if (g->getBalance() == -1) {
                    n->setBalance(0);
                    c->setBalance(1);
                    g->setBalance(0);
                }
                else if (g->getBalance() == 0) {
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else {
                    n->setBalance(-1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(p, nextdiff);
            }
        }
        else if (n->getBalance() + diff == 1) { //case 2
            std::cout << "1 case" << std::endl;
            n->setBalance(1);
            return;
        }
        else { //case 
            std::cout << " right 0 case" << std::endl;
            n->setBalance(0);
            removeFix(p, nextdiff);
        }
    }
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
