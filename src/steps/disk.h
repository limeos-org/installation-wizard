#pragma once
#include "../all.h"

/**
 * Populates the options array with available block devices.
 *
 * @param out_options Array to store disk options.
 * @param max_count Maximum number of options to populate.
 *
 * @return Number of options populated.
 */
int populate_disk_options(StepOption *out_options, int max_count);

/**
 * Runs the disk selection step interactively.
 *
 * @param modal The modal window to draw in.
 *
 * @return 1 if user confirmed selection, 0 if user went back.
 */
int run_disk_step(WINDOW *modal);
