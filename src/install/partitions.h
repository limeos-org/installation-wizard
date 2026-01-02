#pragma once
#include "../all.h"

/**
 * Creates partitions, formats them, and mounts them.
 *
 * @return 0 on success, non-zero on failure.
 */
int create_partitions(void);
