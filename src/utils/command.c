#include "../all.h"

int run_command(const char *cmd)
{
    int result = system(cmd);
    if (result == -1)
    {
        return 1;
    }

    if (WIFEXITED(result))
    {
        return WEXITSTATUS(result);
    }

    return 1;
}
