/**
 * @file AVL.h
 * @author Yugendren S/O Sooriya Moorthi
 * @brief This file contains the AVL tree class declaration
 * which was largely based on Prof Chek's AVl tree class.
 * It inherits from the BST class as much as possible
 * @date 6 Nov 2023
 */

#ifndef AVL_H
#define AVL_H
#include <iostream>
#include <sstream>
#include <stack>
#include "SimpleAllocator.h"


/**
 * @brief AVL tree class
 *        inherits from BST class
 *        This class is templated to allow for different data types
 *        to be stored in the tree
 * @tparam T Type of data to be stored in the tree
 */
#include "BST.h"
template <typename T>
class AVL : public BST<T> {

public:

    //used for balancing the tree after adding/removing
    // It is used to trace back to the unbalanced node(s) 
    // when balancing the tree after adding/removing a node 
    // current value from stack is the most recent node that was added/removed
    using PathStack = std::stack<typename BST<T>::BinTree>;

    /**
     * @brief Constructor.
     *        The inline implementation here calls the BST constructor.
     * @param allocator Pointer to the allocator to use for the tree.
     */
    AVL(SimpleAllocator* allocator = nullptr) : BST<T>(allocator) {}

    /**
     * @brief Destructor
     *        The inline implementation here calls the BST destructor. 
     */
    virtual ~AVL() override {}

    /**
     * @brief Add a new value to the tree and balance the tree.
     *        calls the add_ method to add the value to the tree
     *       and then calls the balance method to balance the tree(within the add function)
     *       using the stack of nodes that was created in the add_ method
     * @param value to be added to the tree
     * @throw BSTException if the value already exists in the tree
     */
    virtual void add(const T& value) override;

    /**
     * @brief Remove a value from the tree and balance the tree.
     *        calls the remove_ method to remove the value from the tree
     *        and then calls the balance method to balance the tree(within the remove function)
     *        using the stack of nodes that was created in the remove_ method
     * @param value to be removed from the tree
     * @throw BSTException if the value does not exist in the tree
     */
    virtual void remove(const T& value) override;

    /**
     * @brief Print the inorder traversal of the tree.
     * @return stringstream containing the inorder traversal of the tree
     */
    std::stringstream printInorder() const;

    /**
     * @brief Get the height of the tree.
     *        Calls the BST height method
     * @return height of the tree
     */
    int height() const;

    /**
     * @brief Get the size of the tree.
     *        calls the BST size method
     * @return size of the tree
     */
    unsigned size() const;

private:

    /**
     * @brief Rotate the tree to the left.
     * @param tree to be rotated
     */
    void rotateLeft(typename BST<T>::BinTree& tree);

    /**
     * @brief Rotate the tree to the right.
     * @param tree to be rotated
     */
    void rotateRight(typename BST<T>::BinTree& tree);

    /**
     * @brief Rotate the left subtree to the left, then the whole tree to the right.
     *        Uses the rotateLeft and rotateRight methods.
     * @param tree to be rotated
     */
    void rotateLeftRight(typename BST<T>::BinTree& tree);

    /**
     * @brief Rotate the right subtree to the right, then the whole tree to the left.
    *        Uses the rotateLeft and rotateRight methods.
     * @param tree to be rotated
     */
    void rotateRightLeft(typename BST<T>::BinTree& tree);

    /**
     * @brief Balance the tree from the given node.
     *        This checks the balance factors of the node and its' children to 
     *        determine which of LL, LR, RR, RL case they are and call the
     *        appropriate rotation methods above.
     * @param tree to be balanced
     */
    void balance(typename BST<T>::BinTree& tree);

    /**
     * @brief Prints the given tree in inorder traversal.
     *        This is a helper method for the printInorder method
     * @param tree to be printed
     */
    void printInorder_(const typename BST<T>::BinTree& tree, std::stringstream& ss) const;
    
    /**
     * @brief Update the balance factor of the given tree.
     * @param tree to be updated
     */
    void updateBalanceFactor(typename BST<T>::BinTree& tree);
    /**
     * @brief does the actual adding of the value to the tree and creates a stack of nodes
     *       that were traversed to get to the node that was added(for balancing purposes)
     *       This is a helper method for the add method
     * @param tree to be add to, value to be added, stack of nodes that were traversed
     */
    void add_( typename BST<T>::BinTree& tree, const T& value, PathStack& pathNodes);

    /**
     * @brief does the actual removal of the value to the tree and creates a stack of nodes
     *       that were traversed to get to the node that was removed(for balancing purposes)
     *       This is a helper method for the remove method
     * @param tree to be removed from, value to be removed, stack of nodes that were traversed
     */
    void remove_(typename BST<T>::BinTree& tree, const T& value, PathStack& pathNodes);

};

//include the implementation file here
//to allow for templated class
#include "AVL.cpp"

#endif // AVL_H

