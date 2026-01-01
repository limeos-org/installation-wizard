#pragma once
#include "../all.h"

// Development flags to skip installation steps.
// Uncomment to skip the corresponding step during testing.
//
// #define INSTALL_SKIP_PARTITIONS
// #define INSTALL_SKIP_ROOTFS
// #define INSTALL_SKIP_BOOTLOADER
// #define INSTALL_SKIP_LOCALE

#ifdef INSTALL_SKIP_PARTITIONS
#define INSTALL_SKIP_PARTITIONS_FLAG 1
#else
#define INSTALL_SKIP_PARTITIONS_FLAG 0
#endif

#ifdef INSTALL_SKIP_ROOTFS
#define INSTALL_SKIP_ROOTFS_FLAG 1
#else
#define INSTALL_SKIP_ROOTFS_FLAG 0
#endif

#ifdef INSTALL_SKIP_BOOTLOADER
#define INSTALL_SKIP_BOOTLOADER_FLAG 1
#else
#define INSTALL_SKIP_BOOTLOADER_FLAG 0
#endif

#ifdef INSTALL_SKIP_LOCALE
#define INSTALL_SKIP_LOCALE_FLAG 1
#else
#define INSTALL_SKIP_LOCALE_FLAG 0
#endif

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
