#pragma once
#include "../all.h"

/**
 * Runs the partition method selection step (Easy vs Manual).
 *
 * @param modal The modal window to draw in.
 *
 * @return - `1` - Indicates user made a selection.
 * @return - `0` - Indicates user went back.
 */
int run_method_step(WINDOW *modal);
