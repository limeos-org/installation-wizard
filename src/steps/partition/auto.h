#pragma once
#include "../../all.h"

/**
 * Automatically configures partitions based on disk size and boot mode.
 * Creates appropriate boot partition (ESP for UEFI, bios_grub for BIOS+GPT)
 * and allocates remaining space to root partition.
 *
 * @param store Pointer to the global store.
 * @param disk_size Total disk size in bytes.
 *
 * @return - `1` - Success.
 * @return - `0` - Failure (disk too small).
 */
int auto_partition_disk(Store *store, unsigned long long disk_size);

/**
 * Runs the auto partition step, showing the generated configuration
 * and allowing user to proceed or go back.
 *
 * @param modal The modal window to draw in.
 *
 * @return - `1` - User confirmed auto partitioning.
 * @return - `0` - User went back.
 */
int run_auto_partition_step(WINDOW *modal);
