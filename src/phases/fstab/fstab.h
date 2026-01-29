#pragma once

/**
 * Generates /etc/fstab on the target system.
 *
 * Creates fstab entries for all configured partitions including
 * root, swap, and any additional mount points.
 *
 * @return - `0` - Success.
 * @return - `-1` - Failed to open fstab file.
 * @return - `-2` - Failed to write fstab header.
 * @return - `-3` - Failed to write fstab entry.
 * @return - `-4` - Failed to flush fstab.
 */
int generate_fstab(void);
