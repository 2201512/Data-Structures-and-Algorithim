#include "BST.h"

template <typename T>
BST<T>::BST(SimpleAllocator* allocator) : allocator_(allocator), root_(nullptr) {
    if (allocator_ == nullptr) {
        // If no allocator is provided, create one with default configuration
        SimpleAllocatorConfig config;
        config.useCPPMemManager = true; // Use C++ memory manager
        config.objectsPerPage = 100;   // Adjust as needed
        // Set other config options here

        allocator_ = new SimpleAllocator(sizeof(BinTreeNode), config);
        isOwnAllocator_ = true; // The BST owns the allocator
    }
}

template <typename T>
BST<T>::BST(const BST& rhs) {
    allocator_ = rhs.allocator_;
    isOwnAllocator_ = false; // Not owning the allocator (sharing with the rhs)
    root_ = nullptr; // Initialize the root

    // Copy the elements from rhs tree to this tree
    copy_(root_, rhs.root_);
}

template <typename T>
BST<T>& BST<T>::operator=(const BST& rhs) {
    if (this == &rhs) {
        return *this; // Return itself if rhs is the same as this tree
    }

    // Clean up the current tree
    clear();

    allocator_ = rhs.allocator_;
    isOwnAllocator_ = false; // Not owning the allocator (sharing with rhs)
    root_ = nullptr; // Initialize the root

    // Copy the elements from rhs tree to this tree
    copy_(root_, rhs.root_);
    return *this;
}

template <typename T>
BST<T>::~BST() {
    // Destructor: Clear the tree
    clear();

    if (isOwnAllocator_) {
        // If this tree owns the allocator, release it
        delete allocator_;
    }
}

template <typename T>
const typename BST<T>::BinTreeNode* BST<T>::operator[](int index) const {
    return getNode_(root_, index);
}

template <typename T>
void BST<T>::add(const T& value) noexcept(false) {
    add_(root_, value);
}

template <typename T>
void BST<T>::remove(const T& value) {
    remove_(root_, value);
}

template <typename T>
void BST<T>::clear() {
    while (!isEmpty(root_)) {
        // Remove all elements in the tree
        remove_(root_, root_->data);
    }
}

template <typename T>
bool BST<T>::find(const T& value, unsigned& compares) const {
    // Search for a value in the tree and count the number of comparisons
    return find_(root_, value, compares);
}

template <typename T>
bool BST<T>::empty() const {
    // Check if the tree is empty
    return isEmpty(root_);
}

template <typename T>
unsigned int BST<T>::size() const {
    // Get the size of the tree
    return size_(root_);
}

template <typename T>
int BST<T>::height() const {
    // Get the height of the tree
    return height_(root_);
}

template <typename T>
typename BST<T>::BinTree BST<T>::root() const {
    // Get the root of the tree
    return root_;
}

template <typename T>
typename BST<T>::BinTree BST<T>::makeNode(const T& value) {
    if (allocator_ == nullptr) {
        throw BSTException(BSTException::E_NO_MEMORY, "Allocator is not set");
    }

    // Allocate memory for the node
    BinTree node = static_cast<BinTree>(allocator_->allocate("NodeLabel"));

    if (node == nullptr) {
        throw BSTException(BSTException::E_NO_MEMORY, "Failed to allocate memory for a new node");
    }

    // Construct the node with the provided value
    new (node) BinTreeNode(value);

    return node;
}

template <typename T>
void BST<T>::freeNode(BinTree node) {
    if (allocator_ == nullptr) {
        throw BSTException(BSTException::E_NO_MEMORY, "Allocator is not set");
    }

    // Call the destructor explicitly
    node->~BinTreeNode();

    // Free the memory
    allocator_->free(node);
}

template <typename T>
int BST<T>::treeHeight(BinTree tree) const {
    if (root_ == nullptr) {
        return -1; // If the tree is empty, its height is -1 (no nodes)
    }
    if (tree == nullptr || (tree->left == nullptr && tree->right == nullptr)) {
        return 0; // If the current node is a leaf or the tree is empty, its height is 0
    }

    // Calculate the height of the left and right subtrees recursively
    int leftHeight = treeHeight(tree->left);
    int rightHeight = treeHeight(tree->right);

    // Return the maximum height between the left and right subtrees, plus 1 for the current node
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}


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


template <typename T>
bool BST<T>::isEmpty(BinTree& tree) const {
    // Check if a tree node is empty
    return tree == nullptr;
}

template <typename T>
bool BST<T>::isEmpty(const BinTree& tree) const {
    // Check if a tree node is empty (const version)
    return tree == nullptr;
}

template <typename T>
bool BST<T>::isLeaf(const BinTree& tree) const {
    // Check if a tree node is a leaf node
    return tree != nullptr && tree->left == nullptr && tree->right == nullptr;
}

template <typename T>
void BST<T>::add_(BinTree& tree, const T& value) {
    if (tree == nullptr) // Base case: Insert a new node
    {
        tree = makeNode(value);
        tree->count = 1;  // The current node has one element
        return;
    }
    // Recursive case: Traverse the tree
    if (value < tree->data) {
        add_(tree->left, value);
    } else if (value > tree->data) {
        add_(tree->right, value);
    } else {
        throw BSTException(BSTException::E_DUPLICATE, "Duplicate value not allowed");
    }

    if (tree != nullptr) {
        // Update the count for the current node
        tree->count = size_(tree->left) + size_(tree->right) + 1;
    }
}

template <typename T>
bool BST<T>::find_(const BinTree& tree, const T& value, unsigned& compares) const {
    if (tree == nullptr) {
        // If the current node is nullptr, the value is not found.
        return false;
    }

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

template <typename T>
unsigned BST<T>::size_(const BinTree& tree) const {
    if (tree == nullptr) {
        // If the current node is nullptr, it has no elements, so the size is 0.
        return 0;
    }
    return tree->count;
}


template <typename T>
void BST<T>::remove_(BinTree& tree, const T& value) {
    if (tree == nullptr) {
        // If the current node is nullptr, the value to remove is not found in the tree.
        throw BSTException(BSTException::E_NOT_FOUND, "Value to remove not found in the tree");
    }

    if (value < tree->data) {
        // If the value is less than the current node's data, continue the search in the left subtree.
        remove_(tree->left, value);
    } else if (value > tree->data) {
        // If the value is greater than the current node's data, continue the search in the right subtree.
        remove_(tree->right, value);
    } else {
        if (tree->left == nullptr) {
            // Case 1: No left child or both children are nullptr
            // Replace the current node with its right child.
            BinTree temp = tree->right;
            freeNode(tree);
            tree = temp;
        } else if (tree->right == nullptr) {
            // Case 2: No right child
            // Replace the current node with its left child.
            BinTree temp = tree->left;
            freeNode(tree);
            tree = temp;
        } else {
            // Case 3: Node has two children
            // Find the predecessor node from the left subtree, replace the current node with it,
            // and remove the predecessor node.
            BinTree predecessor = nullptr;
            findPredecessor(tree, predecessor);
            tree->data = predecessor->data;
            remove_(tree->left, predecessor->data);
        }
    }

    if (tree != nullptr) {
        // Update the count to reflect the number of elements in the current node's subtree.
        tree->count = size_(tree->left) + size_(tree->right) + 1;
    }
}

template <typename T>
int BST<T>::height_(const BinTree& tree) const {
    return treeHeight(tree);
}

template <typename T>
void BST<T>::copy_(BinTree& tree, const BinTree& rtree) {
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

