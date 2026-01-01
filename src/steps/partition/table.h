#pragma once
#include "../../all.h"

/**
 * Maximum number of partition rows visible in the table.
 */
#define MAX_VISIBLE_PARTITIONS 3

/**
 * Converts a filesystem type enum to its string representation.
 *
 * @param fs The filesystem type.
 *
 * @return String representation of the filesystem type.
 */
const char *fs_to_string(PartitionFS fs);

/**
 * Converts a partition type enum to its string representation.
 *
 * @param type The partition type.
 *
 * @return String representation of the partition type.
 */
const char *type_to_string(PartitionType type);

/**
 * Renders the partition table in the modal window.
 *
 * @param modal                   The modal window to draw in.
 * @param store                   The global store containing partitions.
 * @param disk_size               Total disk size in bytes.
 * @param selected_partition      Index of selected partition (-1 for none).
 * @param in_partition_select_mode Whether selection highlighting is active.
 * @param scroll_offset           First visible partition index.
 */
void render_partition_table(
    WINDOW *modal, Store *store, unsigned long long disk_size,
    int selected_partition, int in_partition_select_mode,
    int scroll_offset
);
