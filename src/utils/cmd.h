#pragma once
#include "../all.h"

/**
 * Path to the dry run log file.
 */
#define DRY_RUN_LOG_PATH "dry-run.log"

/**
 * Executes a shell command, or logs it if dry run mode is enabled.
 *
 * In dry run mode, commands are written to DRY_RUN_LOG_PATH instead of
 * being executed, and the function returns 0 (success).
 *
 * @param cmd The shell command to execute.
 *
 * @return The return value of system(), or 0 in dry run mode.
 */
int run_cmd(const char *cmd);

/**
 * Closes the dry run log file if open.
 *
 * Should be called at the end of installation to flush and close
 * the log file properly.
 */
void close_dry_run_log(void);
