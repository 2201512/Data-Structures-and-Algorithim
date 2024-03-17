/**
 * @file AVL.cpp
 * @author Yugendren S/O Sooriya Moorthi
 * @brief This file contains the AVL tree class implementation
 * which was largely based on Prof Chek's AVl tree class.
 * It inherits from the BST class as much as possible
 * @date 6 Nov 2023
 */

#include "AVL.h"

/**
 * @brief Add a new value to the tree and balance the tree.
 *        calls the add_ method to add the value to the tree
 *       and then calls the balance method to balance the tree(within the add function)
 *       using the stack of nodes that was created in the add_ method
 * @param value to be added to the tree
 * @throw BSTException if the value already exists in the tree
 */
template <typename T>
void AVL<T>::add(const T& value) {
    //used for balancing the tree after adding
    PathStack pathNodes;//stack of nodes that were traversed to get to the node that was added
    //1. add the value to the tree
    //2. store the path of nodes in the stack
    //3. balance the tree
    add_(this->root(), value, pathNodes);
}

/**
 * @brief Remove a value from the tree and balance the tree.
 *        calls the remove_ method to remove the value from the tree
 *        and then calls the balance method to balance the tree(within the remove function)
 *        using the stack of nodes that was created in the remove_ method
 * @param value to be removed from the tree
 * @throw BSTException if the value does not exist in the tree
 */
template <typename T>
void AVL<T>::remove(const T& value) {
    //used for balancing the tree after removing
    PathStack pathNodes;//stack of nodes that were traversed to get to the node that was removed
    //1. remove the value from the tree
    //2. store the path of nodes in the stack
    //3. balance the tree
    remove_(this->root(), value, pathNodes);
}

/**
 * @brief Print the inorder traversal of the tree.
 * @return stringstream containing the inorder traversal of the tree
 */
template <typename T>
std::stringstream AVL<T>::printInorder() const {
    std::stringstream stringS;
    //call the printInorder_ method to print the inorder traversal of the tree
    printInorder_(this->root(), stringS);
    //return the stringstream
    return stringS;
}

/**
 * @brief Get the height of the tree.
 *        Calls the BST height method
 * @return height of the tree
 */
template <typename T>
int AVL<T>::height() const {
    //call the BST height method
    return BST<T>::height();
}

/**
 * @brief Get the size of the tree.
 *        calls the BST size method
 * @return size of the tree
 */
template <typename T>
unsigned AVL<T>::size() const {
    //call the BST size method
    return BST<T>::size();
}

/**
 * @brief Rotate the tree to the left.
 *Right-Right (RR):
 *Situation Before Rotation:
 *      tree
 *        \
 *     newRoot(child)
 *          \
 *      granchild
 *
 * Situation After Rotation:
 *     newRoot(child)
 *      /          \
 *   tree     grandchild
 * 
 * @param tree to be rotated
 */
template <typename T>
void AVL<T>::rotateLeft(typename BST<T>::BinTree& tree) {
    // check if the tree is null or the right subtree is null.
    if (BST<T>::isEmpty(tree) || BST<T>::isEmpty(tree->right))
        return;
    // Perform a left rotation. u can look at the diagram above for the situation before and after rotation
    typename BST<T>::BinTree newRoot = tree->right;
    tree->right = newRoot->left;
    newRoot->left = tree;
    tree = newRoot;
}

/**
 * @brief Rotate the tree to the Right.
 * Left-Left (LL):
 * Situation Before Rotation:
 *         tree
 *          /
 *    newRoot (child)
 *        /
 *     grandchild
 *
 * Situation After Rotation:
 *     newRoot (child)
 *      /          \
 *   grandchild    tree
 * 
 * @param tree to be rotated
 */
template <typename T>
void AVL<T>::rotateRight(typename BST<T>::BinTree& tree) {
    // check if the tree is null or the left subtree is null.
    if (BST<T>::isEmpty(tree) || BST<T>::isEmpty(tree->left))
        return;
    // Perform a right rotation.
    typename BST<T>::BinTree newRoot = tree->left;
    tree->left = newRoot->right;
    newRoot->right = tree;
    tree = newRoot;
}

/**
 * @brief Rotate the left subtree to the left, then the whole tree to the right.
 *        Uses the rotateLeft and rotateRight methods.
 * Left-Right (LR):
 * Situation Before Rotation:
 *         tree
 *         /
 *     child
 *        \
 *     grandchild
 * 
 * Situation After Rotation:
 *     grandchild
 *      /      \
 *  child     tree
 * 
 * @param tree to be rotated
 */
template <typename T>
void AVL<T>::rotateLeftRight(typename BST<T>::BinTree& tree) {
    // Check if the tree is null or the left subtree is null.
    if (BST<T>::isEmpty(tree) || tree->left == nullptr)
        return;
    // Perform a left-right rotation.
    rotateLeft(tree->left);
    rotateRight(tree);
}

/**
 * @brief Rotate the right subtree to the right, then the whole tree to the left.
 *        Uses the rotateLeft and rotateRight methods.
 * Right-Left (RL):
 * Situation Before Rotation:
 *     tree
 *      \
 *    child
 *      /
 * grandchild
 *
 * Situation After Rotation:
 *     grandchild
 *      /       \
 *    tree    child      
 *
 * @param tree to be rotated
 */
template <typename T>
void AVL<T>::rotateRightLeft(typename BST<T>::BinTree& tree) {
    // Check if the tree is null or the right subtree is null.
    if (BST<T>::isEmpty(tree) || tree->right == nullptr)
        return;
    // Perform a right-left rotation.
    rotateRight(tree->right);
    rotateLeft(tree);
}


/**
 * @brief Balance the tree from the given node.
 *        This checks the balance factors of the node and its' children to 
 *        determine which of LL, LR, RR, RL case they are and call the
 *        appropriate rotation methods above.
 * @param tree to be balanced
 */
template <typename T>
void AVL<T>::balance(typename BST<T>::BinTree& tree) {
    //base case for recursion
    if (BST<T>::isEmpty(tree)) {
        return;
    }
    // Calculate balance factor for the current node.
    updateBalanceFactor(tree);  
    // Check if the tree is unbalanced.
    if (tree->balanceFactor > 1) {
        // Left heavy, perform right rotations.
        updateBalanceFactor(tree->left);
        // Check if the left subtree is left heavy or right heavy.
        if (tree->left->balanceFactor >= 0) {
            // Left-Left case
            rotateRight(tree);
        }
        else {
            // Left-Right case
            rotateLeftRight(tree);
        }
    }
    // Check if the tree is unbalanced.
    else if (tree->balanceFactor < -1) {
        // Right heavy, perform left rotations.
        updateBalanceFactor(tree->right);
        // Check if the right subtree is left heavy or right heavy.
        if (tree->right->balanceFactor <= 0) {
            // Right-Right case
            rotateLeft(tree);
        }
        else {
            // Right-Left case
            rotateRightLeft(tree);
        }
    }

    // Recursively balance the parent nodes
    balance(tree->left);
    balance(tree->right);
}

/**
 * @brief Prints the given tree in inorder traversal.
 *        This is a helper method for the printInorder method
 * @param tree to be printed
 */
template <typename T>
void AVL<T>::printInorder_(const typename BST<T>::BinTree& tree, std::stringstream& ss) const {
    //base case for recursion
    if(BST<T>::isEmpty(tree))
    {
        return;
    }
    //recursive case
    //print the left subtree, root, right subtree
    if (!BST<T>::isEmpty(tree)) {
        // Print the left subtree.
        printInorder_(tree->left, ss);
        // Print the current node.(root of current tree)
        ss << tree->data << " ";
        // Print the right subtree.
        printInorder_(tree->right, ss);
    }
}

/**
 * @brief Update the balance factor of the given tree.
 * @param tree to be updated
 */
template <typename T>
void AVL<T>::updateBalanceFactor(typename BST<T>::BinTree& tree) {
    if (!BST<T>::isEmpty(tree)) {
        // Calculate the height of the left and right subtrees.
        int leftHeight = BST<T>::treeHeight(tree->left);
        int rightHeight = BST<T>::treeHeight(tree->right);
        // Update the balance factor.
        tree->balanceFactor = leftHeight - rightHeight;
    }
}

/**
 * @brief does the actual adding of the value to the tree and creates a stack of nodes
 *       that were traversed to get to the node that was added(for balancing purposes)
 *       This is a helper method for the add method
 * @param tree to be add to, value to be added, stack of nodes that were traversed
 */
template <typename T>
void AVL<T>::add_(typename BST<T>::BinTree& tree, const T& value, PathStack& pathNodes) {
    //base case for recursion
    if (BST<T>::isEmpty(tree)) {    
        //add the value at the an empty tree
        //making a leaf node
        tree = BST<T>::makeNode(value);
        //pathNodes.push(tree);
    } else if (value < tree->data) {
        //value is less than the current node
        pathNodes.push(tree);  // Push a reference to the current node for potential rebalancing.
        add_(tree->left, value, pathNodes);//recursively add to the left subtree
        
    } else if (value > tree->data) {
        //value is greater than the current node
        pathNodes.push(tree);  // Push a reference to the current node for potential rebalancing.
        add_(tree->right, value, pathNodes);//recursively add to the right subtree
        
    } else {
        //value is equal to the current node
        //throw an exception
        throw BSTException(BSTException::E_NOT_FOUND, "Duplicate value found.");
    }
    //balance the tree
    balance(tree);
}

/**
 * @brief does the actual removal of the value to the tree and creates a stack of nodes
 *       that were traversed to get to the node that was removed(for balancing purposes)
 *       This is a helper method for the remove method
 * @param tree to be removed from, value to be removed, stack of nodes that were traversed
 */
template <typename T>
void AVL<T>::remove_(typename BST<T>::BinTree& tree, const T& value, PathStack& pathNodes) {
    //base case for recursion
    if (BST<T>::isEmpty(tree)) {
        // The value is not found in the tree, so nothing to remove.
        throw BSTException(BSTException::E_DUPLICATE, "value does not exist in the tree.");
    }
    // Push a reference to the current node onto pathNodes for potential rebalancing.
    pathNodes.push(tree);

    if (value < tree->data) {
        // Value is in the left subtree.
        remove_(tree->left, value, pathNodes);
    } else if (value > tree->data) {
        // Value is in the right subtree.
        remove_(tree->right, value, pathNodes);

        //value is equal to the current node
    } else {
        // Value is in the current node.
        if (BST<T>::isLeaf(tree)) {
            // The current node is a leaf, so just remove it.
            BST<T>::freeNode(tree);
            tree = nullptr;//set the tree to null
        } else if (tree->left == nullptr) {
            // The current node has only a right child.
            typename BST<T>::BinTree temp = tree;
            tree = tree->right;//set the tree to the right child
            BST<T>::freeNode(temp);
        } else if (tree->right == nullptr) {
            // The current node has only a left child.
            typename BST<T>::BinTree temp = tree;
            tree = tree->left;//set the tree to the left child
            BST<T>::freeNode(temp);
        } else {
            // The current node has both a left and right child.
            typename BST<T>::BinTree predecessor = nullptr;
            BST<T>::findPredecessor(tree, predecessor);//find the predecessor of the current node
            tree->data = predecessor->data;//set the data of the current node to the predecessor's data
            remove_(tree->left, predecessor->data, pathNodes);//remove the predecessor from the left subtree
        }
    }
    //balance the tree
    balance(tree);
}
