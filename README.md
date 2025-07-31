# Sparse Matrix Calculator in C

A comprehensive and feature-rich implementation of sparse matrix data structure in C using dual-linked lists with advanced matrix operations, management system, and interactive interface.

## Overview

This project implements an advanced sparse matrix calculator optimized for matrices with a large number of zero elements. The implementation uses a sophisticated dual-linked list approach where each non-zero element is connected both horizontally (by row) and vertically (by column), enabling efficient operations across multiple matrices.

The calculator supports up to 10 named matrices (A-J) with a complete matrix management system, advanced mathematical operations including determinant calculation and matrix inversion, and multiple user interfaces for different use cases.

## Key Features

### Core Functionality
- **Memory Efficient**: Only stores non-zero elements, dramatically reducing memory usage
- **Dual Indexing**: Fast access by both rows and columns using separate linked lists
- **Dynamic Sizing**: No predefined matrix size limitations with runtime resizing
- **Named Matrix System**: Manage up to 10 matrices simultaneously (A-J)
- **Robust Error Handling**: Comprehensive error checking and status codes

### Advanced Operations
- **Basic Operations**: Insert, delete, search, clear
- **Matrix Arithmetic**: Addition, subtraction, multiplication
- **Linear Algebra**: Transpose, determinant, matrix inverse
- **Scalar Operations**: Scalar multiplication
- **Matrix Management**: Create, resize, copy, clear matrices

### User Interfaces
- **Menu-Driven Interface**: Intuitive hierarchical menu system
- **Command-Line Operations**: Express operations using simple commands
- **Multiple Display Modes**: Full matrix view and sparse representation
- **Interactive Matrix Builder**: Step-by-step matrix creation

---

## Data Structure

The sparse matrix uses three main node types:

### Element Node (`Sm_Node`)
Stores a non-zero value and links to neighbors in the same row and column.
```c
typedef struct Sm_Node_Tag {
    float data;
    int row, col;
    struct Sm_Node_Tag* right;
    struct Sm_Node_Tag* down;
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
### SparseMatrix Wrapper
```c
typedef struct SparseMatrix {
    int rowCount, colCount;
    Row_Node* rowHead;
    Col_Node* colHead;
} SparseMatrix;
```
### Registry for Matrix Storage
```c
typedef struct Named_Matrix_Tag {
    char name;
    SparseMatrix matrix;
    boolean isOccupied;
} NamedMatrix;

NamedMatrix registry[MAX_MATRICES];
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

## Advanced Algorithms

### Matrix Inversion Algorithm
- **Cofactor Method**: Uses recursive determinant calculation
- **Adjugate Matrix**: Computes transpose of cofactor matrix
- **Singular Matrix Detection**: Handles non-invertible matrices gracefully
- **Memory Optimization**: Efficient handling of intermediate calculations

### Determinant Calculation
- **Recursive Expansion**: Uses cofactor expansion along first row
- **Submatrix Generation**: Dynamic creation of (n-1)×(n-1) submatrices
- **Base Case Optimization**: Direct calculation for 1×1 matrices
- **Sign Alternation**: Proper handling of cofactor signs

### Smart Matrix Operations
- **Dimension Validation**: Automatic compatibility checking
- **Zero Handling**: Intelligent zero-element management
- **Memory Management**: Automatic cleanup of temporary matrices
- **Result Preservation**: Optional saving of operation results

## Installation & Setup

### Prerequisites
- **GCC Compiler**: Version 4.8 or later
- **Standard C Library**: Full C99 support required
- **Memory**: Minimum 1MB RAM for typical operations

### Compilation
```bash
# Standard compilation
gcc -o matrix_calculator sparse_matrix_github.c -lm

# With debugging symbols
gcc -g -o matrix_calculator sparse_matrix_github.c -lm

# Optimized build
gcc -O3 -o matrix_calculator sparse_matrix_github.c -lm
```

### Running the Program
```bash
./matrix_calculator
```

## User Interface Guide

### Main Menu Structure
```
===== MATRIX CALCULATOR =====
1. Matrix Management        # Create, modify, manage matrices
2. Matrix Operations        # Perform calculations using commands
3. Display Options          # View and inspect matrices
4. Exit                     # Clean shutdown
```

### Matrix Management Submenu
- **Create Matrix**: Initialize new matrices with custom dimensions
- **Resize Matrix**: Modify dimensions of existing matrices
- **Insert/Update Element**: Add or modify individual elements
- **Delete Element**: Remove specific elements
- **Clear Matrix**: Reset matrix to empty state

### Operation Command System
Execute operations using natural language commands:

```bash
# Arithmetic Operations
add A B              # A + B
subtract A B         # A - B  
multiply A B         # A × B

# Linear Algebra
transpose A          # A^T
determinant A        # det(A)
inverse A           # A^(-1)

# Scalar Operations
scalar A 2.5        # A × 2.5

# Exit command mode
exit
```

### Display Options
1. **List All Matrices**: Overview of all 10 matrix slots
2. **Full View**: Complete matrix with zeros displayed
3. **Sparse View**: Only non-zero elements with coordinates
4. **Dimensions Only**: Quick size information

## Usage Examples

### Creating and Populating a Matrix
```
Enter your choice: 1
Enter your choice: 1
Enter matrix name (A-J): A
Enter number of rows: 3
Enter number of columns: 3
Matrix A created [3 x 3].

Enter non-zero elements (row col value), or -1 to finish:
> 0 0 5.0
> 0 2 2.0
> 2 0 3.0
> 2 2 1.0
> -1
Matrix A created and initialized.
```

### Matrix Operations via Commands
```
> Operation: add A B
Matrix R [3 x 3]
  8.00   0.00   2.00
  0.00   0.00   0.00
  3.00   0.00   6.00

Do you want to save the resultant matrix before deleting? (y/n): y
Enter a destination matrix name (A-J): C
Matrix copied to 'C'
```

### Matrix Display Modes
**Sparse View:**
```
Matrix A [3 x 3]
(0, 0) -> 5.00
(0, 2) -> 2.00
(2, 0) -> 3.00
(2, 2) -> 1.00
```

**Full View:**
```
Matrix A [3 x 3]
  5.00   0.00   2.00
  0.00   0.00   0.00
  3.00   0.00   1.00
```

## Advanced Features

### Dynamic Matrix Resizing
- **Automatic Cleanup**: Removes out-of-bounds elements
- **Dimension Expansion**: Allows growing matrix size
- **Memory Optimization**: Efficient handling of size changes

### Intelligent Operation Handling
- **Result Management**: Optional saving of operation results
- **Memory Cleanup**: Automatic deallocation of temporary matrices  
- **Error Recovery**: Graceful handling of operation failures

### Matrix Registry System
- **Named Access**: Reference matrices by single character (A-J)
- **Occupancy Tracking**: Automatic management of available slots
- **Overwrite Protection**: Warnings when overwriting existing matrices

## Performance Analysis

### Time Complexity
| Operation | Average Case | Worst Case | Space |
|-----------|--------------|------------|-------|
| Insert/Update | O(r + c) | O(r + c) | O(1) |
| Delete | O(r + c) | O(r + c) | O(1) |
| Search | O(c) | O(c) | O(1) |
| Addition | O(n₁ + n₂) | O(n₁ + n₂) | O(n) |
| Multiplication | O(n₁ × c₂) | O(n₁ × c₂) | O(n) |
| Transpose | O(n) | O(n) | O(1) |
| Determinant | O(n! × d) | O(n! × d) | O(n²) |
| Inverse | O(n³ × d) | O(n³ × d) | O(n²) |

**Legend:**
- n = number of non-zero elements
- r = rows with elements, c = columns in row  
- n₁, n₂ = non-zero elements in operand matrices
- c₂ = columns in second matrix
- d = average density of submatrices

### Memory Efficiency
- **Storage Reduction**: For sparsity s%, memory usage ≈ (1-s) × dense storage
- **Typical Savings**: 80-95% memory reduction for real-world sparse matrices
- **Overhead**: ~24 bytes per non-zero element (on 64-bit systems)

## Error Handling & Robustness

### Input Validation
- **Dimension Checking**: Validates row/column bounds
- **Matrix Compatibility**: Ensures operation validity
- **Memory Allocation**: Handles insufficient memory gracefully

### Error Recovery
- **Partial Operations**: Maintains consistency during failures
- **Resource Cleanup**: Prevents memory leaks
- **User Feedback**: Clear error messages and recovery suggestions

### Edge Cases
- **Empty Matrices**: Proper handling of zero-element matrices
- **Singular Matrices**: Graceful inverse calculation failures
- **Dimension Mismatches**: Clear error reporting for incompatible operations
