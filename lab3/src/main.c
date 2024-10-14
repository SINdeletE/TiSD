#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <stdbool.h>

#include "matrix_tools.h"
#include "vector_tools.h"
#include "operations.h"

#define IO_OK 0
#define IO_ERR_INVALID_DATA 1

// ---
#define CODE_INIT 1

#define CODE_AUTO_INIT 1
#define CODE_SELF_INIT 2
#define CODE_AUTO_SELF_INIT 3
// ---

// ---
#define CODE_OUTPUT 2

#define CODE_OUTPUT_SPARSE 1
#define CODE_OUTPUT_USUAL 2
// ---

#define CODE_SPARSE_MULTIPLICATION 3
#define CODE_MATRIX_MULTIPLICATION 4

#define CODE_STAT 5

#define CODE_EXIT 6

int clear_buf(FILE *f);

int main(void)
{
    // -----------------------------------------------------------------------------------

    // bool matrix_is_vector = false;

    sparse_t sparse = (sparse_t){NULL, NULL, NULL, 0, 0};
    matrix_t matrix = (matrix_t){NULL, 0, 0};

    // ---

    vector_str_t matrix_vector_str = (vector_str_t){NULL, NULL, 0, 0};
    vector_t matrix_vector = (vector_t){NULL, 0};

    // ---

    // -----------------------------------------------------------------------------------

    vector_str_t vector_str = (vector_str_t){NULL, NULL, 0, 0};
    vector_str_t result_vector_str = (vector_str_t){NULL, NULL, 0, 0};

    vector_t vector = (vector_t){NULL, 0};
    vector_t result_vector = (vector_t){NULL, 0};

    size_t m = 0;
    size_t n = 0;

    bool flag = true;
    int code;
    int func_code = 0;

    // --- INIT

    int percent;

    // ---

    while (flag)
    {
        printf("\nEnter a number of command:\n");
        printf("1. Initialize matrix and vector\n");
        printf("2. Output matrix (sparse) and vector (str format)\n");
        printf("3. Count vector str and sparse multiplication\n");
        printf("4. Count vector and matrix multiplication\n");

        printf("\n5. Get statistics\n");

        printf("\n6. Exit program\n");

        printf("Code: ");
        if (scanf("%d", &code) != 1)
            code = 0;
            
        if (clear_buf(stdin))
            code = 0;

        switch(code)
        {
            case CODE_INIT:
                printf("\nMATRIX\n");
                printf("Input matrix strokes count (it must be >= 2): ");
                if (scanf("%zu", &m) != 1 || clear_buf(stdin) || m < 2)
                {
                    printf("\nINVALID COUNT\n");

                    break;
                }
                printf("\nInput matrix columns count (it must be >= 2): ");
                if (scanf("%zu", &n) != 1 || clear_buf(stdin) || n < 2)
                {
                    printf("\nINVALID COUNT\n");

                    break;
                }

                printf("\nVECTOR\n");
                printf("Vector parameters: 1 x %zu\n", m);

                printf("\nEnter init method:\n");
                printf("1. Auto\n");
                printf("2. Self initialize\n");
                printf("3. Auto self initialize\n");
                printf("\nCode: ");
                if (scanf("%d", &code) != 1 || clear_buf(stdin))
                {
                    printf("\nINVALID CODE\n");

                    break;
                }

                switch (code)
                {
                    case CODE_AUTO_INIT:
                        printf("\nEnter a percent of fill for sparse matrix: ");
                        if (scanf("%d", &percent) != 1 || clear_buf(stdin))
                        {
                            printf("\nINVALID PERCENT\n");

                            break;
                        }
                        // if percent < 1% && percent > 100%
                        if (percent < 1 || percent > 100)
                        {
                            printf("\nINVALID PERCENT\n");

                            break;
                        }

                        // MATRIX INIT

                        matrix_free(&matrix);
                        
                        switch (func_code = matrix_autoinit(&matrix, m, n, percent))
                        {
                            case MAT_INIT_ERR_ALLOC:
                                printf("\nCOMPUTER CAN'T AUTOINITIALIZE MATRIX\n");

                                break;
                            default:
                                printf("\nMATRIX WAS INITIALIZED SUCCESSFULLY!\n");
                        }

                        if (func_code)
                        {
                            matrix_free(&matrix);

                            break;
                        }

                        // MATRIX TO SPARSE CONVERTATION

                        sparse_free(&sparse);

                        switch (func_code = matrix_to_sparse(&matrix, &sparse))
                        {
                            case MAT_INIT_ERR_ALLOC:
                                printf("\nCOMPUTER CAN'T CONVERT TO SPARSE\n");

                                break;
                            case MAT_INIT_ERR_FILL:
                                printf("ERR PERCENT!");

                                break;
                            default:
                                printf("\nMATRIX WAS CONVERTED TO SPARSE SUCCESSFULLY!\n");
                        }

                        if (func_code)
                        {
                            matrix_free(&matrix);

                            break;
                        }

                        // VECTOR INIT

                        printf("\nEnter a percent of fill for vector: ");
                        if (scanf("%d", &percent) != 1 || clear_buf(stdin))
                        {
                            printf("\nINVALID PERCENT\n");

                            break;
                        }
                        // if percent < 1% && percent > 100%
                        if (percent < 1 || percent > 100)
                        {
                            printf("\nINVALID PERCENT\n");

                            break;
                        }

                        vector_free(&vector);

                        switch (func_code = vector_autoinit(&vector, m, percent))
                        {
                            case VEC_INIT_ERR_ALLOC:
                                printf("\nCOMPUTER CAN'T AUTOINITIALIZE VECTOR\n");

                                break;
                            case VEC_INIT_ERR_FILL:
                                printf("\nTOO LOW PERSENT. PLEASE, TRY AGAIN\n");

                                break;
                            default:
                                printf("\nVECTOR WAS INITIALIZED SUCCESSFULLY!\n");
                        }

                        if (func_code)
                        {
                            sparse_free(&sparse);
                            matrix_free(&matrix);

                            vector_free(&vector);

                            break;
                        }

                        // VECTOR TO VECTOR STR CONVERTATION

                        switch (func_code = vector_to_vector_str(&vector, &vector_str))
                        {
                            case VEC_CONVERT_ERR_ALLOC:
                                printf("\nCOMPUTER CAN'T ALLOC MEMORY FOR VECTOR\n");

                                break;
                            default:
                                printf("\nVECTOR WAS CONVERTED SUCCESSFULLY!\n");
                        }

                        if (func_code)
                        {
                            sparse_free(&sparse);
                            vector_str_free(&vector_str);

                            matrix_free(&matrix);
                            vector_free(&vector);

                            break;
                        }

                        break;
                    case CODE_SELF_INIT:
                        // MATRIX INIT

                        printf("\nEnter matrix elements:\n");
                        switch (matrix_init_manual(&matrix, m, n))
                        {
                            case MAT_INIT_ERR_ALLOC:
                                printf("\nCOMPUTER CAN'T INITIALIZE MATRIX\n");

                                break;
                            case MAT_INIT_ERR_FILL:
                                printf("\nENTERED INVALID DATA\n");

                                break;
                            case MAT_INIT_ERR_ZERO_MATRIX:
                                printf("\nMATRIX CAN'T BE ZERO\n");

                                break;
                            default:
                                printf("\nMATRIX WAS INITIALIZED SUCCESSFULLY!\n");
                        }

                        if (func_code)
                        {
                            matrix_free(&matrix);

                            break;
                        }

                        // MATRIX TO SPARSE CONVERTATION

                        sparse_free(&sparse);

                        switch (func_code = matrix_to_sparse(&matrix, &sparse))
                        {
                            case MAT_INIT_ERR_ALLOC:
                                printf("\nCOMPUTER CAN'T CONVERT TO SPARSE\n");

                                break;
                            case MAT_INIT_ERR_FILL:
                                printf("ERR PERCENT!");

                                break;
                            default:
                                printf("\nMATRIX WAS CONVERTED TO SPARSE SUCCESSFULLY!\n");
                        }

                        if (func_code)
                        {
                            sparse_free(&sparse);
                            matrix_free(&matrix);

                            break;
                        }

                        printf("\nEnter vector elements:\n");
                        switch (func_code = vector_init_manual(&vector, m))
                        {
                            case VEC_INIT_ERR_ALLOC:
                                printf("\nCOMPUTER CAN'T INITIALIZE VECTOR\n");

                                break;
                            case VEC_INIT_ERR_FILL:
                                printf("\nENTERED INVALID DATA\n");

                                break;
                            case VEC_INIT_ERR_ZERO_VECTOR:
                                printf("\nVECTOR CAN'T BE ZERO\n");

                                break;
                            default:
                                printf("\nVECTOR WAS INITIALIZED SUCCESSFULLY!\n");
                        }

                        if (func_code)
                        {
                            sparse_free(&sparse);
                            matrix_free(&matrix);

                            vector_free(&vector);
                            break;
                        }

                         // VECTOR TO VECTOR STR CONVERTATION

                        switch (func_code = vector_to_vector_str(&vector, &vector_str))
                        {
                            case VEC_CONVERT_ERR_ALLOC:
                                printf("\nCOMPUTER CAN'T ALLOC MEMORY FOR VECTOR\n");

                                break;
                            default:
                                printf("\nVECTOR WAS CONVERTED SUCCESSFULLY!\n");
                        }

                        if (func_code)
                        {
                            sparse_free(&sparse);
                            vector_str_free(&vector_str);

                            matrix_free(&matrix);
                            vector_free(&vector);

                            break;
                        }

                        break;
                    default:
                        printf("\nINVALID CODE\n");
                    
                        break;
                    case CODE_AUTO_SELF_INIT:
                        printf("\nEnter a percent of fill for sparse matrix: ");
                        if (scanf("%d", &percent) != 1 || clear_buf(stdin))
                        {
                            printf("\nINVALID PERCENT\n");

                            break;
                        }
                        // if percent < 1% && percent > 100%
                        if (percent < 1 || percent > 100)
                        {
                            printf("\nINVALID PERCENT\n");

                            break;
                        }

                        // MATRIX INIT

                        matrix_free(&matrix);
                        
                        switch (func_code = matrix_autoinit_by_user(&matrix, m, n, percent))
                        {
                            case MAT_INIT_ERR_ALLOC:
                                printf("\nCOMPUTER CAN'T AUTOINITIALIZE MATRIX\n");

                                break;
                            case MAT_INIT_ERR_INVALID_ENTERED_DATA:
                                printf("\nENTERED INVALID DATA\n");

                                break;
                            default:
                                printf("\nMATRIX WAS INITIALIZED SUCCESSFULLY!\n");
                        }

                        if (clear_buf(stdin))
                            func_code = MAT_INIT_ERR_INVALID_ENTERED_DATA;

                        if (func_code)
                        {
                            matrix_free(&matrix);

                            break;
                        }

                        // MATRIX TO SPARSE CONVERTATION

                        sparse_free(&sparse);

                        switch (func_code = matrix_to_sparse(&matrix, &sparse))
                        {
                            case MAT_INIT_ERR_ALLOC:
                                printf("\nCOMPUTER CAN'T CONVERT TO SPARSE\n");

                                break;
                            case MAT_INIT_ERR_FILL:
                                printf("ERR PERCENT!");

                                break;
                            default:
                                printf("\nMATRIX WAS CONVERTED TO SPARSE SUCCESSFULLY!\n");
                        }

                        if (func_code)
                        {
                            matrix_free(&matrix);

                            break;
                        }

                        // VECTOR INIT

                        printf("\nEnter a percent of fill for vector: ");
                        if (scanf("%d", &percent) != 1 || clear_buf(stdin))
                        {
                            printf("\nINVALID PERCENT\n");

                            break;
                        }
                        // if percent < 1% && percent > 100%
                        if (percent < 1 || percent > 100)
                        {
                            printf("\nINVALID PERCENT\n");

                            break;
                        }

                        vector_free(&vector);

                        switch (func_code = vector_autoinit_by_user(&vector, m, percent))
                        {
                            case VEC_INIT_ERR_ALLOC:
                                printf("\nCOMPUTER CAN'T AUTOINITIALIZE VECTOR\n");

                                break;
                            case VEC_INIT_ERR_FILL:
                                printf("\nTOO LOW PERSENT. PLEASE, TRY AGAIN\n");

                                break;
                            case VEC_INIT_ERR_INVALID_ENTERED_DATA:
                                printf("\nENTERED INVALID DATA\n");

                                break;
                            default:
                                printf("\nVECTOR WAS INITIALIZED SUCCESSFULLY!\n");
                        }

                        if (clear_buf(stdin))
                            func_code = MAT_INIT_ERR_INVALID_ENTERED_DATA;

                        if (func_code)
                        {
                            sparse_free(&sparse);
                            matrix_free(&matrix);

                            vector_free(&vector);

                            break;
                        }

                        // VECTOR TO VECTOR STR CONVERTATION

                        switch (func_code = vector_to_vector_str(&vector, &vector_str))
                        {
                            case VEC_CONVERT_ERR_ALLOC:
                                printf("\nCOMPUTER CAN'T ALLOC MEMORY FOR VECTOR\n");

                                break;
                            default:
                                printf("\nVECTOR WAS CONVERTED SUCCESSFULLY!\n");
                        }

                        if (func_code)
                        {
                            sparse_free(&sparse);
                            vector_str_free(&vector_str);

                            matrix_free(&matrix);
                            vector_free(&vector);

                            break;
                        }

                        break;
                }

                // VECTOR INIT 



                break;
            case CODE_OUTPUT:
                if (sparse.A == NULL || vector_str.B == NULL)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("\nEnter output method:\n");
                printf("1. Sparse output\n");
                printf("2. Usual output\n");
                printf("\nCode: ");
                if (scanf("%d", &code) != 1 || clear_buf(stdin))
                {
                    printf("\nINVALID CODE\n");

                    break;
                }

                switch (code)
                {
                    case CODE_OUTPUT_SPARSE:
                        printf("\nMATRIX\n");
                        sparse_output(&sparse, &vector_str);

                        printf("\nVECTOR\n");
                        vector_str_output(&vector_str);

                        break;
                    case CODE_OUTPUT_USUAL:
                        printf("\nMATRIX\n");
                        sparse_output_usual(&sparse, &vector_str);

                        printf("\nVECTOR\n");
                        vector_str_output_usual(&vector_str);

                        break;
                    default:
                        printf("\nINVALID OUTPUT CODE!\n");
                }

                    break;
            case CODE_SPARSE_MULTIPLICATION:
                if (sparse.A == NULL || vector_str.B == NULL)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                if (vector_str_sparse_multiplic(&result_vector_str, &vector_str, &sparse))
                {
                    printf("\nCOMPUTER CAN'T ALLOC NEEDED DATA\n");

                    break;
                }

                printf("\nRESULT VECTOR\n");
                vector_str_output(&result_vector_str);

                break;

            case CODE_MATRIX_MULTIPLICATION:
                if (matrix.strs == NULL || vector.coords == NULL)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                if (vector_matrix_multiplic(&result_vector, &vector, &matrix))
                {
                    printf("\nCOMPUTER CAN'T ALLOC NEEDED DATA\n");

                    break;
                }

                printf("\nRESULT VECTOR\n");
                vector_output(&result_vector);

                break;
            case CODE_STAT:
                printf("Input matrix strokes count (it must be >= 2): ");
                if (scanf("%zu", &m) != 1 || clear_buf(stdin) || m < 2)
                {
                    printf("\nINVALID COUNT\n");

                    break;
                }
                printf("\nInput matrix columns count (it must be >= 2): ");
                if (scanf("%zu", &n) != 1 || clear_buf(stdin) || n < 2)
                {
                    printf("\nINVALID COUNT\n");

                    break;
                }

                switch (func_code = vector_matrix_statistics(m, n))
                {
                case MULTI_STAT_ERR_ALLOC:
                    printf("\nCOMPUTER CAN'T ALLOC MEMORY FOR STATISTICS GENERATION\n");
                    
                    break;
                }

                break;
            case CODE_EXIT:
                sparse_free(&sparse);
                vector_str_free(&vector_str);
                vector_str_free(&result_vector_str);

                matrix_free(&matrix);
                vector_free(&vector);
                vector_free(&result_vector);

                flag = false;

                break;
            default:
                printf("\nINVALID CODE\n");
        }
    }

    return IO_OK;
}

int clear_buf(FILE *f)
{
    int ch;

    do
    {
        ch = getc(f);
        if (! isspace(ch) && ch != EOF)
            return 1;
    } while (ch != '\n' && ch != EOF);

    return 0;
}
