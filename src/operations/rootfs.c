/**
 * This code is responsible for extracting the root filesystem archive
 * to the target mount point during installation.
 */

#include "../all.h"

#define ROOTFS_TARBALL_PATH "/usr/share/limeos/rootfs.tar.gz"

int extract_rootfs(void)
{
    Store *store = get_store();

    // In non-dry-run mode, ensure the rootfs archive exists.
    if (!store->dry_run)
    {
        if (access(ROOTFS_TARBALL_PATH, F_OK) != 0)
        {
            return -1;
        }
    }

    // Extract the rootfs archive to /mnt.
    // Note: Root partition is already mounted by create_partitions().
    if (run_command("tar -xzf " ROOTFS_TARBALL_PATH " -C /mnt >>" INSTALL_LOG_PATH " 2>&1") != 0)
    {
        return -2;
    }

    return 0;
}
