#pragma once
#include "../all.h"

/**
 * Runs the partition management step interactively.
 * Allows users to add, edit, and remove partitions.
 *
 * @param modal The modal window to draw in.
 *
 * @return 1 if user confirmed partitions, 0 if user went back.
 */
int run_partition_step(WINDOW *modal);
