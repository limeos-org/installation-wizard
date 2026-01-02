#include "../all.h"

int setup_bootloader(void)
{
    Store *store = get_store();
    const char *disk = store->disk;

    // Detect UEFI based on whether user configured an ESP partition.
    int is_uefi = 0;
    for (int i = 0; i < store->partition_count; i++)
    {
        if (store->partitions[i].flag_esp)
        {
            is_uefi = 1;
            break;
        }
    }

    // Mount EFI partition if running in UEFI mode.
    if (is_uefi)
    {
        if (store->dry_run)
        {
            // Log mount command in dry run mode.
            run_cmd("mount -t vfat /dev/efi /mnt/boot/efi");
        }
        else
        {
            // Mount the EFI system partition.
            if (mount(disk, "/mnt/boot/efi", "vfat", 0, NULL) != 0)
            {
                return 1;
            }
        }
    }

    // Install GRUB bootloader based on firmware type.
    if (is_uefi)
    {
        // Install GRUB for UEFI systems.
        char *install_args[] = {
            "grub-install",
            "--target=x86_64-efi",
            "--efi-directory=/boot/efi",
            "--bootloader-id=GRUB",
            NULL
        };
        if (run_cmd_chroot("grub-install", install_args) != 0)
        {
            return 1;
        }
    }
    else
    {
        // Install GRUB for BIOS systems.
        char *install_args[] = {"grub-install", (char *)disk, NULL};
        if (run_cmd_chroot("grub-install", install_args) != 0)
        {
            return 1;
        }
    }

    // Generate GRUB configuration file.
    char *update_args[] = {"update-grub", NULL};
    if (run_cmd_chroot("update-grub", update_args) != 0)
    {
        return 1;
    }

    return 0;
}
