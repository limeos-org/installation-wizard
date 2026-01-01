#pragma once
#include "../all.h"

/**
 * Initializes the ncurses library and configures color pairs.
 */
void init_ui(void);

/**
 * Cleans up ncurses and restores terminal state.
 */
void cleanup_ui(void);

/**
 * Renders footer items with darker background, separated by spaces.
 *
 * @param modal The modal window to render the footer in.
 * @param items NULL-terminated array of strings.
 */
void render_footer(WINDOW *modal, const char **items);
