#ifndef STATISTICS_H__

#define STATISTICS_H__

#include "node_tools.h"
#include "avl_tools.h"
#include "hash_table_tools.h"
#include "file_tools.h"

#define MAX_COLL_COUNT 2048
#define MIN_COLL_COUNT 1
#define COLL_STEP 2

#define HASHSTAT_STR_SIZE 50

#define ITER_COUNT 1500

void hashstat(void);
void avl_hash_stat(void);
void total_stat_search(void);
void total_stat_compares(void);

#endif
