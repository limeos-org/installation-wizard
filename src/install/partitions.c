#include "../all.h"

int create_partitions(void)
{
    Store *store = get_store();
    const char *disk = store->disk;
    char cmd[512];

    // Create GPT partition table.
    snprintf(cmd, sizeof(cmd), "parted -s %s mklabel gpt 2>/dev/null", disk);
    if (run_cmd(cmd) != 0)
    {
        return 1;
    }

    // Create each partition in sequence.
    unsigned long long start_mb = 1;
    for (int i = 0; i < store->partition_count; i++)
    {
        Partition *p = &store->partitions[i];
        unsigned long long size_mb = p->size_bytes / (1024 * 1024);
        unsigned long long end_mb = start_mb + size_mb;

        // Create partition with calculated boundaries.
        snprintf(
            cmd, sizeof(cmd),
            "parted -s %s mkpart %s %lluMiB %lluMiB 2>/dev/null",
            disk,
            (p->type == PART_PRIMARY) ? "primary" : "logical",
            start_mb, end_mb
        );
        if (run_cmd(cmd) != 0)
        {
            return 1;
        }

        // Set boot flag if needed.
        if (p->flag_boot)
        {
            snprintf(
                cmd, sizeof(cmd),
                "parted -s %s set %d boot on 2>/dev/null",
                disk, i + 1
            );
            if (run_cmd(cmd) != 0)
            {
                return 1;
            }
        }

        // Set ESP flag if needed.
        if (p->flag_esp)
        {
            snprintf(
                cmd, sizeof(cmd),
                "parted -s %s set %d esp on 2>/dev/null",
                disk, i + 1
            );
            if (run_cmd(cmd) != 0)
            {
                return 1;
            }
        }

        start_mb = end_mb;
    }

    // Format each partition with appropriate filesystem.
    for (int i = 0; i < store->partition_count; i++)
    {
        Partition *p = &store->partitions[i];
        char partition_device[128];
        get_partition_device(disk, i + 1, partition_device, sizeof(partition_device));

        if (p->filesystem == FS_EXT4)
        {
            snprintf(cmd, sizeof(cmd), "mkfs.ext4 -F %s 2>/dev/null", partition_device);
        }
        else if (p->filesystem == FS_SWAP)
        {
            snprintf(cmd, sizeof(cmd), "mkswap %s 2>/dev/null", partition_device);
        }

        if (run_cmd(cmd) != 0)
        {
            return 1;
        }
    }

    // Mount root partition first.
    for (int i = 0; i < store->partition_count; i++)
    {
        Partition *p = &store->partitions[i];
        if (strcmp(p->mount_point, "/") == 0)
        {
            char partition_device[128];
            get_partition_device(disk, i + 1, partition_device, sizeof(partition_device));
            snprintf(cmd, sizeof(cmd), "mount %s /mnt 2>/dev/null", partition_device);
            if (run_cmd(cmd) != 0)
            {
                return 1;
            }
            break;
        }
    }

    // Mount remaining partitions.
    for (int i = 0; i < store->partition_count; i++)
    {
        Partition *p = &store->partitions[i];
        if (p->filesystem == FS_SWAP)
        {
            char partition_device[128];
            get_partition_device(disk, i + 1, partition_device, sizeof(partition_device));
            snprintf(cmd, sizeof(cmd), "swapon %s 2>/dev/null", partition_device);
            run_cmd(cmd);
        }
        else if (strcmp(p->mount_point, "/") != 0 && p->mount_point[0] == '/')
        {
            char partition_device[128];
            char mount_path[256];
            get_partition_device(disk, i + 1, partition_device, sizeof(partition_device));
            snprintf(mount_path, sizeof(mount_path), "/mnt%s", p->mount_point);
            snprintf(
                cmd, sizeof(cmd),
                "mkdir -p %s && mount %s %s 2>/dev/null",
                mount_path, partition_device, mount_path
            );
            run_cmd(cmd);
        }
    }

    return 0;
}
