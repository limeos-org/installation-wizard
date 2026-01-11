#pragma once
#include "../all.h"

/**
 * Runs the full installation process using settings from the global store.
 *
 * @param progress_cb Callback for progress updates (can be NULL for silent 
 * mode).
 * @param context User data passed to callback.
 *
 * @return - `0` - on success.
 * @return - `-1` - if partitioning fails.
 * @return - `-2` - if rootfs extraction fails.
 * @return - `-3` - if bootloader setup fails.
 * @return - `-4` - if locale configuration fails.
 */
int run_install(install_progress_cb progress_cb, void *context);
