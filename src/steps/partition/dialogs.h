#pragma once
#include "../../all.h"

/**
 * Displays the add partition dialog and creates a new partition.
 *
 * @param modal     The modal window to draw in.
 * @param store     The global store to add partition to.
 * @param disk_size Total disk size in bytes.
 *
 * @return - `1` - Indicates partition was added.
 * @return - `0` - Indicates cancelled.
 */
int add_partition_dialog(
    WINDOW *modal, Store *store, unsigned long long disk_size
);

/**
 * Displays the edit partition dialog and modifies an existing partition.
 *
 * @param modal     The modal window to draw in.
 * @param store     The global store containing partitions.
 * @param disk_size Total disk size in bytes.
 *
 * @return - `1` - Indicates partition was modified.
 * @return - `0` - Indicates cancelled.
 */
int edit_partition_dialog(
    WINDOW *modal, Store *store, unsigned long long disk_size
);

/**
 * Displays the remove partition dialog and removes a partition.
 *
 * @param modal     The modal window to draw in.
 * @param store     The global store containing partitions.
 * @param disk_size Total disk size in bytes.
 *
 * @return - `1` - Indicates partition was removed.
 * @return - `0` - Indicates cancelled.
 */
int remove_partition_dialog(
    WINDOW *modal, Store *store, unsigned long long disk_size
);

/**
 * Automatically creates an optimal partition layout based on system type.
 *
 * Clears existing partitions and creates:
 * - Boot partition (ESP for UEFI, bios_grub for BIOS+GPT)
 * - Swap partition (sized based on system RAM)
 * - Root partition (remaining disk space)
 *
 * @param store     The global store to populate with partitions.
 * @param disk_size Total disk size in bytes.
 *
 * @return - `1` - Indicates partitions were created.
 * @return - `0` - Indicates autofill failed or was cancelled.
 */
int autofill_partitions(Store *store, unsigned long long disk_size);
