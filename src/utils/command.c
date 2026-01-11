/**
 * This code is responsible for executing shell commands and managing
 * dry run logging functionality.
 */

#include "../all.h"

static FILE *dry_run_log = NULL;
static CommandPollCallback poll_callback = NULL;

void set_command_poll_callback(CommandPollCallback callback)
{
    poll_callback = callback;
}

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

    // If no poll callback, use simple blocking execution.
    if (!poll_callback)
    {
        return system(command);
    }

    // Fork and exec to allow polling for input during execution.
    pid_t pid = fork();
    if (pid < 0)
    {
        // Fork failed, fall back to system().
        return system(command);
    }

    if (pid == 0)
    {
        // Child process: execute the command.
        execl("/bin/sh", "sh", "-c", command, (char *)NULL);
        _exit(127); // exec failed
    }

    // Parent process: poll for completion while checking input.
    int status;
    while (1)
    {
        pid_t result = waitpid(pid, &status, WNOHANG);
        if (result == pid)
        {
            // Child finished.
            if (WIFEXITED(status))
            {
                return WEXITSTATUS(status);
            }
            return -1; // Abnormal termination
        }
        if (result < 0)
        {
            return -1; // waitpid error
        }

        // Child still running, invoke poll callback.
        if (poll_callback)
        {
            poll_callback();
        }

        // Small delay to avoid busy-waiting.
        usleep(50000); // 50ms
    }
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
