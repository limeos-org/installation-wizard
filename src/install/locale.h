#pragma once
#include "../all.h"

/**
 * Configures the system locale settings.
 *
 * @param locale The locale string to configure (e.g., "en_US.UTF-8").
 *
 * @return 0 on success, non-zero on failure.
 */
int configure_locale(const char *locale);
