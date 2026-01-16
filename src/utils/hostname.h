#pragma once
#include "../all.h"

/**
 * Returns the default hostname suffix based on chassis type.
 *
 * @return "laptop" for laptops, "pc" for desktops and unknown systems.
 */
const char *get_default_hostname_suffix(void);
