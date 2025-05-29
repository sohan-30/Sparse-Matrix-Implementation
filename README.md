# Sparse Matrix Implementation in C

A comprehensive implementation of sparse matrix data structure in C using linked lists with both row and column indexing for efficient storage and operations.

## Overview

This project implements a sparse matrix data structure optimized for matrices with a large number of zero elements. Instead of storing all elements (including zeros), it only stores non-zero elements, significantly reducing memory usage for sparse matrices.

The implementation uses a dual-linked list approach where each non-zero element is connected both horizontally (by row) and vertically (by column), allowing efficient row-wise and column-wise operations.The program supports a variety of operations including insertion, deletion, search, transpose, addition, subtraction, and multiplication. It also includes a **menu-driven interface** for interactive testing

## Features

-  **Memory Efficient**: Only stores non-zero elements
-  **Dual Indexing**: Fast access by both rows and columns
-  **Dynamic Size**: No predefined matrix size limitations
-  **Complete Operations**: Insert, delete, search, transpose, add, subtract, multiply
-  **Interactive Menu**: User-friendly command-line interface
-  **Error Handling**: Robust error checking and status codes

## Data Structure

The sparse matrix uses three main node types:

### Element Node (`Sm_Node`)
```c
typedef struct Sm_Node_Tag
{
    matrix_entry data;           // The actual value
    int row, col;               // Position coordinates
    struct Sm_Node_Tag* right;  // Next element in same row
    struct Sm_Node_Tag* down;   // Next element in same column
} Sm_Node;
```

### Row Header Node (`Row_Node`)
```c
typedef struct Row_Node_Tag
{
    int row;                    // Row number
    Sm_Node* rowlist;          // First element in this row
    struct Row_Node_Tag* next; // Next row header
} Row_Node;
```

### Column Header Node (`Col_Node`)
```c
typedef struct Col_Node_Tag
{
    int col;                    // Column number
    Sm_Node* collist;          // First element in this column
    struct Col_Node_Tag* next; // Next column header
} Col_Node;
```

## How It Works

### Storage Strategy
The sparse matrix maintains two separate linked lists:
1. **Row Headers**: A linked list of row headers, each pointing to elements in that row
2. **Column Headers**: A linked list of column headers, each pointing to elements in that column

### Element Connectivity
Each non-zero element is connected in two directions:
- **Horizontally**: Connected to other elements in the same row via `right` pointers
- **Vertically**: Connected to other elements in the same column via `down` pointers

### Key Algorithms

#### Insertion Process
1. **Find/Create Row Header**: Locate the appropriate row or create a new row header
2. **Find/Create Column Header**: Locate the appropriate column or create a new column header
3. **Position Element**: Find the correct position within the row's element list
4. **Link Horizontally**: Connect the element to its row neighbors
5. **Link Vertically**: Connect the element to its column neighbors

#### Search Process
1. **Locate Row**: Find the row header for the target row
2. **Traverse Row**: Follow the `right` pointers to find the target column
3. **Verify Position**: Confirm the element exists at the specified coordinates

#### Matrix Operations
- **Addition/Subtraction**: Traverse both matrices simultaneously, combining elements at matching positions
- **Multiplication**: Use row-column traversal, utilizing the dual-indexing for efficient access
- **Transpose**: Swap row and column headers, and exchange `right`/`down` pointers

## Installation

### Prerequisites
- GCC compiler or any C compiler
- Standard C library support

### Compilation
```bash
# Compile the program
gcc -o sparse_matrix sparse_matrix.c

# Run the program
./sparse_matrix
```

## Usage

Run the compiled program to access the interactive menu:

```bash
./sparse_matrix
```

The program will display a menu with the following options:
```
==== Sparse Matrix Menu ====
1. Insert Element
2. Delete Element
3. Search Element
4. Print Matrix
5. Transpose Matrix
6. Add Two Matrices
7. Subtract Two Matrices
8. Multiply Two Matrices
0. Exit
```

## Operations

### Basic Operations

#### Insert Element
```c
status_code insertElement(int row, int col, matrix_entry data, SparseMatrix* matrix)
```
- Inserts a non-zero element at specified position
- Creates row/column headers if they don't exist
- Updates existing element if position is occupied

#### Delete Element
```c
status_code deleteElement(int row, int col, SparseMatrix* matrix, matrix_entry *dptr)
```
- Removes element at specified position
- Cleans up empty row/column headers
- Returns the deleted value

#### Search Element
```c
boolean search(int row, int col, SparseMatrix* matrix)
```
- Checks if an element exists at specified position
- Returns TRUE if found, FALSE otherwise

### Matrix Operations

#### Addition
```c
status_code addMatrix(SparseMatrix* matrix1, SparseMatrix* matrix2, SparseMatrix* result)
```
- Adds two sparse matrices
- Handles matrices of different sparsity patterns
- Validates matrix dimensions

#### Subtraction
```c
status_code subtractMatrix(SparseMatrix* matrix1, SparseMatrix* matrix2, SparseMatrix* result)
```
- Subtracts second matrix from first
- Implemented using negation and addition

#### Multiplication
```c
status_code multiplyMatrix(SparseMatrix* matrix1, SparseMatrix* matrix2, SparseMatrix* result)
```
- Multiplies two sparse matrices
- Validates dimension compatibility (columns of first = rows of second)
- Utilizes dual-indexing for efficient computation

#### Transpose
```c
status_code transpose(SparseMatrix* matrix)
```
- Transposes the matrix in-place
- Swaps row and column indices
- Exchanges horizontal and vertical links

## Examples

### Creating a Simple Matrix
```
Matrix representation of:
[5.0  0   2.0]
[0    0   0  ]
[3.0  0   1.0]

Storage (only non-zero elements):
(0,0) -> 5.0
(0,2) -> 2.0
(2,0) -> 3.0
(2,2) -> 1.0
```

### Sample Interaction
```
Enter your choice: 1
Insert into Matrix 1.
Enter row, col, value: 0 0 5.0
Element inserted.

Enter your choice: 1
Enter row, col, value: 0 2 2.0
Element inserted.

Enter your choice: 4
Matrix 1:
Sparse Matrix Representation:
(0, 0) -> 5.00
(0, 2) -> 2.00
```

## Time Complexity

| Operation | Average Case | Worst Case |
|-----------|--------------|------------|
| Insert    | O(r + c)     | O(r + c)   |
| Delete    | O(r + c)     | O(r + c)   |
| Search    | O(c)         | O(c)       |
| Addition  | O(n₁ + n₂)   | O(n₁ + n₂) |
| Multiplication | O(n₁ × c₂) | O(n₁ × c₂) |
| Transpose | O(n)         | O(n)       |

Where:
- `r` = number of rows with elements
- `c` = number of columns in a row
- `n` = number of non-zero elements
- `n₁, n₂` = non-zero elements in matrix 1 and 2
- `c₂` = number of columns in matrix 2

## Memory Complexity

- **Space Complexity**: O(n) where n is the number of non-zero elements
- **Memory Savings**: For a matrix with sparsity s (percentage of zeros), memory usage is approximately (1-s) × traditional storage

## Acknowledgments

- Inspired by classical sparse matrix implementations
- Designed for educational purposes and practical applications
- Optimized for both memory efficiency and operation speed
