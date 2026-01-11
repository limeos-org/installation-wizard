/**
 * This code is responsible for creating, formatting, and mounting disk
 * partitions based on the user's configuration stored in the global store.
 */

#include "../all.h"

static int create_gpt_table(const char *disk)
{
    char command[512];
    snprintf(command, sizeof(command), "parted -s '%s' mklabel gpt >>" INSTALL_LOG_PATH " 2>&1", disk);
    if (run_command(command) != 0)
    {
        return -1;
    }
    return 0;
}

static int create_partition_entries(const char *disk, Store *store)
{
    char command[512];
    unsigned long long start_mb = 1;

    for (int i = 0; i < store->partition_count; i++)
    {
        Partition *partition = &store->partitions[i];
        unsigned long long size_mb = partition->size_bytes / (1024 * 1024);
        unsigned long long end_mb = start_mb + size_mb;

        // Create partition with calculated boundaries.
        snprintf(
            command, sizeof(command),
            "parted -s '%s' mkpart %s %lluMiB %lluMiB >>" INSTALL_LOG_PATH " 2>&1",
            disk,
            (partition->type == PART_PRIMARY) ? "primary" : "logical",
            start_mb, end_mb
        );
        if (run_command(command) != 0)
        {
            return -2;
        }

        // Set boot flag if needed.
        if (partition->flag_boot)
        {
            snprintf(
                command, sizeof(command),
                "parted -s '%s' set %d boot on >>" INSTALL_LOG_PATH " 2>&1",
                disk, i + 1
            );
            if (run_command(command) != 0)
            {
                return -3;
            }
        }

        // Set ESP flag if needed.
        if (partition->flag_esp)
        {
            snprintf(
                command, sizeof(command),
                "parted -s '%s' set %d esp on >>" INSTALL_LOG_PATH " 2>&1",
                disk, i + 1
            );
            if (run_command(command) != 0)
            {
                return -4;
            }
        }

        // Set BIOS boot flag if needed (for GPT + BIOS boot).
        if (partition->flag_bios_grub)
        {
            snprintf(
                command, sizeof(command),
                "parted -s '%s' set %d bios_grub on >>" INSTALL_LOG_PATH " 2>&1",
                disk, i + 1
            );
            if (run_command(command) != 0)
            {
                return -4;
            }
        }

        // Update start for next partition.
        start_mb = end_mb;
    }

    return 0;
}

static int format_partitions(const char *disk, Store *store)
{
    char command[512];

    for (int i = 0; i < store->partition_count; i++)
    {
        Partition *partition = &store->partitions[i];
        char partition_device[128];
        get_partition_device(disk, i + 1, partition_device, sizeof(partition_device));

        // Determine formatting command based on filesystem type.
        if (partition->filesystem == FS_EXT4)
        {
            snprintf(command, sizeof(command), "mkfs.ext4 -F '%s' >>" INSTALL_LOG_PATH " 2>&1", partition_device);
        }
        else if (partition->filesystem == FS_SWAP)
        {
            snprintf(command, sizeof(command), "mkswap '%s' >>" INSTALL_LOG_PATH " 2>&1", partition_device);
        }
        else if (partition->filesystem == FS_FAT32)
        {
            snprintf(command, sizeof(command), "mkfs.vfat -F 32 '%s' >>" INSTALL_LOG_PATH " 2>&1", partition_device);
        }
        else
        {
            // Unknown filesystem type, skip formatting.
            continue;
        }

        // Execute formatting command.
        if (run_command(command) != 0)
        {
            return -5;
        }
    }

    return 0;
}

static int find_root_partition_index(Store *store)
{
    for (int i = 0; i < store->partition_count; i++)
    {
        if (strcmp(store->partitions[i].mount_point, "/") == 0)
        {
            return i;
        }
    }
    return -1;
}

static int mount_root_partition(const char *disk, int root_index)
{
    char command[512];
    char root_device[128];

    get_partition_device(disk, root_index + 1, root_device, sizeof(root_device));
    snprintf(command, sizeof(command), "mount '%s' /mnt >>" INSTALL_LOG_PATH " 2>&1", root_device);

    if (run_command(command) != 0)
    {
        return -7;
    }

    return 0;
}

static int mount_remaining_partitions(const char *disk, Store *store)
{
    char command[512];

    for (int i = 0; i < store->partition_count; i++)
    {
        Partition *partition = &store->partitions[i];

        // Enable swap or mount at mount point as needed.
        if (partition->filesystem == FS_SWAP)
        {
            char partition_device[128];
            get_partition_device(disk, i + 1, partition_device, sizeof(partition_device));
            snprintf(command, sizeof(command), "swapon '%s' >>" INSTALL_LOG_PATH " 2>&1", partition_device);
            if (run_command(command) != 0)
            {
                fprintf(stderr, "Warning: failed to enable swap on %s\n", partition_device);
            }
        }
        else if (strcmp(partition->mount_point, "/") != 0 && partition->mount_point[0] == '/')
        {
            char partition_device[128];
            char mount_path[256];
            get_partition_device(disk, i + 1, partition_device, sizeof(partition_device));
            snprintf(mount_path, sizeof(mount_path), "/mnt%s", partition->mount_point);
            snprintf(
                command, sizeof(command),
                "mkdir -p '%s' && mount '%s' '%s' >>" INSTALL_LOG_PATH " 2>&1",
                mount_path, partition_device, mount_path
            );
            if (run_command(command) != 0)
            {
                fprintf(stderr, "Warning: failed to mount %s at %s\n", partition_device, mount_path);
            }
        }
    }

    return 0;
}

int create_partitions(void)
{
    Store *store = get_store();
    const char *disk = store->disk;
    int result;

    // Create GPT partition table.
    result = create_gpt_table(disk);
    if (result != 0)
    {
        return result;
    }

    // Create each partition in sequence.
    result = create_partition_entries(disk, store);
    if (result != 0)
    {
        return result;
    }

    // Format each partition with appropriate filesystem.
    result = format_partitions(disk, store);
    if (result != 0)
    {
        return result;
    }

    // Find and validate root partition.
    int root_index = find_root_partition_index(store);
    if (root_index < 0)
    {
        return -6;
    }

    // Mount the root partition.
    result = mount_root_partition(disk, root_index);
    if (result != 0)
    {
        return result;
    }

    // Mount remaining partitions and enable swap.
    result = mount_remaining_partitions(disk, store);
    if (result != 0)
    {
        return result;
    }

    return 0;
}
