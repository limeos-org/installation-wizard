#pragma once
#include "../all.h"

/**
 * Runs the confirmation step displaying selected options.
 *
 * @param modal The modal window to draw in.
 *
 * @return 1 if user confirmed installation, 0 if user went back.
 */
int run_confirmation_step(WINDOW *modal);
