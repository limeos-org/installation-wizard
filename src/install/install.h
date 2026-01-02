#pragma once
#include "../all.h"

/**
 * Installation result codes.
 */
#define INSTALL_SUCCESS            0
#define INSTALL_ERROR_DISK        -1
#define INSTALL_ERROR_EXTRACT     -2
#define INSTALL_ERROR_BOOTLOADER  -3

/**
 * Runs the full installation process using settings from the global store.
 *
 * @param modal The modal window for displaying progress.
 *
 * @return - `INSTALL_SUCCESS` on success.
 * @return - `INSTALL_ERROR_DISK` if partitioning fails.
 * @return - `INSTALL_ERROR_EXTRACT` if rootfs extraction fails.
 * @return - `INSTALL_ERROR_BOOTLOADER` if bootloader setup fails.
 */
int run_install(WINDOW *modal);
