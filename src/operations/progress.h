#pragma once

/**
 * Installation progress events.
 */
typedef enum {
    INSTALL_START,
    INSTALL_STEP_BEGIN,
    INSTALL_STEP_OK,
    INSTALL_STEP_FAIL,
    INSTALL_COMPLETE,
    INSTALL_AWAIT_REBOOT
} InstallEvent;

/**
 * Installation steps.
 */
typedef enum {
    STEP_PARTITIONS,
    STEP_ROOTFS,
    STEP_BOOTLOADER,
    STEP_LOCALE
} InstallStep;

/**
 * Progress callback function type.
 *
 * @param event The type of progress event.
 * @param step Which installation step (for STEP_* events).
 * @param error_code Error code (for INSTALL_STEP_FAIL).
 * @param context User-provided context data.
 */
typedef void (*install_progress_cb)(
    InstallEvent event,
    InstallStep step,
    int error_code,
    void *context
);

/**
 * Returns the display name for an installation step.
 *
 * @param step The installation step.
 *
 * @return The human-readable name for the step.
 */
const char *get_install_step_name(InstallStep step);
