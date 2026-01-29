/**
 * This code is responsible for installing and configuring the bootloader
 * on the target system. Both UEFI and BIOS systems use GRUB for consistency.
 * Silent boot is pre-configured in /etc/default/grub.d/ by iso-builder.
 */

#include "../../all.h"

static int verify_chroot_works(void)
{
    const char *marker = "/mnt/tmp/.chroot_verify";

    // Escape the marker path for shell command.
    char escaped_marker[256];
    if (common.shell_escape(marker, escaped_marker, sizeof(escaped_marker)) != 0)
    {
        return -1;
    }

    // Create marker file inside chroot /mnt.
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo 'limeos' > %s", escaped_marker);
    if (run_install_command(cmd) != 0)
    {
        return -2;
    }

    // Verify chroot can see the marker at /tmp/.chroot_verify (not /mnt/tmp).
    // If chroot fails silently, cat would look at the host's /tmp and fail.
    int result = run_install_command("chroot /mnt cat /tmp/.chroot_verify >/dev/null 2>&1");

    // Clean up marker file.
    snprintf(cmd, sizeof(cmd), "rm -f %s", escaped_marker);
    run_install_command(cmd);

    return (result == 0) ? 0 : -3;
}

static int detect_uefi_mode(void)
{
    return (detect_firmware_type() == FIRMWARE_UEFI);
}

static int verify_esp_mounted(void)
{
    // The partitions phase mounts all partitions including ESP.
    // Just verify it's mounted where we expect.
    if (run_install_command("mountpoint -q /mnt/boot/efi") != 0)
    {
        return -1;
    }
    return 0;
}

static int mount_chroot_system_dirs(void)
{
    // Bind mount /dev for device access inside chroot.
    if (run_install_command("mount --bind /dev /mnt/dev") != 0)
    {
        return -1;
    }

    // Mount proc filesystem for process information.
    if (run_install_command("mount -t proc proc /mnt/proc") != 0)
    {
        run_install_command("umount /mnt/dev");
        return -2;
    }

    // Mount sysfs for kernel and device information.
    if (run_install_command("mount -t sysfs sys /mnt/sys") != 0)
    {
        run_install_command("umount /mnt/proc");
        run_install_command("umount /mnt/dev");
        return -3;
    }

    return 0;
}

static void unmount_chroot_system_dirs(void)
{
    // Unmount sysfs.
    run_install_command("umount /mnt/sys");

    // Unmount proc filesystem.
    run_install_command("umount /mnt/proc");

    // Unmount /dev bind mount.
    run_install_command("umount /mnt/dev");
}

static int setup_chroot_environment(void)
{
    write_install_log("Mounting chroot system directories (dev, proc, sys)");
    if (mount_chroot_system_dirs() != 0)
    {
        write_install_log("Failed to mount chroot system directories");
        return -1;
    }

    write_install_log("Verifying chroot environment");
    if (verify_chroot_works() != 0)
    {
        write_install_log("Chroot verification failed");
        unmount_chroot_system_dirs();
        return -2;
    }
    
    write_install_log("Chroot environment verified");

    return 0;
}

static int install_grub_packages(int is_uefi)
{
    // Ensure target apt cache directory exists.
    if (run_install_command("mkdir -p /mnt/var/cache/apt/archives >>" CONFIG_INSTALL_LOG_PATH " 2>&1") != 0)
    {
        return -1;
    }

    // Copy only the appropriate packages based on firmware type.
    // UEFI uses grub-efi-*, BIOS uses grub-pc-*.
    const char *cp_cmd = is_uefi
        ? "cp /var/cache/apt/archives/grub-efi*.deb /mnt/var/cache/apt/archives/ >>" CONFIG_INSTALL_LOG_PATH " 2>&1"
        : "cp /var/cache/apt/archives/grub-pc*.deb /mnt/var/cache/apt/archives/ >>" CONFIG_INSTALL_LOG_PATH " 2>&1";
    if (run_install_command(cp_cmd) != 0)
    {
        return -2;
    }

    // Install GRUB packages. Run dpkg twice: first pass unpacks all packages,
    // second pass configures them in dependency order.
    // Note: Inner sh -c ensures the glob expands inside the chroot, not on the host.
    // Use --no-triggers to prevent dpkg from running initramfs-tools triggers,
    // which would regenerate initramfs in the chroot (where firmware detection
    // fails). The pre-built initramfs already has GPU firmware/drivers embedded.
    run_install_command("chroot /mnt sh -c 'dpkg -i --no-triggers /var/cache/apt/archives/*.deb' >>" CONFIG_INSTALL_LOG_PATH " 2>&1");

    // Configure installed packages.
    if (run_install_command("chroot /mnt dpkg --configure -a --no-triggers >>" CONFIG_INSTALL_LOG_PATH " 2>&1") != 0)
    {
        return -3;
    }

    return 0;
}

static int run_grub_install(const char *disk, int is_uefi)
{
    if (is_uefi)
    {
        // Install GRUB for UEFI target with EFI directory.
        if (run_install_command("chroot /mnt /usr/sbin/grub-install "
            "--target=x86_64-efi --efi-directory=/boot/efi --bootloader-id=GRUB "
            ">>" CONFIG_INSTALL_LOG_PATH " 2>&1") != 0)
        {
            return -1;
        }

        // Create fallback boot path. UEFI looks for /EFI/BOOT/BOOTX64.EFI when
        // no NVRAM boot entry exists. efibootmgr can't create NVRAM entries in
        // a chroot (no access to efivars), so we must provide this fallback.
        if (run_install_command("mkdir -p /mnt/boot/efi/EFI/BOOT") != 0)
        {
            return -4;
        }
        if (run_install_command("cp /mnt/boot/efi/EFI/GRUB/grubx64.efi /mnt/boot/efi/EFI/BOOT/BOOTX64.EFI") != 0)
        {
            return -5;
        }
    }
    else
    {
        // Escape disk path for shell command.
        char escaped_disk[256];
        if (common.shell_escape(disk, escaped_disk, sizeof(escaped_disk)) != 0)
        {
            return -2;
        }

        // Install GRUB to disk MBR for BIOS boot.
        char cmd[512];
        snprintf(cmd, sizeof(cmd),
            "chroot /mnt /usr/sbin/grub-install %s >>" CONFIG_INSTALL_LOG_PATH " 2>&1",
            escaped_disk);
        if (run_install_command(cmd) != 0)
        {
            return -3;
        }
    }

    return 0;
}

static int run_update_grub(void)
{
    // Run update-grub inside chroot to (re)generate GRUB config.
    if (run_install_command("chroot /mnt /usr/sbin/update-grub >>" CONFIG_INSTALL_LOG_PATH " 2>&1") != 0)
    {
        return -1;
    }

    return 0;
}

static int setup_grub_bios(const char *disk)
{
    if (setup_chroot_environment() != 0)
    {
        return -1;
    }

    // Install GRUB packages for BIOS.
    write_install_log("Installing GRUB BIOS packages from apt cache");
    if (install_grub_packages(0) != 0)
    {
        write_install_log("Failed to install GRUB packages");
        return -3;
    }

    // Run grub-install for BIOS.
    write_install_log("Running grub-install for BIOS");
    if (run_grub_install(disk, 0) != 0)
    {
        write_install_log("grub-install failed");
        return -4;
    }

    // Generate GRUB configuration.
    write_install_log("Running update-grub to generate configuration");
    if (run_update_grub() != 0)
    {
        write_install_log("update-grub failed");
        return -5;
    }

    return 0;
}

static int setup_grub_uefi(const char *disk)
{
    // Verify ESP is mounted (partitions phase handles mounting).
    write_install_log("Verifying ESP is mounted at /mnt/boot/efi");
    if (verify_esp_mounted() != 0)
    {
        write_install_log("ESP not mounted at /mnt/boot/efi");
        return -1;
    }

    if (setup_chroot_environment() != 0)
    {
        return -2;
    }

    // Install GRUB packages for UEFI.
    write_install_log("Installing GRUB EFI packages from apt cache");
    if (install_grub_packages(1) != 0)
    {
        write_install_log("Failed to install GRUB packages");
        return -4;
    }

    // Run grub-install for UEFI and create fallback boot path.
    write_install_log("Running grub-install for UEFI");
    if (run_grub_install(disk, 1) != 0)
    {
        write_install_log("grub-install failed");
        return -5;
    }
    write_install_log("Created fallback boot path at /boot/efi/EFI/BOOT/BOOTX64.EFI");

    // Generate GRUB configuration.
    write_install_log("Running update-grub to generate configuration");
    if (run_update_grub() != 0)
    {
        write_install_log("update-grub failed");
        return -6;
    }

    return 0;
}

int setup_bootloader(void)
{
    Store *store = get_store();
    const char *disk = store->disk;

    // Detect boot mode.
    int is_uefi = detect_uefi_mode();
    write_install_log("Boot mode: %s", is_uefi ? "UEFI" : "BIOS");

    int result;
    if (is_uefi)
    {
        write_install_log("Using GRUB for UEFI");
        result = setup_grub_uefi(disk);
    }
    else
    {
        write_install_log("Using GRUB for BIOS");
        result = setup_grub_bios(disk);
    }

    if (result != 0)
    {
        return result;
    }

    write_install_log("Bootloader installation complete");

    return 0;
}
