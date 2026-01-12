/**
 * This code is responsible for automatic partition configuration
 * based on disk size and boot mode (UEFI or BIOS).
 */

#include "../../all.h"

// Minimum sizes for automatic partitioning.
#define AUTO_ESP_SIZE       (300ULL * 1000000)    // 300MB for ESP (recommended)
#define AUTO_BIOS_GRUB_SIZE (2ULL * 1000000)      // 2MB for bios_grub
#define AUTO_MIN_ROOT_SIZE  (4ULL * 1000000000)   // 4GB minimum for root

int auto_partition_disk(Store *store, unsigned long long disk_size)
{
    // Detect boot mode.
    int is_uefi = (access("/sys/firmware/efi", F_OK) == 0);

    // Override with force_uefi setting if set.
    if (store->force_uefi == 1)
    {
        is_uefi = 1;
    }
    else if (store->force_uefi == -1)
    {
        is_uefi = 0;
    }

    // Calculate boot partition size.
    unsigned long long boot_size = is_uefi ? AUTO_ESP_SIZE : AUTO_BIOS_GRUB_SIZE;

    // Check if disk is large enough.
    if (disk_size < boot_size + AUTO_MIN_ROOT_SIZE)
    {
        return 0;
    }

    // Clear existing partitions.
    store->partition_count = 0;
    memset(store->partitions, 0, sizeof(store->partitions));

    // Create boot partition.
    Partition *boot = &store->partitions[0];
    boot->size_bytes = boot_size;
    boot->type = PART_PRIMARY;

    if (is_uefi)
    {
        // ESP partition for UEFI.
        snprintf(boot->mount_point, sizeof(boot->mount_point), "/boot/efi");
        boot->filesystem = FS_FAT32;
        boot->flag_esp = 1;
        boot->flag_boot = 0;
        boot->flag_bios_grub = 0;
    }
    else
    {
        // BIOS GRUB partition for BIOS+GPT - not mounted, used by GRUB directly.
        snprintf(boot->mount_point, sizeof(boot->mount_point), "[bios]");
        boot->filesystem = FS_NONE;
        boot->flag_bios_grub = 1;
        boot->flag_boot = 0;
        boot->flag_esp = 0;
    }

    store->partition_count = 1;

    // Create root partition with remaining space.
    Partition *root = &store->partitions[1];
    root->size_bytes = disk_size - boot_size;
    root->type = PART_PRIMARY;
    snprintf(root->mount_point, sizeof(root->mount_point), "/");
    root->filesystem = FS_EXT4;
    root->flag_boot = 0;
    root->flag_esp = 0;
    root->flag_bios_grub = 0;

    store->partition_count = 2;

    return 1;
}

int run_auto_partition_step(WINDOW *modal)
{
    Store *store = get_store();
    unsigned long long disk_size = get_disk_size(store->disk);

    // Generate automatic partition layout.
    if (!auto_partition_disk(store, disk_size))
    {
        // Disk too small - show error.
        clear_modal(modal);
        wattron(modal, A_BOLD);
        mvwprintw(modal, 2, 3, "Step 4: Auto Partitioning");
        wattroff(modal, A_BOLD);

        render_error(modal, 5, 3,
            "Disk is too small for automatic partitioning.\n"
            "Minimum 4.5GB required. Use Manual mode instead."
        );

        const char *footer[] = {"[Esc] Back", NULL};
        render_footer(modal, footer);
        wrefresh(modal);

        // Wait for escape.
        while (getch() != 27);
        return 0;
    }

    // Show the generated partition layout.
    int scroll_offset = 0;

    while (1)
    {
        clear_modal(modal);
        wattron(modal, A_BOLD);
        mvwprintw(modal, 2, 3, "Step 4: Auto Partitioning");
        wattroff(modal, A_BOLD);

        // Show info about auto configuration.
        int is_uefi = (access("/sys/firmware/efi", F_OK) == 0);
        if (store->force_uefi == 1) is_uefi = 1;
        else if (store->force_uefi == -1) is_uefi = 0;

        if (is_uefi)
        {
            render_info(modal, 4, 3,
                "UEFI mode detected.\n"
                "Creating ESP (300MB) + Root partition."
            );
        }
        else
        {
            render_info(modal, 4, 3,
                "BIOS mode detected.\n"
                "Creating BIOS boot (2MB) + Root partition."
            );
        }

        // Render the partition table.
        render_partition_table(modal, store, disk_size, -1, 0, scroll_offset);

        const char *footer[] = {
            "[Enter] Continue", "[Esc] Back", NULL
        };
        render_footer(modal, footer);
        wrefresh(modal);

        int key = getch();
        if (key == '\n')
        {
            return 1;
        }
        else if (key == 27)
        {
            // Clear partitions if going back.
            store->partition_count = 0;
            memset(store->partitions, 0, sizeof(store->partitions));
            return 0;
        }
        else if (key == KEY_UP && scroll_offset > 0)
        {
            scroll_offset--;
        }
        else if (key == KEY_DOWN)
        {
            int max_scroll = store->partition_count - MAX_VISIBLE_PARTITIONS;
            if (max_scroll < 0) max_scroll = 0;
            if (scroll_offset < max_scroll) scroll_offset++;
        }
    }
}
