#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "apart_tools.h"
#include "aparts_tools.h"
#include "string_tools.h"

#define IO_OK 0
#define IO_ERR 1

#define CODE_READ 1
#define CODE_SHOW 2
#define CODE_ADD 3
#define CODE_DELETE 4

// ---
#define CODE_SORT 5

#define CODE_INSERTION_SORT 1
#define CODE_GNOME_SORT 2

#define CODE_ADDRESS 1
#define CODE_SQUARE 2
#define CODE_ROOM_COUNT 3
#define CODE_QUAD_METER_COST 4
//---

#define CODE_BY_PRICE 6
#define CODE_STATISTICS 7
#define CODE_OUTPUT_BY_KEYS 8

#define CODE_SAVE 9
#define CODE_EXIT 10

void keys_free(keystat_t **keys);

int main(void)
{
    apart_t *aparts = NULL;
    size_t size = 0;

    bool flag = true;
    int code;
    int func_code = 0;

    // Add/Delete
    size_t index = 0;

    // Sort
    int (*sort)(void *base, size_t num, size_t size, int (*compare)(const void *, const void *, void *(*)(apart_t *)), void *(*key)(apart_t *)) = NULL;
    int (*comparator)(const void *, const void *, void *(*)(apart_t *)) = NULL;
    void *(*key)(apart_t *) = NULL;

    // Get secondary aparts by price
    double min_price = 0.0;
    double max_price = 0.0;

    // KEY
    keystat_t *keys = NULL;

    while (flag)
    {
        printf("\nEnter a number of command:\n");
        printf("1. Read File\n");
        printf("2. Show table\n");
        printf("3. Add apart\n");
        printf("4. Delete apart\n");
        printf("5. Sort table\n");
        printf("6. Get secondary aparts by price without animals with 2 rooms\n");
        printf("7. Get sorts statistics\n");
        printf("8. Output aparts by keys (AFTER SORT STATISTICS)\n");

        printf("\n9. Save file\n");
        printf("10. Exit program\n");

        printf("Code: ");
        if (scanf("%d", &code) != 1)
            code = 0;
            
        if (clear_buf(stdin))
            code = 0;

        switch(code)
        {
            case CODE_READ:
                printf("\nEnter a filename: ");
                func_code = aparts_read(&aparts, &size);

                switch (func_code)
                {
                    case READ_ERR_INVALID_FILENAME:
                        printf("\nINVALID FILENAME VALUE\n");

                        break;
                    case READ_ERR_NO_FILE:
                        printf("\nTHERE IS NO FILE WITH THIS FILENAME\n");

                        break;
                    case READ_ERR_INVALID_FILE:
                        printf("\nINVALID FILE WORKING\n");

                        break;
                    case READ_ERR_DATA_ALLOC:
                        printf("\nCOMPUTER CAN'T SAVE DATA\n");

                        break;
                    case READ_ERR_INVALID_APART:
                        printf("\nINVALID APART VALUE\n");

                        break;
                    case READ_ERR_NO_DATA:
                        printf("\nNO DATA IN FILE\n");

                        break;
                    default:
                        keys_free(&keys);

                        printf("\nDATA WAS READ SUCCESSFULLY!\n");
                }   

                break;
            case CODE_SHOW:
                if (aparts != NULL && size != 0)
                {
                    printf("\n\n");
                    aparts_output(aparts, size);
                }
                else
                    printf("\nNO DATA\n");

                break;
            case CODE_ADD:
                printf("Enter apart index (starts from 1): ");

                if (scanf("%zu", &index) != 1 || index < 1 || index > size + 1)
                {
                    printf("\nINVALID INDEX\n");

                    break;
                }

                if (clear_buf(stdin))
                {
                    printf("\nINVALID INDEX!\n");

                    break;
                }

                func_code = aparts_add(&aparts, &size, index);

                switch (func_code)
                {
                    case APART_ADD_ERR_INVALID_DATA_BUF:
                        printf("\nENTERED DATA IS INVALID!\n");
                        
                        stdin_clear_buf();

                        break;
                    case APART_ADD_ERR_INVALID_DATA_NO_BUF:
                        printf("\nENTERED DATA IS INVALID!\n");

                        break;
                    case APART_ADD_ERR_DATA_ALLOC:
                        printf("\nCOMPUTER CAN'T ADD DATA\n");

                        break;
                    default:
                        keys_free(&keys);

                        printf("\nDATA WAS ADDED SUCCESSFULLY!\n");
                }

                break;
            case CODE_DELETE:
                if (size == 0)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("Enter apart index (starts from 1): ");

                if (scanf("%zu", &index) != 1 || index < 1 || index > size + 1)
                {
                    printf("\nINVALID INDEX\n");

                    break;
                }

                if (clear_buf(stdin))
                {
                    printf("\nINVALID INDEX!\n");

                    break;
                }

                func_code = aparts_delete(&aparts, &size, index - 1);

                switch (func_code)
                {
                    case APART_DELETE_ERR_NO_DATA:
                        printf("\nNOTHING TO DELETE\n");

                        break;
                    case APART_DELETE_ERR_DATA_ALLOC:
                        printf("\nCOMPUTER CAN'T DELETE DATA\n");

                        break;
                    default:
                        keys_free(&keys);

                        printf("\nAPART WAS DELETED SUCCESSFULLY!\n");
                }

                break;

            case CODE_SORT:
                if (size == 0)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("\n1. Insertion sort\n");
                printf("2. Gnome sort\n");
                printf("Enter sort algorithm code: ");
                
                if (scanf("%d", &func_code) != 1)
                    func_code = 0;

                if (clear_buf(stdin))
                {
                    printf("\nINVALID SORT CODE!\n");

                    break;
                }

                switch (func_code)
                {
                    case CODE_INSERTION_SORT:
                        sort = insertion_sort;

                        break;
                    case CODE_GNOME_SORT:
                        sort = gnome_sort;

                        break;
                    default:
                        sort = NULL;
                }

                if (!sort)
                {
                    printf("\nINVALID SORT CODE!\n");

                    break;
                }

                printf("\n1. Address\n");
                printf("2. Square\n");
                printf("3. Room count\n");
                printf("4. Quad meter cost\n");
                printf("Enter sort algorithm code: ");
                
                if (scanf("%d", &func_code) != 1)
                    func_code = 0;

                if (clear_buf(stdin))
                    func_code = 0;

                switch (func_code)
                {
                    case CODE_ADDRESS:
                        comparator = aparts_strcmp;
                        key = (void *(*)(apart_t *))address;
                        
                        if (sort(aparts, size, sizeof(apart_t), comparator, key))
                            printf("\nCOMPUTER CAN'T SORT\n");
                        else
                        {
                            keys_free(&keys);

                            printf("\nAPART WAS SORTED SUCCESSFULLY!\n");
                        }
                    
                        break;
                    case CODE_SQUARE:
                        comparator = aparts_doublecmp;
                        key = (void *(*)(apart_t *))p_square;
                        
                        if (sort(aparts, size, sizeof(apart_t), comparator, key))
                            printf("\nCOMPUTER CAN'T SORT\n");
                        else
                        {
                            keys_free(&keys);
                            
                            printf("\nAPART WAS SORTED SUCCESSFULLY!\n");
                        }
                    
                        break;
                    case CODE_ROOM_COUNT:
                        comparator = aparts_intcmp;
                        key = (void *(*)(apart_t *))p_room_count;
                        
                        if (sort(aparts, size, sizeof(apart_t), comparator, key))
                            printf("\nCOMPUTER CAN'T SORT\n");
                        else
                        {
                            keys_free(&keys);
                            
                            printf("\nAPART WAS SORTED SUCCESSFULLY!\n");
                        }
                    
                        break;
                    case CODE_QUAD_METER_COST:
                        comparator = aparts_doublecmp;
                        key = (void *(*)(apart_t *))p_quad_meter_cost;
                        
                        if (sort(aparts, size, sizeof(apart_t), comparator, key))
                            printf("\nCOMPUTER CAN'T SORT\n");
                        else
                        {
                            keys_free(&keys);
                            
                            printf("\nAPART WAS SORTED SUCCESSFULLY!\n");
                        }
                    
                        break;
                    default:
                        printf("\nINVALID KEY CODE!\n");
                }

                break;
            case CODE_BY_PRICE:
                if (size == 0)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("\nEnter min price: ");
                if (scanf("%lf", &min_price) != 1 || min_price < -EPS)
                {
                    printf("\nINVALID MIN VALUE\n");

                    break;
                }

                if (clear_buf(stdin))
                {
                    printf("\nINVALID MIN VALUE\n");

                    break;
                }

                printf("Enter max price: ");
                if (scanf("%lf", &max_price) != 1 || max_price < -EPS)
                {
                    printf("\nINVALID MAX VALUE\n");

                    break;
                }

                if (clear_buf(stdin))
                {
                    printf("\nINVALID MAX VALUE\n");

                    break;
                }

                if (max_price - min_price < -EPS)
                {
                    printf("\nMAX PRICE CAN'T BE LOWER THAN MIN PRICE\n");

                    break;
                }

                printf("\n");
                switch (aparts_output_by_price(aparts, size, min_price, max_price))
                {
                    case APART_BY_PRICE_ERR_NO_SECONDARY:
                        printf("\nNO SECONDARY APARTS WITHOUT ANIMALS WITH 2 ROOMS IN FILE\n");

                        break;
                    case APART_BY_PRICE_ERR_NO_DATA:
                        printf("\nNO DATA TO OUTPUT\n");

                        break;
                    
                    default:
                        printf("\nAPARTS FOUNDED!\n");
                }

                break;
            case CODE_STATISTICS:
                if (size == 0)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                if (keys_create(aparts, size, &keys))
                {
                    printf("\nCOMPUTER COULDN't ALLOC MEMORY!\n");

                    break;
                }
                
                if (statistics_get(aparts, size, keys))
                {
                    printf("\nCOMPUTER COULDN't ALLOC MEMORY!\n");

                    break;
                }

                break;
            case CODE_OUTPUT_BY_KEYS:
                if (size == 0)
                {
                    printf("\nNO DATA\n");

                    break;
                }
                if (keys == NULL)
                {
                    printf("\nNO STATISTICS! PLEASE, GET STATISTICS FOR OUTPUT BY KEYS\n");

                    break;
                }
                
                aparts_output_by_keys(aparts, size, keys);

                break;
            case CODE_SAVE:
                if (size == 0)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("Enter a filename: ");
                func_code = aparts_save(aparts, size);

                switch (func_code)
                {
                    case APART_WRITE_ERR_FILE_OPEN:
                        printf("\nCOMPUTER CAN't OPEN/CREATE THIS FILE\n");

                        break;
                    case APART_WRITE_ERR_INVALID_FILENAME:
                        printf("\nINVALID FILENAME\n");

                        break;
                    case APART_WRITE_ERR_NO_DATA:
                        printf("\nNO DATA\n");

                        break;
                    default:
                        printf("\nFILE WAS SAVED SUCCESSFULLY!\n");
                }

                break;
            case CODE_EXIT:
                flag = false;

                if (aparts != NULL && size != 0)
                    free(aparts);

                keys_free(&keys);

                break;
            default:
                printf("\nINVALID CODE\n");
        }
    }

    printf("\nHave a nice day!\n");

    return IO_OK;
}

void keys_free(keystat_t **keys)
{
    if (*keys != NULL)
        free(*keys);

    *keys = NULL;
}
