/**
 * This code is responsible for displaying a confirmation summary
 * of all user selections before proceeding with installation.
 */

#include "../../all.h"

// Minimum sizes for boot partitions.
#define MIN_ESP_SIZE       (100ULL * 1000000)    // 100MB minimum for ESP
#define MIN_BIOS_GRUB_SIZE (1ULL * 1000000)      // 1MB minimum for bios_grub
#define MAX_BIOS_GRUB_SIZE (2ULL * 1000000)      // 2MB maximum for bios_grub
#define MIN_BOOT_SIZE      (300ULL * 1000000)    // 300MB minimum for /boot

semistatic int has_root_partition(Store *store)
{
    for (int i = 0; i < store->partition_count; i++)
    {
        if (strcmp(store->partitions[i].mount_point, "/") == 0)
        {
            return 1;
        }
    }
    return 0;
}

semistatic int has_duplicate_mount_points(Store *store)
{
    for (int i = 0; i < store->partition_count; i++)
    {
        if (store->partitions[i].mount_point[0] == '[') continue;
        for (int j = i + 1; j < store->partition_count; j++)
        {
            if (store->partitions[j].mount_point[0] == '[') continue;
            if (strcmp(
                store->partitions[i].mount_point,
                store->partitions[j].mount_point
            ) == 0)
            {
                return 1;
            }
        }
    }
    return 0;
}

/**
 * Validation result codes for boot partition checks.
 */
typedef enum {
    BOOT_VALID = 0,
    BOOT_ERR_MISSING_ESP,
    BOOT_ERR_ESP_NOT_FAT32,
    BOOT_ERR_ESP_TOO_SMALL,
    BOOT_ERR_ESP_HAS_BIOS_GRUB,
    BOOT_ERR_MISSING_BIOS_GRUB,
    BOOT_ERR_BIOS_GRUB_HAS_FS,
    BOOT_ERR_BIOS_GRUB_WRONG_SIZE,
    BOOT_ERR_BIOS_GRUB_HAS_ESP,
    BOOT_ERR_BOOT_NO_FS,
    BOOT_ERR_BOOT_TOO_SMALL,
    BOOT_ERR_BOOT_HAS_BIOS_GRUB
} BootValidationResult;

/**
 * Validates boot partition configuration according to firmware mode.
 *
 * Rules:
 * - UEFI: Requires ESP (FAT32, esp flag, >=100MB), forbids bios_grub
 * - BIOS+GPT: Requires bios_grub (no fs, 1-2MB), forbids ESP
 * - /boot (if exists): Must have filesystem, >=300MB, no bios_grub flag
 */
semistatic BootValidationResult validate_boot_partitions(Store *store, int is_uefi)
{
    Partition *esp = NULL;
    Partition *bios_grub = NULL;
    Partition *boot_mount = NULL;

    // Find relevant partitions.
    for (int i = 0; i < store->partition_count; i++)
    {
        Partition *p = &store->partitions[i];
        if (p->flag_esp) esp = p;
        if (p->flag_bios_grub) bios_grub = p;
        if (strcmp(p->mount_point, "/boot") == 0) boot_mount = p;
        if (strcmp(p->mount_point, "/boot/efi") == 0) boot_mount = NULL; // /boot/efi is not /boot
    }

    // Re-check for /boot specifically (not /boot/efi).
    boot_mount = NULL;
    for (int i = 0; i < store->partition_count; i++)
    {
        if (strcmp(store->partitions[i].mount_point, "/boot") == 0)
        {
            boot_mount = &store->partitions[i];
            break;
        }
    }

    if (is_uefi)
    {
        // Case A: UEFI (any disk label)
        // Require ESP.
        if (!esp)
        {
            return BOOT_ERR_MISSING_ESP;
        }
        // ESP must be FAT32.
        if (esp->filesystem != FS_FAT32)
        {
            return BOOT_ERR_ESP_NOT_FAT32;
        }
        // ESP must be >= 100MB.
        if (esp->size_bytes < MIN_ESP_SIZE)
        {
            return BOOT_ERR_ESP_TOO_SMALL;
        }
        // Forbid bios_grub in UEFI mode.
        if (bios_grub)
        {
            return BOOT_ERR_ESP_HAS_BIOS_GRUB;
        }
    }
    else
    {
        // Case B: BIOS + GPT
        // Require bios_grub partition.
        if (!bios_grub)
        {
            return BOOT_ERR_MISSING_BIOS_GRUB;
        }
        // bios_grub must have no filesystem.
        if (bios_grub->filesystem != FS_NONE)
        {
            return BOOT_ERR_BIOS_GRUB_HAS_FS;
        }
        // bios_grub must be 1-2MB.
        if (bios_grub->size_bytes < MIN_BIOS_GRUB_SIZE ||
            bios_grub->size_bytes > MAX_BIOS_GRUB_SIZE)
        {
            return BOOT_ERR_BIOS_GRUB_WRONG_SIZE;
        }
        // Forbid ESP in BIOS mode.
        if (esp)
        {
            return BOOT_ERR_BIOS_GRUB_HAS_ESP;
        }
    }

    // Step 3: Validate /boot only if it exists.
    if (boot_mount)
    {
        // /boot must have a filesystem.
        if (boot_mount->filesystem == FS_NONE)
        {
            return BOOT_ERR_BOOT_NO_FS;
        }
        // /boot must be >= 300MB.
        if (boot_mount->size_bytes < MIN_BOOT_SIZE)
        {
            return BOOT_ERR_BOOT_TOO_SMALL;
        }
        // /boot must NOT have bios_grub flag.
        if (boot_mount->flag_bios_grub)
        {
            return BOOT_ERR_BOOT_HAS_BIOS_GRUB;
        }
    }

    return BOOT_VALID;
}

static void render_config_summary(WINDOW *modal, Store *store)
{
    // Display summary of selected options.
    mvwprintw(modal, 4, 3, "Ready to install LimeOS with the following settings:");
    mvwprintw(modal, 6, 3, "  Locale: %s", store->locale);
    mvwprintw(modal, 7, 3, "  Disk: %s", store->disk);

    // Display partition summary.
    unsigned long long disk_size = get_disk_size(store->disk);
    unsigned long long used = sum_partition_sizes(store->partitions, store->partition_count);
    unsigned long long free_space = (disk_size > used) ? disk_size - used : 0;
    char free_str[32];
    format_disk_size(free_space, free_str, sizeof(free_str));

    if (store->partition_count > 0)
    {
        mvwprintw(
            modal, 8, 3,
            "  Partitions: %d partitions, %s left",
            store->partition_count, free_str
        );
    }
    else
    {
        mvwprintw(modal, 8, 3, "  Partitions: (none)");
    }
}

static void render_duplicate_error(WINDOW *modal)
{
    render_error(modal, 10, 3,
        "Multiple partitions share the same mount point.\n"
        "Go back and fix the configuration."
    );
    const char *footer[] = {"[Esc] Back", NULL};
    render_footer(modal, footer);
}

static void render_no_root_error(WINDOW *modal)
{
    render_error(modal, 10, 3,
        "A root (/) partition is required.\n"
        "Go back and add one to continue."
    );
    const char *footer[] = {"[Esc] Back", NULL};
    render_footer(modal, footer);
}

static void render_boot_error(WINDOW *modal, BootValidationResult err)
{
    const char *msg;
    switch (err)
    {
        case BOOT_ERR_MISSING_ESP:
            msg = "UEFI requires an EFI System Partition (ESP).\n"
                  "Add: FAT32, >=100MB, flag=esp, mount=/boot/efi";
            break;
        case BOOT_ERR_ESP_NOT_FAT32:
            msg = "ESP must be formatted as FAT32.\n"
                  "Edit the ESP and set filesystem to fat32.";
            break;
        case BOOT_ERR_ESP_TOO_SMALL:
            msg = "ESP must be at least 100MB (300MB recommended).\n"
                  "Go back and resize the ESP partition.";
            break;
        case BOOT_ERR_ESP_HAS_BIOS_GRUB:
            msg = "UEFI mode forbids bios_grub partitions.\n"
                  "Remove the bios_grub partition.";
            break;
        case BOOT_ERR_MISSING_BIOS_GRUB:
            msg = "BIOS+GPT requires a BIOS boot partition.\n"
                  "Add: 1-2MB, no filesystem, flag=bios_grub";
            break;
        case BOOT_ERR_BIOS_GRUB_HAS_FS:
            msg = "BIOS boot partition must have no filesystem.\n"
                  "Edit it and set filesystem to 'none'.";
            break;
        case BOOT_ERR_BIOS_GRUB_WRONG_SIZE:
            msg = "BIOS boot partition must be 1-2MB.\n"
                  "Go back and resize it.";
            break;
        case BOOT_ERR_BIOS_GRUB_HAS_ESP:
            msg = "BIOS mode forbids ESP partitions.\n"
                  "Remove the ESP partition.";
            break;
        case BOOT_ERR_BOOT_NO_FS:
            msg = "/boot partition must have a filesystem.\n"
                  "Edit /boot and set a filesystem (ext4).";
            break;
        case BOOT_ERR_BOOT_TOO_SMALL:
            msg = "/boot partition must be at least 300MB.\n"
                  "Go back and resize /boot.";
            break;
        case BOOT_ERR_BOOT_HAS_BIOS_GRUB:
            msg = "/boot cannot have the bios_grub flag.\n"
                  "Edit /boot and remove the bios_grub flag.";
            break;
        default:
            msg = "Unknown boot partition error.";
            break;
    }
    render_error(modal, 10, 3, msg);
    const char *footer[] = {"[Esc] Back", NULL};
    render_footer(modal, footer);
}

static void render_ready_message(WINDOW *modal, Store *store)
{
    if (store->dry_run)
    {
        render_info(modal, 10, 3,
            "Dry run mode enabled.\n"
            "No changes will be made to disk."
        );
    }
    else
    {
        char warning_text[128];
        snprintf(
            warning_text, sizeof(warning_text),
            "All data on %s will be erased!\n"
            "This action cannot be undone.", store->disk
        );
        render_warning(modal, 10, 3, warning_text);
    }
    const char *footer[] = {"[Enter] Install", "[Esc] Back", NULL};
    render_footer(modal, footer);
}

int run_confirmation_step(WINDOW *modal)
{
    Store *store = get_store();

    // Clear and draw step header.
    clear_modal(modal);
    wattron(modal, A_BOLD | COLOR_PAIR(CUSTOM_COLOR_PAIR_MAIN));
    mvwprintw(modal, 2, 3, "Step 5: Confirm Installation");
    wattroff(modal, A_BOLD);

    // Render configuration summary.
    render_config_summary(modal, store);

    // Detect firmware mode.
    int is_uefi = (access("/sys/firmware/efi", F_OK) == 0);
    if (store->force_uefi == 1) is_uefi = 1;
    else if (store->force_uefi == -1) is_uefi = 0;

    // Perform validations.
    int has_root = has_root_partition(store);
    int has_duplicate = has_duplicate_mount_points(store);
    BootValidationResult boot_result = validate_boot_partitions(store, is_uefi);
    int can_install = has_root && !has_duplicate && (boot_result == BOOT_VALID);

    // Render the appropriate message based on validation.
    if (has_duplicate)
    {
        render_duplicate_error(modal);
    }
    else if (!has_root)
    {
        render_no_root_error(modal);
    }
    else if (boot_result != BOOT_VALID)
    {
        render_boot_error(modal, boot_result);
    }
    else
    {
        render_ready_message(modal, store);
    }

    wrefresh(modal);

    // Wait for user confirmation or back.
    int key;
    while ((key = getch()) != 27 && (key != '\n' || !can_install))
    {
        // Ignore other input.
    }

    return key == '\n' && can_install;
}
