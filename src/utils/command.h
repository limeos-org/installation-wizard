#pragma once
#include "../all.h"

/**
 * Executes a shell command and returns its exit status.
 *
 * @param cmd The command string to execute.
 *
 * @return 0 on success, non-zero on failure.
 */
int run_command(const char *cmd);
