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

void close_dry_run_log(void)
{
    // Close and reset log file handle if open.
    if (dry_run_log)
    {
        fclose(dry_run_log);
        dry_run_log = NULL;
    }
}
