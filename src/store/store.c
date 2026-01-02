/**
 * This code is responsible for managing the global installer state,
 * including user selections and installation progress.
 */

#include "../all.h"

static Store store = {
    .current_step = 0,
    .dry_run = 0,
    .locale = "",
    .disk = "",
    .partitions = {{0}},
    .partition_count = 0
};

Store *get_store(void)
{
    return &store;
}

void reset_store(void)
{
    store.current_step = 0;
    store.dry_run = 0;
    store.locale[0] = '\0';
    store.disk[0] = '\0';
    memset(store.partitions, 0, sizeof(store.partitions));
    store.partition_count = 0;
}
