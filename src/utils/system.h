#pragma once
#include "../all.h"

/** A type representing system chassis types. */
typedef enum {
    CHASSIS_DESKTOP,
    CHASSIS_LAPTOP,
    CHASSIS_UNKNOWN
} ChassisType;

/**
 * Detects the system chassis type by reading DMI information.
 * Checks /sys/class/dmi/id/chassis_type for the chassis type code.
 *
 * Common chassis type codes:
 * - Desktop: 3 (Desktop), 4 (Low Profile Desktop), 6 (Mini Tower),
 *            7 (Tower), 13 (All-in-One), 35 (Mini PC)
 * - Laptop: 8 (Portable), 9 (Laptop), 10 (Notebook), 14 (Sub Notebook),
 *           31 (Convertible), 32 (Detachable)
 *
 * @return The detected chassis type.
 */
ChassisType detect_system_chassis(void);

/**
 * Gets the total system RAM in bytes by reading /proc/meminfo.
 *
 * @return Total RAM in bytes, or 0 if unavailable.
 */
unsigned long long get_system_ram(void);
