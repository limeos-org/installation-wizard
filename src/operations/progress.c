/**
 * This code is responsible for providing human-readable names for
 * installation steps used in progress display.
 */

#include "../all.h"

const char *get_install_step_name(InstallStep step)
{
    switch (step)
    {
        case STEP_PARTITIONS: return "Partitioning";
        case STEP_ROOTFS:     return "Extracting system files";
        case STEP_BOOTLOADER: return "Installing bootloader";
        case STEP_LOCALE:     return "Configuring locale";
        default:              return "Processing";
    }
}
