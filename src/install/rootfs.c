#include "../all.h"

int extract_rootfs(void)
{
    // Ensure the placeholder rootfs archive exists.
    if (access("assets/placeholder-rootfs.gz", F_OK) != 0)
    {
        return 1;
    }

    // Extract the rootfs archive to /mnt.
    int result = system("tar -xzf assets/placeholder-rootfs.gz -C /mnt &> /dev/null");
    if (result == -1)
    {
        return 1;
    }
    if (WIFEXITED(result))
    {
        if (WEXITSTATUS(result) != 0)
        {
            return 1;
        }
    }

    // Set up necessary mounts for chroot environment.
    result = system("sh src/install/mounts.sh &> /dev/null");
    if (result == -1)
    {
        return 1;
    }
    if (WIFEXITED(result))
    {
        if (WEXITSTATUS(result) != 0)
        {
            return 1;
        }
    }

    return 0;
}
