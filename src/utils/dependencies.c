/**
 * This code is responsible for checking availability of system
 * dependencies including shared libraries and executable commands.
 */

#include "../all.h"

int is_library_available(const char *name)
{
    // Attempt to load the shared library.
    void *handle = dlopen(name, RTLD_NOW);
    if (handle == NULL)
    {
        return 0;
    }

    // Close handle and return success.
    dlclose(handle);
    return 1;
}

int is_command_available(const char *name)
{
    // Get PATH environment variable.
    const char *path_env = getenv("PATH");
    if (path_env == NULL)
    {
        return 0;
    }

    // Duplicate PATH string for tokenization.
    char *path = strdup(path_env);
    if (path == NULL)
    {
        return 0;
    }

    // Search each directory in PATH for the executable.
    char *dir = strtok(path, ":");
    while (dir != NULL)
    {
        // Build full path and check if executable exists.
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, name);
        if (access(full_path, X_OK) == 0)
        {
            free(path);
            return 1;
        }
        dir = strtok(NULL, ":");
    }

    // Clean up and return not found.
    free(path);
    return 0;
}
