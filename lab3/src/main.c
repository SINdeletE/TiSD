#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <stdbool.h>

#include "matrix_tools.h"
#include "vector_tools.h"

#define IO_OK 0
#define IO_ERR_INVALID_DATA 1

// ---
#define CODE_INIT 1

#define CODE_AUTO_INIT 1
#define CODE_SELF_INIT 2
// ---

#define CODE_OUTPUT 2

#define CODE_EXIT 5

int clear_buf(FILE *f);

int main(void)
{
    matrix_t matrix = (matrix_t){NULL, NULL, NULL, 0};
    vector_t vector = (vector_t){NULL, NULL, 0};

    size_t m = 0;
    size_t n = 0;

    bool flag = true;
    int code;
    // int func_code = 0;

    // --- INIT

    int percent;

    // ---

    while (flag)
    {
        printf("\nEnter a number of command:\n");
        printf("1. Initialize matrix and vector\n");
        printf("2. Output matrix (sparse)\n");

        printf("5. Exit program\n");

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
                printf("\nCode: ");
                if (scanf("%d", &code) != 1 || clear_buf(stdin))
                {
                    printf("\nINVALID CODE\n");

                    break;
                }

                switch (code)
                {
                    case CODE_AUTO_INIT:
                        printf("\nEnter a percent of fill: ");
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

                        matrix_free(&matrix);
                        vector_free(&vector);

                        switch (vector_autoinit(&vector, m, percent))
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
                        
                        switch (matrix_autoinit(&matrix, m, n, percent))
                        {
                            case MAT_INIT_ERR_ALLOC:
                                printf("\nCOMPUTER CAN'T AUTOINITIALIZE MATRIX\n");

                                break;
                            case MAT_INIT_ERR_FILL:
                                printf("\nTOO LOW PERSENT. PLEASE, TRY AGAIN\n");

                                break;
                            default:
                                printf("\nMATRIX WAS INITIALIZED SUCCESSFULLY!\n");
                        }                        

                        break;
                    case CODE_SELF_INIT:
                        

                        break;
                    default:
                        printf("\nINVALID CODE\n");
                }

                break;
            case CODE_OUTPUT:
                if (matrix.A != NULL)
                    matrix_output_sparse(&matrix);
                else
                    printf("\nNO MATRIX\n");

                break;
            case CODE_EXIT:
                matrix_free(&matrix);
                vector_free(&vector);

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
