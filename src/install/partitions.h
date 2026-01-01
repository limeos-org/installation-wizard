#pragma once
#include "../all.h"

/**
 * Creates partitions, formats them, and mounts them.
 *
 * @param disk  The target disk device path.
 * @param store The global store containing partition configuration.
 *
 * @return 0 on success, non-zero on failure.
 */
int create_partitions(const char *disk, Store *store);
