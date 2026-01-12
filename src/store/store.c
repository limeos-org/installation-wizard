/**
 * This code is responsible for managing the global store singleton that
 * holds user selections and installation settings throughout the wizard.
 */

#include "../all.h"

static Store store = {
    .current_step = 0,
    .dry_run = 0,
    .force_uefi = 0,
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
    // Reset step and mode state.
    store.current_step = 0;
    store.dry_run = 0;
    store.force_uefi = 0;

    // Clear user selection strings.
    store.locale[0] = '\0';
    store.disk[0] = '\0';

    // Clear partition configuration.
    memset(store.partitions, 0, sizeof(store.partitions));
    store.partition_count = 0;
}
