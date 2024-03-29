# Data Structures and Algorithim
Data Structures and Algorithim

Includes Projects on graphs, BST, AVL, Allocator.

Certainly! Here's a revised version suitable for a GitHub README:

---

# Graph Assignment

This project focuses on graph representations using the Adjacency List Graph (ALGraph) abstract data type (ADT) and the implementation of Dijkstra's algorithm.

## Overview

The ALGraph is implemented using `std::vec` to optimize the maintenance of order in the adjacency lists. The project includes a class definition in `ALGraph.h` and an implementation in `ALGraph.cpp`. The `test.cpp` file contains unit tests for the implementation.

## Features

- **Adding Edges:** Directed and undirected edges can be added to the graph using `addDEdge` and `addUEdge`, respectively.
- **Viewing the Graph:** The `dumpGraph` method prints the adjacency lists in a human-readable format. Graphs are also saved in the DOT format for visualization.
- **Dijkstra's Algorithm:** The `dijkstra` method implements Dijkstra's all-source shortest paths algorithm, returning a vector of `DijkstraInfo` with the shortest path information.

## Compilation and Testing

Use the provided Makefile for compilation and testing:

- **Compile the code:**
  ```
  make
  ```
- **Run a specific test:**
  ```
  make test<test_number>
  ```
- **Run a test without comparing to the expected output:**
  ```
  make test<test_number>-nocompare
  ```
- **Clean up compiled files:**
  ```
  make clean
  ```
- **Generate a PNG file for graph visualization:**
  ```
  make test<test_number>-dot
  ```

## Usage

To use the ALGraph in a project, include `ALGraph.h` and link against the compiled `ALGraph.cpp`. Instantiate the `ALGraph` class and use its methods to add edges and perform graph operations.

## Debugging

The `getAdjList` method provides direct access to the internal adjacency list for debugging purposes.

## Visualization

Graphs can be visualized using Graphviz by converting the `.dot` files generated by the `dumpGraph` method.

---

---

# Simple Allocator Assignment

## Overview

This assignment involves implementing a simple memory allocator. The main objective is to gain a deeper understanding of memory management and linked list data structures.

## Implementation

Your task is to create a `SimpleAllocator.cpp` file that conforms to the `SimpleAllocator` class provided in the `SimpleAllocator.h` file. The `test.cpp` file contains tests for your implementation, and your goal is to pass all these tests.

## Approach

1. Read the `SimpleAllocator.h` file to understand the interface of the `SimpleAllocator` class.
2. Read the `test.cpp` file to understand the tests.
3. Implement the `SimpleAllocator` class in the `SimpleAllocator.cpp` file to pass the tests, starting with `test0`.
4. Compile and test with each test case using the commands provided below.
5. Proceed to the next test case if the current test passes.

## Compilation and Testing

Use the following commands in the terminal:

- **Compile the code:**
  ```
  make
  ```
- **Run a specific test:**
  ```
  make test<test_number>
  ```
  Replace `<test_number>` with the correct test number.
- **Clean up compiled files:**
  ```
  make clean
  ```

## SimpleAllocator

The `SimpleAllocator` is responsible for allocating and freeing memory blocks. It manages a linked list of pages and a linked list of free blocks. The allocator breaks up a page of memory into blocks and maintains these lists for efficient memory allocation and deallocation.

## Memory Layout

The memory layout typically includes a pointer to the next page, alignment padding, headers, and memory blocks. The allocator uses two linked lists: one for the pages and another for the free blocks.

## Example

Here's an example of the memory layout after constructing the allocator, allocating two blocks, and freeing one block:

```
Constructing the allocator:
- Allocate a page of memory.
- Break up the memory into blocks.
- Initialize the page list and free list.

Allocating memory:
- Return the first free block to the client.
- Update the free list and allocation statistics.

Freeing memory:
- Mark the block as free.
- Add the block to the head of the free list.
- Update the allocation statistics.
```

## Memory Signatures

Memory signatures are used to check the integrity of the memory. The allocator uses different patterns to mark unallocated, allocated, and freed memory, as well as padding to detect buffer overruns.

## Allocator Statistics

The allocator maintains statistics about object size, page size, number of free objects, objects in use, pages in use, most objects in use, allocations, and deallocations. These statistics help track the allocator's performance and usage.

---

---

# Binary Search Tree (BST) Assignment

## Overview

This assignment involves implementing a Binary Search Tree (BST) as an Abstract Data Type (ADT) container. While a BST is a relatively simple tree structure, mastering its implementation will lay the foundation for working with more complex tree structures, such as Octrees and Merkle Trees.

## Implementation

Your task is to create a `BST.cpp` file that conforms to the `BST` class provided in the `BST.h` file. The `test.cpp` file contains test cases for your implementation, and your goal is to pass all these tests.

## Compilation and Testing

Use the following commands in the terminal:

- **Compile the code:**
  ```
  make
  ```
- **Run a specific test:**
  ```
  make test<test_number>
  ```
  Replace `<test_number>` with the correct test number.
- **Run a test without comparing to the expected output:**
  ```
  make test<test_number>-nocompare
  ```
- **Clean up compiled files:**
  ```
  make clean
  ```

## Description

The BST should use an allocator to manage memory for the nodes. A dummy `SimpleAllocator.h` is provided. The data stored in the BST should be unique, and duplicates should be rejected with a `BST_EXCEPTION::E_DUPLICATE` exception.

## Templated

The BST class is templated, allowing it to store any type of data. The implementation file should be included in the header (`#include "BST.cpp"`) due to the templated nature of the class.

## Subscript Operator

Implementing the subscript `[]` operator is a challenging part of this assignment. It requires a helper function for recursion to find the node at a given index. The implementation should follow the algorithm provided in the assignment description.

## Stress Test

Test case 10 is a stress test that involves a large number of insertions and deletions. It is left as an exercise to optimize the implementation for better performance.

---

---

# AVL Tree Assignment

## Overview

This assignment involves creating an AVL Tree, a self-balancing binary search tree. The main objective is to implement the AVL tree and ensure that it maintains its balance property after every insertion and deletion operation.

## Implementation

Your task is to create an `AVL.cpp` file that conforms to the `AVL` class provided in the `AVL.h` file. The `test.cpp` file contains test cases for your implementation, and your goal is to pass all these tests.

## Compilation and Testing

Use the following commands in the terminal:

- **Compile the code:**
  ```
  make
  ```
- **Run a specific test:**
  ```
  make test<test_number>
  ```
  Replace `<test_number>` with the correct test number.
- **Clean up compiled files:**
  ```
  make clean
  ```

## Description

The AVL tree is a balanced binary search tree (BST). You may inherit from an existing BST implementation and override the necessary code to ensure balance. The AVL tree must maintain the property that the height of the left and right subtrees of every node differ by at most 1.

Start with the basic BST operations in the first two test cases. Subsequent test cases will check if the operations maintain the AVL tree's balance.

## Rotations

Implement the rotation operations as described in class. You may choose your own method names for these operations. After a rotation, remember to update the parent of the subtree to point to the new root.

## Inorder Traversal

Implement an inorder traversal method to print the tree. This is required for the last test case.

## Memory Allocation

A dummy `SimpleAllocator` is provided for node allocation and deallocation. While the tests do not specifically check for memory allocation strategies, it is recommended to use the `SimpleAllocator` appropriately.

## Notes

- The test cases are designed to test a variety of scenarios. Passing all test cases here is a good indication of your implementation's correctness, but additional tests may be used during grading.
- If your implementation simply generates output files that match the expected outputs without properly implementing the AVL operations, marks will be deducted accordingly.

---
