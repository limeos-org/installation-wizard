/**
 * This code is responsible for executing shell commands and managing
 * dry run logging functionality.
 */

#include "../all.h"

static FILE *dry_run_log = NULL;

int run_command(const char *command)
{
    Store *store = get_store();

    // Log command to file instead of executing in dry run mode.
    if (store->dry_run)
    {
        // Open log file if not already open.
        if (!dry_run_log)
        {
            dry_run_log = fopen(DRY_RUN_LOG_PATH, "w");
        }

        // Write command to log file.
        if (dry_run_log)
        {
            fprintf(dry_run_log, "%s\n", command);
            fflush(dry_run_log);
        }
        return 0;
    }

    // Execute command using system shell.
    return system(command);
}

int run_command_chroot(const char *cmd_path, char *const argv[])
{
    Store *store = get_store();

    // Log command and return success in dry run mode.
    if (store->dry_run)
    {
        if (!dry_run_log)
        {
            dry_run_log = fopen("dry-run.log", "w");
        }
        if (dry_run_log)
        {
            fprintf(dry_run_log, "chroot /mnt %s", cmd_path);
            for (int i = 1; argv[i] != NULL; i++)
            {
                fprintf(dry_run_log, " %s", argv[i]);
            }
            fprintf(dry_run_log, "\n");
            fflush(dry_run_log);
        }
        return 0;
    }

    // Fork child process to execute command in chroot.
    pid_t pid = fork();

    if (pid == 0)
    {
        // Enter chroot environment in child process.
        if (chdir("/mnt") != 0 || chroot("/mnt") != 0 || chdir("/") != 0)
        {
            _exit(1);
        }
        execvp(cmd_path, argv);
        _exit(1);
    }
    else if (pid > 0)
    {
        // Wait for child process to complete.
        int status;
        waitpid(pid, &status, 0);
        return (WIFEXITED(status) && WEXITSTATUS(status) == 0) ? 0 : 1;
    }

    return 1;
}

void close_dry_run_log(void)
{
    // Close and reset log file handle if open.
    if (dry_run_log)
    {
        fclose(dry_run_log);
        dry_run_log = NULL;
    }
}
