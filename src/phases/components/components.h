#pragma once

/**
 * Installs LimeOS components and configures X11 on the target system.
 *
 * This function:
 * 1. Copies component binaries from live system to target
 * 2. Installs bundled component dependencies if present
 * 3. Writes X11 configuration files (xinitrc, xsession) so the installed
 *    system auto-starts X
 *
 * @return - `0` - Success (or no components to install).
 * @return - `-1` - Failed to copy component binaries.
 * @return - `-2` - Failed to install component packages.
 * @return - `-3` - Failed to write xinitrc.
 * @return - `-4` - Failed to write xsession.
 */
int install_components(void);
