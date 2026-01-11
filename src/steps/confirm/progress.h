#pragma once
#include "../../all.h"

/**
 * ncurses-based progress callback for installation.
 * Displays progress in a modal window.
 *
 * @param event Progress event type.
 * @param step Installation step identifier.
 * @param error_code Error code for failure events.
 * @param context Must be a valid WINDOW* pointer.
 */
void ncurses_install_progress(
    InstallEvent event,
    InstallStep step,
    int error_code,
    void *context
);

/**
 * Sets the visibility of the installation logs viewer.
 *
 * @param visible Non-zero to show logs, zero to hide.
 */
void set_logs_visible(int visible);

/**
 * Gets the current visibility state of the installation logs viewer.
 *
 * @return Non-zero if logs are visible, zero otherwise.
 */
int get_logs_visible(void);

/**
 * Toggles the visibility of the installation logs viewer.
 */
void toggle_logs_visible(void);

/**
 * Sets the modal window used for polling during command execution.
 *
 * @param modal The modal window to refresh when toggling logs.
 */
void set_install_poll_modal(void *modal);

/**
 * Callback for polling input during command execution.
 * Handles backtick key to toggle log visibility.
 */
void install_poll_callback(void);
