/**
 * This code is responsible for unmounting all filesystems that were mounted
 * during the installation process, ensuring proper cleanup on completion
 * or failure.
 */

#include "../all.h"

int cleanup_mounts(void)
{
    int errors = 0;

    // Unmount in reverse order of mounting.

    // 1. Unmount chroot bind mounts.
    if (run_command("umount /mnt/sys >/dev/null 2>&1") != 0)
    {
        errors++;
    }
    if (run_command("umount /mnt/proc >/dev/null 2>&1") != 0)
    {
        errors++;
    }
    if (run_command("umount /mnt/dev >/dev/null 2>&1") != 0)
    {
        errors++;
    }

    // 2. Unmount EFI partition (not an error if it wasn't mounted).
    run_command("umount /mnt/boot/efi >/dev/null 2>&1");

    // 3. Disable swap partitions and unmount other partitions.
    Store *store = get_store();
    for (int i = store->partition_count - 1; i >= 0; i--)
    {
        Partition *partition = &store->partitions[i];
        char partition_device[128];
        get_partition_device(store->disk, i + 1, partition_device, sizeof(partition_device));

        if (partition->filesystem == FS_SWAP)
        {
            // Disable swap.
            char cmd[256];
            snprintf(cmd, sizeof(cmd), "swapoff '%s' >/dev/null 2>&1", partition_device);
            run_command(cmd);
        }
        else if (strcmp(partition->mount_point, "/") != 0 &&
                 partition->mount_point[0] == '/')
        {
            // Unmount non-root partitions.
            char mount_path[256];
            snprintf(mount_path, sizeof(mount_path), "/mnt%s", partition->mount_point);
            char cmd[256];
            snprintf(cmd, sizeof(cmd), "umount '%s' >/dev/null 2>&1", mount_path);
            run_command(cmd);
        }
    }

    // 4. Unmount root partition last.
    if (run_command("umount /mnt >/dev/null 2>&1") != 0)
    {
        errors++;
    }

    return errors;
}
