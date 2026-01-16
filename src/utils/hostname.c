/**
 * This code is responsible for providing hostname-related utilities.
 */

#include "../all.h"

const char *get_default_hostname_suffix(void)
{
    // Return suffix based on detected chassis type.
    if (detect_system_chassis() == CHASSIS_LAPTOP)
    {
        return "laptop";
    }
    return "pc";
}
