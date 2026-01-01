#pragma once
#include "../all.h"

/**
 * Extracts the root filesystem archive to the target mount point.
 *
 * @return 0 on success, non-zero on failure.
 */
int extract_rootfs(void);
