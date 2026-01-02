#pragma once
#include "../all.h"

/** The path to the dry run log file. */
#define DRY_RUN_LOG_PATH "dry-run.log"

/**
 * Executes a shell command, or logs it if dry run mode is enabled.
 *
 * In dry run mode, commands are written to DRY_RUN_LOG_PATH instead of
 * being executed, and the function returns 0 (success).
 *
 * @param command The shell command to execute.
 *
 * @return - `0` - Success (or dry run mode).
 * @return - `non-zero` - The return value of system() on failure.
 */
int run_command(const char *command);

/**
 * Executes a command inside a chroot environment at /mnt.
 *
 * Forks a child process that enters the chroot before executing the command.
 * In dry run mode, logs the command instead of executing.
 *
 * @param cmd_path The command to execute.
 * @param argv     NULL-terminated argument array (argv[0] should be cmd_path).
 *
 * @return 0 on success, non-zero on failure.
 */
int run_command_chroot(const char *cmd_path, char *const argv[]);

/**
 * Closes the dry run log file if open.
 *
 * Should be called at the end of installation to flush and close
 * the log file properly.
 */
void close_dry_run_log(void);
