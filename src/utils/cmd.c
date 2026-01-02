#include "../all.h"

static FILE *dry_run_log = NULL;

int run_cmd(const char *cmd)
{
    Store *store = get_store();

    if (store->dry_run)
    {
        if (!dry_run_log)
        {
            dry_run_log = fopen("dry-run.log", "w");
        }
        if (dry_run_log)
        {
            fprintf(dry_run_log, "%s\n", cmd);
            fflush(dry_run_log);
        }
        return 0;
    }

    return system(cmd);
}

void close_dry_run_log(void)
{
    if (dry_run_log)
    {
        fclose(dry_run_log);
        dry_run_log = NULL;
    }
}
