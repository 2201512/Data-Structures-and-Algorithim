/**
 * @file BST.cpp
 * @author Yugendren S/O Sooriya Moorthi 2201512
 * @brief BST class CPP implementation
 *        adapted to work with an AVL class that inherits this class
 * @date 6 Nov 2023
 */
#include "BST.h"

/**
     * @brief Default constructor
     * @param allocator The allocator to be used
     */
template <typename T>
BST<T>::BST(SimpleAllocator* allocator) : allocator_(allocator), root_(nullptr) {
    if (allocator_ == nullptr) {
        // If no allocator is provided, create one with default configuration
        SimpleAllocatorConfig config;
        // Set other config options is needed
        allocator_ = new SimpleAllocator(sizeof(BinTreeNode), config);
        //isOwnAllocator_ is auto set to false
        isOwnAllocator_ = true; // The BST owns the allocator
    }
}

/**
 * @brief Copy constructor
 * @param rhs The BST to be copied
 */
template <typename T>
BST<T>::BST(const BST& rhs) {
    allocator_ = rhs.allocator_;// Copy the allocator
    isOwnAllocator_ = false; // Not owning the allocator (sharing with the rhs)
    root_ = nullptr; // Initialize the root

    // Copy the elements from rhs tree to this tree
    copy_(root_, rhs.root_);
}

/**
 * @brief Assignment operator
 * @param rhs The BST to be copied
 */
template <typename T>
BST<T>& BST<T>::operator=(const BST& rhs) {
    if (this == &rhs) {
        return *this; // Return itself if rhs is the same as this tree
    }

    // Clean up the current tree
    clear();
    // Copy the elements from rhs tree to this tree
    BST<T> temp(rhs);
    // Swap the root and allocator and isOwnAllocator with the temp tree
    std::swap(root_, temp.root_);
    std::swap(allocator_, temp.allocator_);
    std::swap(isOwnAllocator_, temp.isOwnAllocator_);
    // Return this tree
    return *this;

}

 /**
     * @brief Destructor
     *        It calls clear() to free all nodes
     *        It is virtual so that the destructor of the derived class
     *        will be called when appropriate
     */
template <typename T>
BST<T>::~BST() {
    // Destructor: Clear the tree
    clear();

    if (isOwnAllocator_) {
        // If this tree owns the allocator, release it
        delete allocator_;
    }
}

  /**
 * @brief Subscript operator thatreturns the node at the specified index.
 *        It calls getNode_() to do the actual recursive traversal
 * @param index The index of the node to be returned
 * @return The node at the specified index
 * @throw BSTException if the index is out of range
 */
template <typename T>
const typename BST<T>::BinTreeNode* BST<T>::operator[](int index) const {
    // Get the node at the specified index
    //exception is thrown in getNode_() if index is out of range
    return getNode_(root_, index);
}

/**
 * @brief Insert a value into the tree
 *        It calls add_() to do the actual recursive insertion
 *        It is virtual so that any derived class knows to override it
 * @param value The value to be added
 * @throw BSTException if the value already exists
 */
template <typename T>
void BST<T>::add(const T& value) noexcept(false) {
    // Add a value to the tree using private function
    //exception is thrown in add_() if value already exists
    add_(root_, value);
}

/**
 * @brief Remove a value from the tree
 *        It calls remove_() to do the actual recursive removal
 *        It is virtual so that any derived class knows to override it
 * @param value The value to be removed
 * @throw BSTException if the value does not exist
 */
template <typename T>
void BST<T>::remove(const T& value) {
    // Remove a value from the tree using private function
    //exception is thrown in remove_() if value does not exist
    remove_(root_, value);
}

/**
 * @brief Remove all nodes in the tree
 */
template <typename T>
void BST<T>::clear() {
    while (!isEmpty(root_)) {
        // Remove all elements in the tree
        remove_(root_, root_->data);
    }
}

/**
 * @brief Find a value in the tree
 *        It calls find_() to do the actual recursive search
 * @param value The value to be found
 * @param compares The number of comparisons made 
 *                 (a reference to provide as output)
 * @return true if the value is found
 *         false otherwise
 */
template <typename T>
bool BST<T>::find(const T& value, unsigned& compares) const {
    // Search for a value in the tree and count the number of comparisons
    return find_(root_, value, compares);
}

/**
 * @brief Check if the tree is empty
 * @return true if the tree is empty
 *         false otherwise
 */
template <typename T>
bool BST<T>::empty() const {
    // Check if the tree is empty
    return isEmpty(root_);
}

/**
 * @brief Get the number of nodes in the tree
 * @return The number of nodes in the tree
 */
template <typename T>
unsigned int BST<T>::size() const {
    // Get the size of the tree
    return size_(root_);
}

/**
 * @brief Get the height of the tree
 *        It calls height_() to do the actual recursive calculation
 * @return The height of the tree
 */
template <typename T>
int BST<T>::height() const {
    // Get the height of the tree
    return height_(root_);
}

/**
 * @brief Get the root of the tree
 * @return The root of the tree
 */
template <typename T>
typename BST<T>::BinTree BST<T>::root() const {
    // Get the root of the tree
    return root_;
}

/**
 * @brief Get the root of the tree as a refrence (used for AVL class)
 * @return The root of the tree as a refrence
 */
template <typename T>
typename BST<T>::BinTree& BST<T>::root(){
    // Get the root of the tree
    return root_;
}

/**
 * @brief A recursive step to calculate the height of the tree
 * @param tree The tree to be calculated
 * @return The height of the tree
 */
template <typename T>
int BST<T>::height_(const BinTree& tree) const {
    // Call the treeHeight function (private and recursive)
    return treeHeight(tree);
}

/**
 * @brief Calculate the height of the tree
 * @param tree The tree to be calculated
 */
template <typename T>
int BST<T>::treeHeight(BinTree tree) const {
    // Base case: If the current node is nullptr, the height is -1.
    if (tree == nullptr) {
        return -1;
    }
    // Recursively calculate the height of the left and right subtrees.
    int leftHeight = treeHeight(tree->left);
    int rightHeight = treeHeight(tree->right);
    // Return the maximum height of the left and right subtrees plus 1(root).
    return 1 + std::max(leftHeight, rightHeight);
}

/**
 * @brief Allocate a new node
 * @param value The value to be stored in the new node
 */
template <typename T>
typename BST<T>::BinTree BST<T>::makeNode(const T& value) {
    if (allocator_ == nullptr) {
        //throw exception if allocator is not set
        throw BSTException(BSTException::E_NO_MEMORY, "Allocator is not set");
    }

    // Allocate memory for the node
    BinTree node = static_cast<BinTree>(allocator_->allocate("NodeLabel"));

    if (node == nullptr) {
        //throw exception if allocation fails
        throw BSTException(BSTException::E_NO_MEMORY, "Failed to allocate memory for a new node");
    }

    // Construct the node with the provided value
    new (node) BinTreeNode(value);

    return node;
}

/**
 * @brief Free a node
 * @param node The node to be freed
 */
template <typename T>
void BST<T>::freeNode(BinTree node) {
    if (allocator_ == nullptr) {
        //throw exception if allocator is not set
        throw BSTException(BSTException::E_NO_MEMORY, "Allocator is not set");
    }

    // Call the destructor explicitly
    node->~BinTreeNode();

    // Free the memory
    allocator_->free(node);
}

/**
 * @brief Find the predecessor of a node
 * @param tree The tree to be searched
 * @param predecessor The predecessor of the node
 */
template <typename T>
void BST<T>::findPredecessor(BinTree tree, BinTree& predecessor) const {
    if (tree == nullptr) {
        // If the current node is nullptr, there is no predecessor.
        predecessor = nullptr;
        return;
    }

    if (tree->left == nullptr) {
        // If there is no left child, there is no predecessor.
        predecessor = nullptr;
        return;
    }

    // Traverse to the rightmost node in the left subtree to find the predecessor.
    tree = tree->left;
    while (tree->right != nullptr) {
        tree = tree->right;
    }
    predecessor = tree;
}

/**
 * @brief Find the succesor of a node
 * @param tree The tree to be searched
 * @param successor The succesor of the node
 */ 
template <typename T>
void BST<T>::findSuccessor(BinTree tree, BinTree& successor) const {
    if (tree == nullptr) {
        // If the current node is nullptr, there is no successor.
        successor = nullptr;
        return;
    }

    if (tree->right == nullptr) {
        // If there is no right child, there is no successor.
        successor = nullptr;
        return;
    }

    // Traverse to the leftmost node in the right subtree to find the successor.
    tree = tree->right;
    while (tree->left != nullptr) {
        tree = tree->left;
    }
    successor = tree;
}

/**
 * @brief Check if the tree is empty
 * @param tree The tree to be checked
 * @return true if the tree is empty
 *         false otherwise
 */
template <typename T>
bool BST<T>::isEmpty(BinTree& tree) const {
    // Check if a tree node is empty
    return tree == nullptr;
}

/**
 * @brief Check if the tree is empty (const)
 * @param tree The tree to be checked
 * @return true if the tree is empty
 *         false otherwise
 */
template <typename T>
bool BST<T>::isEmpty(const BinTree& tree) const {
    // Check if a tree node is empty (const version)
    return tree == nullptr;
}

/**
 * @brief Check if the tree is a leaf
 * @param tree The tree to be checked
 * @return true if the tree is a leaf
 *         false otherwise
 */
template <typename T>
bool BST<T>::isLeaf(const BinTree& tree) const {
    // Check if a tree node is a leaf node
    return tree != nullptr && tree->left == nullptr && tree->right == nullptr;
}

/**
 * @brief A recursive step to add a value into the tree
 * @param tree The tree to be added
 * @param value The value to be added
 */
template <typename T>
void BST<T>::add_(BinTree& tree, const T& value) {
    //base case if tree is empty
    if (tree == nullptr) // Base case: Insert a new node
    {
        tree = makeNode(value);
        tree->count = 1;  // The current node has one element
        return;
    }
    // Recursive case: Traverse the tree.
    if (value < tree->data) {
        // If the value is less than the current node's data, search in the left subtree
        add_(tree->left, value);
    } else if (value > tree->data) {
        // If the value is greater than the current node's data, search in the right subtree.
        add_(tree->right, value);
    } else {
        // If the value is equal to the current node's data, throw an exception.
        throw BSTException(BSTException::E_DUPLICATE, "Duplicate value not allowed");
    }

    if (tree != nullptr) {
        // Update the count for the current node
        tree->count = size_(tree->left) + size_(tree->right) + 1;
    }
}

/**
 * @brief A recursive step to find the value in the tree
 * @param tree The tree to be searched
 * @param value The value to be found
 * @param compares The number of comparisons made
 *                (a reference to provide as output)
 *                Note that it is not const as it is used to provide output
 * @return true if the value is found
 */
template <typename T>
bool BST<T>::find_(const BinTree& tree, const T& value, unsigned& compares) const {
    // Base case: If the current node is nullptr, the value is not found.
    if (tree == nullptr) {
        // If the current node is nullptr, the value is not found.
        return false;
    }
    // Increment the number of comparisons.
    compares++;
    if (value == tree->data) {
        // If the value matches the current node's data, it is found.
        return true;
    } else if (value < tree->data) {
        // If the value is less than the current node's data, search in the left subtree.
        return find_(tree->left, value, compares);
    } else {
        // If the value is greater than the current node's data, search in the right subtree.
        return find_(tree->right, value, compares);
    }
}

/**
 * @brief A recursive step to get to the node at the specified index
 *        This is used by operator[]
 * @param tree The tree to be traversed
 * @param index The index of the node to be returned
 * @return The node at the specified index
 * @throw BSTException if the index is out of range
 */
template <typename T>
const typename BST<T>::BinTree BST<T>::getNode_(const BinTree& tree, int index) const {
    if (tree == nullptr) {
        // If the current node is nullptr, the index is out of bounds.
        throw BSTException(BSTException::E_OUT_BOUNDS, "Index out of bounds");
    }

    int leftSize = size_(tree->left);
    if (index < leftSize) {
        // If the index is within the left subtree, recursively search in the left subtree.
        return getNode_(tree->left, index);
    } else if (index == leftSize) {
        // If the index matches the size of the left subtree, return the current node.
        return tree;
    } else {
        // If the index is in the right subtree, adjust the index and search in the right subtree.
        return getNode_(tree->right, index - leftSize - 1);
    }
}

/**
 * @brief A recursive step to calculate the size of the tree
 * @param tree The tree to be calculated
 * @return The size of the tree
 */
template <typename T>
unsigned BST<T>::size_(const BinTree& tree) const {
    // Base case: If the current node is nullptr, the size is 0.
    if (tree == nullptr) {
        return 0;
    }
    // Recursively calculate the size of the left and right subtrees.
    return 1 + size_(tree->left) + size_(tree->right);
}

/**
 * @brief A recursive step to remove a value from the tree
 * @param tree The tree to be removed
 * @param value The value to be removed
 */
template <typename T>
void BST<T>::remove_(BinTree& tree, const T& value) {
    // Base case: If the current node is nullptr, the value is not found.
    if (tree == nullptr) {
        throw BSTException(BSTException::E_NOT_FOUND, "Value to remove not found in the tree");
    }
    // Recursive case: Traverse the tree.
    if (value < tree->data) {
        // If the value is less than the current node's data, search in the left subtree.
        remove_(tree->left, value);
    } else if (value > tree->data) {
        // If the value is greater than the current node's data, search in the right subtree.
        remove_(tree->right, value);
    } else {
        if (isLeaf(tree)) {
            // If the current node is a leaf node, remove it.
            freeNode(tree);
            tree = nullptr;
        } else if (tree->left == nullptr) {
            // If there is no left child, replace the current node with the right child.
            BinTree temp = tree;
            tree = tree->right;
            freeNode(temp);
        } else if (tree->right == nullptr) {
            // If there is no right child, replace the current node with the left child.
            BinTree temp = tree;
            tree = tree->left;
            freeNode(temp);
        } else {
            // If there are both left and right children, replace the current node with the predecessor.
            BinTree predecessor = nullptr;
            findPredecessor(tree, predecessor);
            tree->data = predecessor->data;
            remove_(tree->left, predecessor->data);
        }
    }
}

/**
 * @brief A recursive step to copy the tree 
 * @param tree The tree to be copied
 * @param rtree The tree to be copied to
 */
template <typename T>
void BST<T>::copy_(BinTree& tree, const BinTree& rtree) {
    // Base case: If the right tree is nullptr, there's nothing to copy.
    if (rtree == nullptr) {
        // If the right tree is nullptr, there's nothing to copy.
        return;
    }

    // Create a new node in the current tree with the same data as the right tree.
    tree = makeNode(rtree->data);
    tree->count = rtree->count;

    // Recursively copy the left and right subtrees.
    copy_(tree->left, rtree->left);
    copy_(tree->right, rtree->right);
}

