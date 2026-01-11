#pragma once
#include "../../all.h"

/**
 * Runs the confirmation step displaying selected options.
 *
 * @param modal The modal window to draw in.
 *
 * @return - `1` - Indicates user confirmed installation.
 * @return - `0` - Indicates user went back.
 */
int run_confirmation_step(WINDOW *modal);
