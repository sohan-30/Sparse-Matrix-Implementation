#include <stdio.h>
#include <stdlib.h>

typedef enum{FAILURE, SUCCESS} status_code;
typedef enum{FALSE, TRUE} boolean;
typedef float matrix_entry;

typedef struct Sm_Node_Tag    // element node
{
    matrix_entry data;
    int row, col;
    struct Sm_Node_Tag* right;
    struct Sm_Node_Tag* down;
} Sm_Node;
typedef struct Row_Node_Tag   // row head
{
    int row;
    Sm_Node* rowlist;
    struct Row_Node_Tag* next;
} Row_Node;
typedef struct Col_Node_Tag   // column head
{
    int col;
    Sm_Node* collist;
    struct Col_Node_Tag* next;
} Col_Node;
typedef struct SparseMatrix
{
    Row_Node *rowHead;
    Col_Node *colHead;
} SparseMatrix;

void initializeMatrix(SparseMatrix* matrix)
{
    matrix->colHead = NULL;
    matrix->rowHead = NULL;
}
Row_Node* createRowNode(int row)
{
    Row_Node* nptr;
    nptr = (Row_Node*)malloc(sizeof(Row_Node));
    nptr->row = row;
    nptr->rowlist = NULL;
    nptr->next = NULL;
    return nptr;
}
Col_Node* createColNode(int col)
{
    Col_Node* nptr;
    nptr = (Col_Node*)malloc(sizeof(Col_Node));
    nptr->col = col;
    nptr->collist = NULL;
    nptr->next = NULL;
    return nptr;
}
Sm_Node* createEleNode(int row, int col, matrix_entry data)
{
    Sm_Node* nptr;
    nptr = (Sm_Node*)malloc(sizeof(Sm_Node));
    nptr->row = row;
    nptr->col = col;
    nptr->data = data;
    nptr->down = nptr->right = NULL;
    return nptr;
}
status_code insertElement(int row, int col, matrix_entry data, SparseMatrix* matrix)
{
    status_code sc = SUCCESS;
    Sm_Node *prevE = NULL, *element, *nptrE;

    Row_Node* prevR = NULL, *rowPos = matrix->rowHead, *nptrR;
    Col_Node* prevC = NULL, *colPos = matrix->colHead, *nptrC;
    while(rowPos != NULL && rowPos->row<row) // search row
    {
        prevR = rowPos;
        rowPos = rowPos->next;
    }
    while(colPos != NULL && colPos->col<col) // search column
    {
        prevC = colPos;
        colPos = colPos->next;
    }
    if(rowPos == NULL || rowPos->row != row)// create row node if not exist
    {
        nptrR = createRowNode(row);
        if(nptrR != NULL)
        {
            nptrR->next = rowPos;
            if(prevR != NULL)
            {
                prevR->next = nptrR;
            }
            else
            {
                matrix->rowHead = nptrR;
            }
            nptrE = createEleNode(row, col, data);//created here
            nptrR->rowlist = nptrE;
            rowPos = nptrR;
        }
        else
        {
            sc = FAILURE;
        }
    }
    else // row node exists
    { // search if element node exist or not
        prevE = NULL;
        element = rowPos->rowlist;
        while((element != NULL) && (element->col < col))
        {
            prevE = element;
            element = element->right;
        }
        if(element!=NULL/*update*/ && element->col == col)// element exists // no need to update in column 
        {
            element->data = data;
        }
        else // element DNE
        {// lets dont worry about column, lets just create element node
            nptrE = createEleNode(row, col, data);
            if(nptrE != NULL)
            {
                nptrE->right = element;
                if(prevE != NULL)
                {
                    prevE->right = nptrE;
                }
                else
                {
                    rowPos->rowlist = nptrE;
                }
            }
            else
            {
                sc = FAILURE;
            }
        }
    }

    if(colPos == NULL || colPos->col != col)// create column node if not exist
    {
        nptrC = createColNode(col);
        if(nptrC != NULL)
        {   
            nptrC->next = colPos;
            if(prevC != NULL)
            {
                prevC->next = nptrC;
            }
            else
            {
                matrix->colHead = nptrC;
            }
            nptrC->collist = nptrE;// element created above 
            colPos = nptrC;
        }
        else
        {
            sc = FAILURE;
        }
    }
    else // column node exists
    { // search if element node exist or not
        prevE = NULL;
        element = colPos->collist;
        //searching for correct pos of element
        while((element != NULL) && (element->row < row))
        {
            prevE = element;
            element = element->down;
        }
        if(element!=NULL/*update*/ && element->row == row && element->col == col)
        {//if ele exist, updating value done in row
            printf("Unexpected duplication at row = %d, col = %d\n", row, col);
            sc = FAILURE;
        }
        else
        {
            // attached it according to row // element created above
            nptrE->down = element;
            if(prevE != NULL)
            {
                prevE->down = nptrE;
            }
            else
            {
                colPos->collist = nptrE;
            }
        }
    }
    return sc;
}
status_code deleteElement(int row, int col, SparseMatrix* matrix, matrix_entry *dptr)
{
    //lets assume insertion done correctly and no error happen
    status_code sc = SUCCESS;
    Sm_Node *prevE = NULL, *element, *nptrE;

    Row_Node* prevR = NULL, *rowPos = matrix->rowHead;
    Col_Node* prevC = NULL, *colPos = matrix->colHead;

    while(rowPos != NULL && rowPos->row<row) // search row
    {
        prevR = rowPos;
        rowPos = rowPos->next;
    }
    while(colPos != NULL && colPos->col<col) // search column
    {
        prevC = colPos;
        colPos = colPos->next;
    }

    if(rowPos != NULL && rowPos->row == row)
    {
        prevE = NULL;
        element = rowPos->rowlist;
        while((element != NULL) && (element->col < col))
        {
            prevE = element;
            element = element->right;
        }
        if(element!=NULL && element->col == col) // row exists
        {
            //delete from row
            if(prevE != NULL)
            {
                prevE->right = element->right;
            }
            else
            {
                rowPos->rowlist = element->right;
            }

            if(colPos != NULL && colPos->col == col)// just for safety
            {
                prevE = NULL;
                element = colPos->collist;
                //searching for correct pos of element in column
                while((element != NULL) && (element->row != row))
                {
                    prevE = element;
                    element = element->down;
                }
                if(element != NULL)//just for safety measures//column exists 
                {
                    //delete from column
                    if(prevE != NULL)
                    {
                        prevE->down = element->down;
                    }
                    else
                    {
                        colPos->collist = element->down;
                    }
                    *dptr = element->data;
                    free(element);

                    if(rowPos->rowlist == NULL)
                    {
                        if(prevR != NULL)
                        {
                            prevR->next = rowPos->next;
                        }
                        else
                        {
                            matrix->rowHead = rowPos->next;
                        }
                        free(rowPos);
                    }
                    if(colPos->collist == NULL)
                    {
                        if(prevC != NULL)
                        {
                            prevC->next = colPos->next;
                        }
                        else
                        {
                            matrix->colHead = colPos->next;
                        }
                        free(colPos);
                    }
                }
                else// col DNE
                {
                    sc = FAILURE;
                }
            }

        }
        else// row DNE
        {
            sc = FAILURE;
        }
    }
    else
    {
        sc = FAILURE;
    }
    return sc;
}
void printMatrix(SparseMatrix *sm) {
    printf("\nSparse Matrix Representation:\n");
    Row_Node* rptr = sm->rowHead;
    while (rptr) 
    {
        Sm_Node* sptr = rptr->rowlist;
        while (sptr) 
        {
            printf("(%d, %d) -> %.2f\n", sptr->row, sptr->col, sptr->data);
            sptr = sptr->right;
        }
        rptr = rptr->next;
    }
}
void clearMatrix(SparseMatrix *sm) 
{
    Row_Node* rptr = sm->rowHead;
    matrix_entry d;
    while (rptr) 
    {
        Sm_Node* sptr = rptr->rowlist;
        while (sptr) 
        {
            deleteElement(sptr->row, sptr->col, sm, &d);
            sptr = sptr->right;
        }
        rptr = rptr->next;
    }
}
boolean search(int row, int col, SparseMatrix* matrix)//if exists true otherwise false
{
    boolean exist;
    Sm_Node *element;

    Row_Node *rowPos = matrix->rowHead;
    Col_Node *colPos = matrix->colHead;

    while(rowPos != NULL && rowPos->row<row) // search row
    {
        rowPos = rowPos->next;
    }
    while(colPos != NULL && colPos->col<col) // search column
    {
        colPos = colPos->next;
    }
    if(rowPos == NULL || rowPos->row != row || colPos == NULL || colPos->col != col)
    {
        exist = FALSE;
    }
    else
    {
        element = rowPos->rowlist;
        while((element != NULL) && (element->col < col))
        {
            element = element->right;
        }
        if(element!=NULL && element->col == col && element->row == row)
        {
            exist = TRUE;
        }
        else
        {
            exist = FALSE; 
        }
    }
    return exist;
}
status_code transpose(SparseMatrix* matrix)
{
    status_code sc = SUCCESS;
    Sm_Node *element, *temp, *prev;

    Row_Node *prevR = NULL, *rowPos = matrix->rowHead, *nptrColR, *prevColR = NULL, *tempHeadNewRow = NULL;
    Col_Node *prevC = NULL, *colPos = matrix->colHead, *nptrRowC, *prevRowC = NULL;
    while(colPos != NULL)
    {
        nptrColR = createRowNode(colPos->col);
        if(nptrColR != NULL)
        {
            nptrColR->rowlist = colPos->collist;
            if(prevColR)
            {
                prevColR->next = nptrColR;
            }
            else
            {
                tempHeadNewRow = nptrColR;
            }
            prevColR = nptrColR;
            prevC = colPos;
            colPos = colPos->next;
            free(prevC);
        }
        else
        {
            sc = FAILURE;
        }
    }
    while(rowPos != NULL)
    {
        nptrRowC = createColNode(rowPos->row);
        if(nptrRowC != NULL)
        {
            nptrRowC->collist = rowPos->rowlist;
            if(prevRowC)
            {
                prevRowC->next = nptrRowC;
            }
            else
            {
                (matrix->colHead) = nptrRowC;
            }
            element = rowPos->rowlist;
            while(element != NULL)
            {
                element->row ^= element->col;
                element->col ^= element->row;
                element->row ^= element->col;
                
                prev = element;
                element = element->right;

                temp = prev->right;
                prev->right = prev->down;
                prev->down = temp;
            }
            prevRowC = nptrRowC;
            prevR = rowPos;
            rowPos = rowPos->next;
            free(prevR);
        }
        else
        {
            sc = FAILURE;
        }
    }
    matrix->rowHead = tempHeadNewRow;
    return sc;
}
status_code addMatrix(SparseMatrix* matrix1, SparseMatrix* matrix2, SparseMatrix* result)
{
    status_code sc = SUCCESS;
    Sm_Node *element1, *element2;
    int sum;

    Row_Node *rowPos1 = matrix1->rowHead, *rowPos2 = matrix2->rowHead;
    if(matrix1->rowHead == NULL) 
    {
        while(rowPos2)
        {
            element2 = rowPos2->rowlist;
            while(element2)
            {
                insertElement(element2->row, element2->col, element2->data, result);
                element2 = element2->right;
            }
            rowPos2 = rowPos2->next;
        }
        return sc;
    } 
    else if(matrix2->rowHead == NULL) 
    {
        while(rowPos1)
        {
            element1 = rowPos1->rowlist;
            while(element1)
            {
                insertElement(element1->row, element1->col, element1->data, result);
                element1 = element1->right;
            }
            rowPos1 = rowPos1->next;
        }
        return sc;
    }
    else
    {
        Col_Node *colPos1 = matrix1->colHead, *colPos2 = matrix2->colHead;
        int maxRow1 = 0, maxCol1 = 0, maxRow2 = 0, maxCol2 = 0;
        
        while(rowPos1 != NULL)
        {
            if(rowPos1->row > maxRow1) 
            {
                maxRow1 = rowPos1->row;
            }
            rowPos1 = rowPos1->next;
        }
        while(colPos1 != NULL)
        {
            if(colPos1->col > maxCol1) 
            {
                maxCol1 = colPos1->col;
            }
            colPos1 = colPos1->next;
        }

        while(rowPos2 != NULL)
        {
            if(rowPos2->row > maxRow2) 
            {
                maxRow2 = rowPos2->row;
            }
            rowPos2 = rowPos2->next;
        }
        while(colPos2 != NULL)
        {
            if(colPos2->col > maxCol2) 
            {
                maxCol2 = colPos2->col;
            }
            colPos2 = colPos2->next;
        }

        if(maxRow1 != maxRow2 || maxCol1 != maxCol2)
        {
            sc = FAILURE;
        }
        else
        {
            rowPos1 = matrix1->rowHead, rowPos2 = matrix2->rowHead;
            while(rowPos1 || rowPos2)
            {
                if(!rowPos2 || (rowPos1 && rowPos1->row < rowPos2->row))
                {
                    element1 = rowPos1->rowlist;
                    while(element1 != NULL)
                    {
                        insertElement(element1->row, element1->col, element1->data, result);
                        element1 = element1->right;
                    }
                    rowPos1 = rowPos1->next;
                }
                else if(!rowPos1 || (rowPos2 && rowPos1->row > rowPos2->row))
                {
                    element2 = rowPos2->rowlist;
                    while(element2 != NULL)
                    {
                        insertElement(element2->row, element2->col, element2->data, result);
                        element2 = element2->right;
                    }
                    rowPos2 = rowPos2->next;
                }
                else
                {
                    element1 = rowPos1->rowlist;
                    element2 = rowPos2->rowlist;
                    while(element1 || element2)
                    {
                        if(!element2 || (element1 && element1->col < element2->col))
                        {
                            insertElement(element1->row, element1->col, element1->data, result);
                            element1 = element1->right;
                        }
                        else if(!element1 || (element2 && element1->col > element2->col))
                        {
                            insertElement(element2->row, element2->col, element2->data, result);
                            element2 = element2->right;
                        }
                        else
                        {
                            sum = element1->data + element2->data;
                            if(sum != 0)
                            {
                                insertElement(element1->row, element1->col, sum, result);
                            }
                            element1 = element1->right;
                            element2 = element2->right;
                        }
                    }
                    rowPos1 = rowPos1->next;
                    rowPos2 = rowPos2->next;
                }
            }
        }
    }
    return sc;
}
status_code subtractMatrix(SparseMatrix* matrix1, SparseMatrix* matrix2, SparseMatrix* result) {
    status_code sc = SUCCESS;
    SparseMatrix negMatrix2;
    initializeMatrix(&negMatrix2);

    Sm_Node* element;
    Row_Node* rowPos = matrix2->rowHead;
    while (rowPos != NULL) 
    {
        element = rowPos->rowlist;
        while (element != NULL) 
        {
            insertElement(element->row, element->col, -element->data, &negMatrix2);
            element = element->right;
        }
        rowPos = rowPos->next;
    }

    sc = addMatrix(matrix1, &negMatrix2, result);

    clearMatrix(&negMatrix2);

    return sc;
}
status_code multiplyMatrix(SparseMatrix* matrix1, SparseMatrix* matrix2, SparseMatrix* result)
{
    status_code sc = SUCCESS;
    Sm_Node *element1, *element2;
    int product = 0;
    
    Row_Node *rowPos1 = matrix1->rowHead, *rowPos2 = matrix2->rowHead;
    Col_Node *colPos1 = matrix1->colHead, *colPos2 = matrix2->colHead;
    if(matrix1->rowHead == NULL || matrix2->rowHead == NULL)
    {
        initializeMatrix(result);
        sc = SUCCESS;
    }
    else
    {
        int maxCol1 = 0, maxRow2 = 0;
        while(colPos1 != NULL)
        {
            if(colPos1->col > maxCol1) 
            {
                maxCol1 = colPos1->col;
            }
            colPos1 = colPos1->next;
        }
        while(rowPos2 != NULL)
        {
            if(rowPos2->row > maxRow2) 
            {
                maxRow2 = rowPos2->row;
            }
            rowPos2 = rowPos2->next;
        }
        if(maxCol1 == maxRow2)
        {
            while(rowPos1)
            {
                colPos2 = matrix2->colHead;
                while(colPos2)
                {
                    product = 0;
                    element1 = rowPos1->rowlist;
                    element2 = colPos2->collist;
                    while(element1)
                    {
                        while(element2 && element2->row < element1->col)
                        {
                            element2 = element2->down;
                        }
                        if(element2 != NULL && element2->row == element1->col)
                        {
                            product = product + element1->data * element2->data;
                        }
                        element1 = element1->right;
                    }
                    if(product != 0)
                    {
                        insertElement(rowPos1->row, colPos2->col, product, result);
                    }
                    colPos2 = colPos2->next;
                }
                rowPos1 = rowPos1->next;
            }
        }
        else
        {
            sc = FAILURE;
        }
    }
    return sc;
}
void displayMenu()
{
    printf("\n==== Sparse Matrix Menu ====\n");
    printf("1. Insert Element\n");
    printf("2. Delete Element\n");
    printf("3. Search Element\n");
    printf("4. Print Matrix\n");
    printf("5. Transpose Matrix\n");
    printf("6. Add Two Matrices\n");
    printf("7. Subtract Two Matrices\n");
    printf("8. Multiply Two Matrices\n");
    printf("0. Exit\n");
    printf("Enter your choice: ");
}

int main()
{
    SparseMatrix matrix1, matrix2, result;
    initializeMatrix(&matrix1);
    initializeMatrix(&matrix2);
    initializeMatrix(&result);

    int choice, row, col;
    matrix_entry value;
    matrix_entry deletedValue;

    while(1)
    {
        displayMenu();
        scanf("%d", &choice);
        switch (choice)
        {
            case 1:
                printf("Insert into Matrix 1.\nEnter row, col, value: ");
                scanf("%d %d %f", &row, &col, &value);
                if (insertElement(row, col, value, &matrix1) == SUCCESS)
                    printf("Element inserted.\n");
                else
                    printf("Insertion failed.\n");
                break;

            case 2:
                printf("Delete from Matrix 1.\nEnter row and col: ");
                scanf("%d %d", &row, &col);
                if (deleteElement(row, col, &matrix1, &deletedValue) == SUCCESS)
                    printf("Deleted value: %.2f\n", deletedValue);
                else
                    printf("Deletion failed.\n");
                break;

            case 3:
                printf("Search in Matrix 1.\nEnter row and col: ");
                scanf("%d %d", &row, &col);
                if (search(row, col, &matrix1))
                    printf("Element exists at (%d,%d).\n", row, col);
                else
                    printf("Element NOT found.\n");
                break;

            case 4:
                printf("Matrix 1:\n");
                printMatrix(&matrix1);
                break;

            case 5:
                if (transpose(&matrix1) == SUCCESS)
                    printf("Transpose successful.\n");
                else
                    printf("Transpose failed.\n");
                break;

            case 6:
                clearMatrix(&result);
                printf("Enter elements for Matrix 2 for addition:\n");
                // Allow user to input a few entries into matrix2
                printf("How many elements to insert into Matrix 2? ");
                int n_add;
                scanf("%d", &n_add);
                for (int i = 0; i < n_add; i++) {
                    printf("Enter row, col, value: ");
                    scanf("%d %d %f", &row, &col, &value);
                    insertElement(row, col, value, &matrix2);
                }
                if (addMatrix(&matrix1, &matrix2, &result) == SUCCESS) {
                    printf("Result (Matrix1 + Matrix2):\n");
                    printMatrix(&result);
                } else {
                    printf("Addition failed (incompatible sizes).\n");
                }
                clearMatrix(&matrix2);
                break;

            case 7:
                clearMatrix(&result);
                printf("Enter elements for Matrix 2 for subtraction:\n");
                int n_sub;
                scanf("%d", &n_sub);
                for (int i = 0; i < n_sub; i++) {
                    printf("Enter row, col, value: ");
                    scanf("%d %d %f", &row, &col, &value);
                    insertElement(row, col, value, &matrix2);
                }
                if (subtractMatrix(&matrix1, &matrix2, &result) == SUCCESS) {
                    printf("Result (Matrix1 - Matrix2):\n");
                    printMatrix(&result);
                } else {
                    printf("Subtraction failed (incompatible sizes).\n");
                }
                clearMatrix(&matrix2);
                break;

            case 8:
                clearMatrix(&result);
                printf("Enter elements for Matrix 2 for multiplication:\n");
                int n_mul;
                scanf("%d", &n_mul);
                for (int i = 0; i < n_mul; i++) {
                    printf("Enter row, col, value: ");
                    scanf("%d %d %f", &row, &col, &value);
                    insertElement(row, col, value, &matrix2);
                }
                if (multiplyMatrix(&matrix1, &matrix2, &result) == SUCCESS) {
                    printf("Result (Matrix1 x Matrix2):\n");
                    printMatrix(&result);
                } else {
                    printf("Multiplication failed (dimension mismatch).\n");
                }
                clearMatrix(&matrix2);
                break;

            case 0:
                printf("Exiting program.\n");
                clearMatrix(&matrix1);
                clearMatrix(&matrix2);
                clearMatrix(&result);
                return 0;

            default:
                printf("Invalid choice. Try again.\n");
                break;
        }
    }

    return 0;
}

