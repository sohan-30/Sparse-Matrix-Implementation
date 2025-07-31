#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define MAX_MATRICES 10

typedef enum{FAILURE, SUCCESS} status_code;
typedef enum{FALSE, TRUE} boolean;
typedef enum{SPARSE_VIEW, FULL_VIEW } PrintMode;
typedef float matrix_entry;


typedef struct Sm_Node_Tag    // element node
{
    matrix_entry data;
    int row, col;
    struct Sm_Node_Tag* right;
    struct Sm_Node_Tag* down;
} Sm_Node;
typedef struct Row_Node_Tag   // row list
{
    int row;
    Sm_Node* rowlist;
    struct Row_Node_Tag* next;
} Row_Node;
typedef struct Col_Node_Tag   // column list
{
    int col;
    Sm_Node* collist;
    struct Col_Node_Tag* next;
} Col_Node;
typedef struct SparseMatrix
{
    int rowCount, colCount;
    Row_Node *rowHead;
    Col_Node *colHead;
} SparseMatrix;

typedef struct Named_Matrix_Tag
{
    char name;
    SparseMatrix matrix;
    boolean isOccupied;
}NamedMatrix;

NamedMatrix registry[MAX_MATRICES];


void initializeMatrix(SparseMatrix* matrix)
{
    matrix->rowCount = 0;
    matrix->colCount = 0;
    matrix->colHead = NULL;
    matrix->rowHead = NULL;
}
void initializeMatrixWithSize(SparseMatrix* matrix, int rows, int cols)
{
    matrix->rowCount = rows;
    matrix->colCount = cols;
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
    if(data != 0)
    {
        if(row < matrix->rowCount && col < matrix->colCount)
        {
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
                    nptrE = createEleNode(row, col, data);//created here  // can check nptrE!=NULL

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
                    printf("Unexpected duplication at row = %d, col = %d, data = %f\n", row, col, data);
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
        }
        else
        {
            printf("Row and column number are out of bounds.");
            sc = FAILURE;
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
void printNamedMatrix(const SparseMatrix* matrix, const char name, PrintMode mode)
{
    printf("Matrix %c [%d x %d]\n", name, matrix->rowCount, matrix->colCount);

    if (mode == FULL_VIEW) {
        for (int i = 0; i < matrix->rowCount; i++) {
            for (int j = 0; j < matrix->colCount; j++) {
                Sm_Node* element = NULL;
                Row_Node* row = matrix->rowHead;

                // Find the row
                while (row && row->row < i) row = row->next;
                if (row && row->row == i) {
                    element = row->rowlist;
                    while (element && element->col < j) element = element->right;
                }

                if (element && element->col == j)
                    printf("%6.2f ", element->data);
                else
                    printf("%6.2f ", 0.0);
            }
            printf("\n");
        }
    } else {
        // SPARSE_VIEW
        Row_Node* row = matrix->rowHead;
        while (row) {
            Sm_Node* element = row->rowlist;
            while (element) {
                printf("(%d, %d) -> %.2f\n", element->row, element->col, element->data);
                element = element->right;
            }
            row = row->next;
        }
    }
}
void clearMatrix(SparseMatrix *sm) 
{
    Row_Node* rptr = sm->rowHead;
    matrix_entry d;
    while(rptr) 
    {
        Sm_Node* sptr = rptr->rowlist;
        while(sptr) 
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

    // swapping rowCount and columnCount
    matrix->rowCount ^= matrix->colCount;
    matrix->colCount ^= matrix->rowCount;
    matrix->rowCount ^= matrix->colCount;

    return sc;
}
status_code addMatrix(SparseMatrix* matrix1, SparseMatrix* matrix2, SparseMatrix* result)
{
    status_code sc = SUCCESS;
    Sm_Node *element1, *element2;
    int sum;

    Row_Node *rowPos1 = matrix1->rowHead, *rowPos2 = matrix2->rowHead;

    initializeMatrix(result);

    if(matrix1->rowHead == NULL) 
    {
        result->rowCount = matrix2->rowCount;
        result->colCount = matrix2->colCount;

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
        result->rowCount = matrix1->rowCount;
        result->colCount = matrix1->colCount;

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
        if(matrix1->rowCount != matrix2->rowCount || matrix1->colCount != matrix2->colCount)
        {
            sc = FAILURE;
        }
        else
        {
            result->rowCount = matrix1->rowCount;
            result->colCount = matrix1->colCount;

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
status_code subtractMatrix(SparseMatrix* matrix1, SparseMatrix* matrix2, SparseMatrix* result)
{
    status_code sc = SUCCESS;
    SparseMatrix negMatrix2;
    initializeMatrixWithSize(&negMatrix2, matrix2->rowCount, matrix2->colCount);

    Sm_Node* element;
    Row_Node* rowPos = matrix2->rowHead;
    while(rowPos != NULL)
    {
        element = rowPos->rowlist;
        while(element != NULL)
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

    Row_Node *rowPos1 = matrix1->rowHead;
    Col_Node *colPos2 = matrix2->colHead;

    initializeMatrix(result);

    if(matrix1->rowHead == NULL || matrix2->rowHead == NULL)
    {
        sc = SUCCESS;
    }
    else
    {
        if(matrix1->colCount == matrix2->rowCount)
        {
            initializeMatrixWithSize(result, matrix1->rowCount, matrix2->colCount);

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
float determinant(SparseMatrix* matrix)
{
    float det = 0;
    if(matrix->rowCount == 1)
    {
        if(matrix->rowHead && matrix->rowHead->rowlist)
        {
            det = matrix->rowHead->rowlist->data;
        }
        else
        {
            det = 0;
        }
    }
    else
    {
        Row_Node* rowPos = matrix->rowHead;
        if(rowPos != NULL)
        {
            Sm_Node *element1 = rowPos->rowlist;
            while(element1 != NULL)
            {
                SparseMatrix subMatrix;
                initializeMatrixWithSize(&subMatrix, matrix->rowCount-1, matrix->colCount-1);

                Sm_Node* element2;
                rowPos = matrix->rowHead;
                while(rowPos != NULL) 
                {
                    element2 = rowPos->rowlist;
                    if(element2->row != element1->row)
                    {
                        while(element2 != NULL) 
                        {
                            if(element2->col != element1->col)
                            {
                                int newRow, newCol;
                                if(element2->row > element1->row)
                                {
                                    newRow = element2->row - 1;
                                }
                                else
                                {
                                    newRow = element2->row;
                                }
                                if(element2->col > element1->col)
                                {
                                    newCol = element2->col - 1;
                                }
                                else
                                {
                                    newCol = element2->col;
                                }
                                insertElement(newRow, newCol, element2->data, &subMatrix);
                            }
                            element2 = element2->right;
                        }
                    }
                    rowPos = rowPos->next;
                }

                det += pow(-1, element1->row + element1->col) * element1->data * determinant(&subMatrix);

                clearMatrix(&subMatrix);

                element1 = element1->right;
            }
        }
    }
    return det;
}
status_code determinantOfMatrix(SparseMatrix* matrix, float* result)
{
    status_code sc = SUCCESS;

    if(matrix->rowCount == matrix->colCount)
    {
        *result = determinant(matrix);
    }
    else
    {
        printf("Determinant undefined: Matrix is not square.\n");
        sc = FAILURE;
    }
    return sc;
}

void scalarMultiplyMatrix(SparseMatrix* matrix, float scalar)
{
    Row_Node* rowPos = matrix->rowHead;
    while(rowPos)
    {
        Sm_Node* element = rowPos->rowlist;
        while(element)
        {
            element->data = element->data * scalar;
            element = element->right;
        }
        rowPos = rowPos->next;
    }
}
status_code inverseOfMatrix(SparseMatrix* matrix, SparseMatrix* result)
{
    status_code sc = SUCCESS;
    float det;
    if(determinantOfMatrix(matrix, &det) == SUCCESS)
    {
        if(det == 0)
        {
            sc = FAILURE;
        }
        else
        {
            initializeMatrixWithSize(result, matrix->rowCount, matrix->colCount);

            for(int row = 0; row < matrix->rowCount; row++)
            {
                for(int col = 0; col < matrix->colCount; col++)
                {
                    SparseMatrix subMatrix;
                    initializeMatrixWithSize(&subMatrix, matrix->rowCount-1, matrix->colCount-1);

                    Sm_Node* element2;
                    Row_Node* rowPos = matrix->rowHead;
                    while(rowPos != NULL) 
                    {
                        element2 = rowPos->rowlist;
                        if(element2->row != row)
                        {
                            while(element2 != NULL)
                            {
                                if(element2->col != col)
                                {
                                    int newRow, newCol;
                                    if(element2->row > row)
                                    {
                                        newRow = element2->row - 1;
                                    }
                                    else
                                    {
                                        newRow = element2->row;
                                    }
                                    if(element2->col > col)
                                    {
                                        newCol = element2->col - 1;
                                    }
                                    else
                                    {
                                        newCol = element2->col;
                                    }
                                    insertElement(newRow, newCol, element2->data, &subMatrix);
                                }
                                element2 = element2->right;
                            }
                        }
                        rowPos = rowPos->next;
                    }
                    float subMatrixDet;
                    if(determinantOfMatrix(&subMatrix, &subMatrixDet) == SUCCESS)
                    {
                        if(subMatrixDet != 0)
                        {
                            matrix_entry data = pow(-1, row + col) * subMatrixDet;
                            insertElement(row, col, data, result);
                        }
                    }
                    else
                    {
                        printf("determinant of subMatrix failed");
                        sc = FAILURE;
                    }
                    clearMatrix(&subMatrix);
                }
            }

            transpose(result);
            scalarMultiplyMatrix(result, 1 / det);
        }
    }
    else
    {
        printf("Failed to compute inverse.\n");
        sc = FAILURE;
    }
    return sc;
}
//supportive functions
status_code createMatrix()
{
    status_code sc = SUCCESS;
    char name;
    int rows, cols;

    printf("Enter matrix name (A-J): ");
    scanf(" %c", &name);

    int index = name - 'A';
    if(index < 0 || index >= MAX_MATRICES)
    {
        printf("Invalid matrix name.\n");
        sc = FAILURE;
    }
    else
    {
        if(registry[index].isOccupied)
        {
            printf("Matrix %c already exists. Overwriting...\n", name);
            clearMatrix(&registry[index].matrix);
        }

        printf("Enter number of rows: ");
        scanf("%d", &rows);
        printf("Enter number of columns: ");
        scanf("%d", &cols);

        initializeMatrixWithSize(&registry[index].matrix, rows, cols);
        registry[index].name = name;
        registry[index].isOccupied = TRUE;

        printf("Matrix %c created [%d x %d].\n", name, rows, cols);

        printf("Enter non-zero elements (row col value), or -1 to finish:\n");

        status_code done = FALSE;
        while(done == FALSE)
        {
            int r, c;
            float val;
            printf("> ");
            scanf("%d", &r);

            if(r == -1)
            {
                done = TRUE;
            }
            else
            {
                scanf("%d %f", &c, &val);

                if(r < 0 || r >= rows || c < 0 || c >= cols)
                {
                    printf("Invalid row/column. Please try again.\n");
                }
                else
                {
                    if(insertElement(r, c, val, &registry[index].matrix) != SUCCESS)
                    {
                        printf("Insertion failed.\n");
                    }
                }
            }
        }

        printf("Matrix %c created and initialized.\n", name);
    }
    return sc;
}
void resizeMatrix(SparseMatrix* matrix, int newRowCount, int newColCount)
{
    Row_Node* row = matrix->rowHead;
    Row_Node* prevRow = NULL;

    while(row)
    {
        if(row->row >= newRowCount)
        {
            Row_Node* tempRow = row;
            row = row->next;
            Sm_Node* sptr = tempRow->rowlist;
            matrix_entry d;
            while(sptr) 
            {
                deleteElement(sptr->row, sptr->col, matrix, &d);
                sptr = sptr->right;
            }
            if(prevRow)
            {
                prevRow->next = row;
            }
            else
            {
                matrix->rowHead = row;
            }
            free(tempRow);
        }
        else
        {
            Sm_Node* ele = row->rowlist;
            Sm_Node* prev = NULL;
            while(ele)
            {
                if(ele->col >= newColCount)
                {
                    Sm_Node* temp = ele;
                    ele = ele->right;
                    if(prev)
                    {
                        prev->right = ele;
                    }
                    else
                    {
                        row->rowlist = ele;
                    }
                    free(temp);
                }
                else
                {
                    prev = ele;
                    ele = ele->right;
                }
            }
            prevRow = row;
            row = row->next;
        }
    }

    Col_Node* col = matrix->colHead;
    Col_Node* prevCol = NULL;

    while(col)
    {
        if(col->col >= newColCount)
        {
            Col_Node* tempCol = col;
            col = col->next;
            if(prevCol)
            {
                prevCol->next = col;
            }
            else
            {
                matrix->colHead = col;
            }
            free(tempCol);
        }
        else//just in case
        {
            Sm_Node* ele = col->collist;
            Sm_Node* prev = NULL;
            while(ele)
            {
                if(ele->row >= newRowCount)
                {
                    Sm_Node* temp = ele;
                    ele = ele->down;
                    if (prev) prev->down = ele;
                    else col->collist = ele;
                    free(temp);
                }
                else
                {
                    prev = ele;
                    ele = ele->down;
                }
            }
            prevCol = col;
            col = col->next;
        }
    }

    matrix->rowCount = newRowCount;
    matrix->colCount = newColCount;
}
void resizeMatrixUI()
{
    char name;
    int newRows, newCols;

    printf("Enter matrix name to resize: ");
    scanf(" %c", &name);

    int index = name - 'A';
    if(!registry[index].isOccupied)
    {
        printf("Matrix %c not found.\n", name);
    }
    else
    {
        printf("Enter new row count: ");
        scanf("%d", &newRows);
        printf("Enter new column count: ");
        scanf("%d", &newCols);

        resizeMatrix(&registry[index].matrix, newRows, newCols);

        printf("Matrix %c resized to [%d x %d].\n", name, newRows, newCols);
    }
}
void insertElementUI()
{
    char name;
    int row, col;
    matrix_entry val;

    printf("Enter matrix name to insert into: ");
    scanf(" %c", &name);

    int index = name - 'A';
    if(index < 0 || index >= MAX_MATRICES || !registry[index].isOccupied)
    {
        printf("Matrix %c does not exist.\n", name);
    }
    else
    {
        printf("Enter row index (0-based): ");
        scanf("%d", &row);
        printf("Enter column index (0-based): ");
        scanf("%d", &col);
        printf("Enter value: ");
        scanf("%f", &val);

        if(insertElement(row, col, val, &registry[index].matrix) == SUCCESS)
        {
            printf("Inserted value %.2f at (%d, %d) in matrix %c.\n", val, row, col, name);
        }
        else
        {
            printf("Failed to insert: Check bounds or memory.\n");
        }
    }
}
void deleteElementUI()
{
    char name;
    int row, col;
    float deletedVal;

    printf("Enter matrix name to delete from: ");
    scanf(" %c", &name);

    int idx = name - 'A';
    if(idx < 0 || idx >= MAX_MATRICES || !registry[idx].isOccupied)
    {
        printf("Matrix %c does not exist.\n", name);
    }
    else
    {
        printf("Enter row index (0-based): ");
        scanf("%d", &row);
        printf("Enter column index (0-based): ");
        scanf("%d", &col);

        if(deleteElement(row, col, &registry[idx].matrix, &deletedVal) == SUCCESS)
        {
            printf("Deleted value %.2f from (%d, %d) in matrix %c.\n", deletedVal, row, col, name);
        }
        else
        {
            printf("Element not found or failed to delete.\n");
        }
    }
}
void clearMatrixUI()
{
    char name;
    printf("Enter matrix name to clear: ");
    scanf(" %c", &name);

    int idx = name - 'A';
    if(idx < 0 || idx >= MAX_MATRICES || !registry[idx].isOccupied)
    {
        printf("Matrix %c does not exist.\n", name);
    }
    else
    {
        clearMatrix(&registry[idx].matrix);
        printf("Matrix %c cleared.\n", name);
    }
}

void matrixManagementMenu()
{
    int choice;

    do
    {
        printf("\n===== MATRIX MANAGEMENT =====\n");
        printf("1. Create Matrix\n");
        printf("2. Resize Matrix\n");
        printf("3. Insert/Update Element\n");//should enter non zero element
        printf("4. Delete Element\n");
        printf("5. Clear Matrix\n");
        printf("6. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1: createMatrix(); break;
            case 2: resizeMatrixUI(); break;
            case 3: insertElementUI(); break;
            case 4: deleteElementUI(); break;
            case 5: clearMatrixUI(); break;
            case 6: break;
            default: printf("Invalid choice.\n"); break;
        }
    }while(choice != 6);
}
SparseMatrix* getMatrixByName(char name)
{
    SparseMatrix* ans = NULL;
    int idx = name - 'A';
    if(!(idx < 0 || idx >= MAX_MATRICES || !registry[idx].isOccupied))
    {
        ans = &registry[idx].matrix;
    }
    return ans;
}
void copyMatrix(char destName, SparseMatrix* source)
{
    int index = destName - 'A';
    if(index < 0 || index >= MAX_MATRICES)
    {
        printf("Invalid destination name.\n");
    }
    else
    {
        SparseMatrix *dest = &registry[index].matrix;

        if(registry[index].isOccupied)
        {
            printf("Matrix %c already exists. Overwriting...\n", destName);
            clearMatrix(dest);
        }

        initializeMatrixWithSize(dest, source->rowCount, source->colCount);
        registry[index].name = destName;
        registry[index].isOccupied = TRUE;

        Row_Node* rptr = source->rowHead;
        while(rptr)
        {
            Sm_Node* sptr = rptr->rowlist;
            while(sptr)
            {
                insertElement(sptr->row, sptr->col, sptr->data, dest);
                sptr = sptr->right;
            }
            rptr = rptr->next;
        }

        printf("Matrix copied to '%c'\n", destName);
    }
}
void executeCommand(const char* input)
{
    status_code sc = SUCCESS;
    char op[20], Aname, Bname;
    float scalar;
    int res;

    res = sscanf(input, "%s %c %f", op, &Aname, &scalar);
    if(res == 3 && strcmp(op, "scalar") == 0)
    {
        SparseMatrix* A = getMatrixByName(Aname);
        if(!A)
        {
            printf("Matrix %c does not exist.\n", Aname);
            sc = FAILURE;
        }
        else
        {
            scalarMultiplyMatrix(A, scalar);
            printNamedMatrix(A, Aname, FULL_VIEW);
        }
        return;
    }

    res = sscanf(input, "%s %c %c", op, &Aname, &Bname);
    if(res == 3)
    {
        SparseMatrix *A = getMatrixByName(Aname);
        SparseMatrix *B = getMatrixByName(Bname);
        if(!A || !B)
        {
            printf("Invalid matrix name(s).\n");
            sc = FAILURE;
        }
        else
        {
            SparseMatrix result;
            initializeMatrixWithSize(&result, A->rowCount, B->colCount);

            if(strcmp(op, "add") == 0)
            {
                if (addMatrix(A, B, &result) == SUCCESS)
                {
                    printNamedMatrix(&result, 'R', FULL_VIEW);
                }
                else
                {
                    printf("Addition failed.\n");
                    sc = FAILURE;
                }
            }
            else if(strcmp(op, "subtract") == 0)
            {
                if (subtractMatrix(A, B, &result) == SUCCESS)
                {
                    printNamedMatrix(&result, 'R', FULL_VIEW);
                }
                else
                {
                    printf("Subtraction failed.\n");
                    sc = FAILURE;
                }
            }
            else if(strcmp(op, "multiply") == 0)
            {
                if (multiplyMatrix(A, B, &result) == SUCCESS)
                {
                    printNamedMatrix(&result, 'R', FULL_VIEW);
                }
                else
                {
                    printf("Multiplication failed.\n");
                    sc = FAILURE;
                }
            }
            else
            {
                printf("Unsupported binary operation: %s\n", op);
                sc = FAILURE;
            }
            if(sc == SUCCESS)
            {
                char choice, name;
                status_code sc1 = FAILURE;

                printf("Do you want to save the resultant matrix before deleting? (y/n): ");
                scanf(" %c", &choice);

                if(choice == 'y' || choice == 'Y')
                {
                    while(sc1 == FAILURE)
                    {
                        printf("Enter a destination matrix name (A-J): ");
                        scanf(" %c", &name);
                        if(name >= 'A' && name <= 'J')
                        {
                            copyMatrix(name, &result);
                            sc1 = SUCCESS;
                        }
                        else
                        {
                            printf("Invalid matrix name.\n");
                        }
                    }
                }
            }
            clearMatrix(&result);
        }
        return;
    }

    res = sscanf(input, "%s %c", op, &Aname);
    if(res == 2)
    {
        SparseMatrix *A = getMatrixByName(Aname);
        if(!A)
        {
            printf("Matrix %c does not exist.\n", Aname);
        }
        else
        {
            if(strcmp(op, "transpose") == 0)
            {
                if(transpose(A) == SUCCESS)
                {
                    printNamedMatrix(A, Aname, FULL_VIEW);
                }
                else
                {
                    printf("Transpose failed.\n");
                }
            }
            else if(strcmp(op, "determinant") == 0)
            {
                float det;
                if(determinantOfMatrix(A, &det) == SUCCESS)
                {
                    printf("Determinant of %c = %.2f\n", Aname, det);
                }
                else
                {
                    printf("Failed to compute determinant.\n");
                }
            }
            else if(strcmp(op, "inverse") == 0)
            {
                SparseMatrix inv;
                initializeMatrix(&inv);
                if(inverseOfMatrix(A, &inv) == SUCCESS)
                {
                    printNamedMatrix(&inv, 'R', FULL_VIEW);
                }
                else
                {
                    printf("Matrix not invertible.\n");
                    sc = FAILURE;
                }
                if(sc == SUCCESS)
                {
                    char choice, name;
                    status_code sc = FAILURE;

                    printf("Do you want to save the inverse matrix before deleting? (y/n): ");
                    scanf(" %c", &choice);

                    if(choice == 'y' || choice == 'Y')
                    {
                        while(sc == FAILURE)
                        {
                            printf("Enter a destination matrix name (A-J): ");
                            scanf(" %c", &name);
                            if(name >= 'A' && name <= 'J')
                            {
                                copyMatrix(name, &inv);
                                sc = SUCCESS;
                            }
                            else
                            {
                                printf("Invalid matrix name.\n");
                            }
                        }
                    }
                }
                clearMatrix(&inv);
            }
            else
            {
                printf("Unsupported unary operation: %s\n", op);
            }
        }
        return;
    }

    printf("Invalid command format.\n");
}
void operationsMenu()
{
    char input[100];
    status_code done = FALSE;
    while(done == FALSE)
    {
        printf("\n===== OPERATION COMMAND MODE =====\n");
        printf("Type operations like:\n");
        printf("  add A B\n  transpose A\n  determinant A\n  scalar A 2.5\n  exit\n");
        printf("> Operation: ");

        fgets(input, sizeof(input), stdin);

        if(strncmp(input, "exit", 4) == 0)
        {
            done = TRUE;
        }
        else
        {
            executeCommand(input);
        }
    }
}
void displayOptionsMenu() {
    int choice;

    do
    {
        printf("\n===== DISPLAY OPTIONS =====\n");
        printf("1. List All Matrices\n");
        printf("2. Print Matrix (Full View)\n");
        printf("3. Print Matrix (Sparse View)\n");
        printf("4. Print Dimensions Only\n");
        printf("5. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
            {
                printf("\n=== Matrix Registry ===\n");
                for(int i = 0; i < MAX_MATRICES; ++i)
                {
                    if(registry[i].isOccupied)
                    {
                        printf("%c : [%d x %d] \n",
                            registry[i].name,
                            registry[i].matrix.rowCount,
                            registry[i].matrix.colCount
                        );
                    }
                    else
                    {
                        printf("%c : (empty)\n", 'A' + i);
                    }
                }
                break;
            }
            case 2: //Full View
            case 3:
            {
                char name;
                int mode = (choice == 2) ? FULL_VIEW : SPARSE_VIEW;

                printf("Enter matrix name to print: ");
                scanf(" %c", &name);

                int idx = name - 'A';
                if(idx < 0 || idx >= MAX_MATRICES || !registry[idx].isOccupied)
                {
                    printf("Matrix %c does not exist.\n", name);
                }
                else
                {
                    printNamedMatrix(&registry[idx].matrix, name, mode);
                }
                break;
            }
            case 4:
            {
                char name;
                printf("Enter matrix name: ");
                scanf(" %c", &name);

                int idx = name - 'A';
                if(idx < 0 || idx >= MAX_MATRICES || !registry[idx].isOccupied)
                {
                    printf("Matrix %c does not exist.\n", name);
                }
                else
                {
                    printf("Matrix %c: %d rows x %d columns\n",
                        name,
                        registry[idx].matrix.rowCount,
                        registry[idx].matrix.colCount
                    );
                }
                break;
            }
            case 5: break;;
            default: printf("Invalid choice.\n"); break;
        }
    }while(choice != 5);
}
void initializeRegistry()
{
    for(int i = 0; i < MAX_MATRICES; i++)
    {
        registry[i].name = '\0';
        initializeMatrix(&registry[i].matrix);
        registry[i].isOccupied = FALSE;
    }
}
void freeAllMatrices()
{
    for(int i = 0; i < MAX_MATRICES; i++)
    {
        if(registry[i].isOccupied)
        {
            clearMatrix(&registry[i].matrix);
        }
    }
}
int main()
{
    initializeRegistry();

    int choice;
    do
    {
        printf("\n===== MATRIX CALCULATOR =====\n");
        printf("1. Matrix Management\n");
        printf("2. Matrix Operations\n");
        printf("3. Display Options\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                matrixManagementMenu();
                break;
            case 2:
                operationsMenu();
                break;
            case 3:
                displayOptionsMenu();
                break;
            case 4:
                freeAllMatrices();
                printf("Exiting Matrix Calculator. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }while(choice != 4);

    return 0;
}