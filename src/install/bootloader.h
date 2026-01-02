#pragma once
#include "../all.h"

/**
 * Installs and configures the bootloader on the target disk.
 *
 * @return 0 on success, non-zero on failure.
 */
int setup_bootloader(void);
