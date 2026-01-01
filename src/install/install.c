#include "../all.h"

int run_install(WINDOW *modal)
{
    Store *store = get_store();

    clear_modal(modal);
    mvwprintw(modal, 2, 3, "Installing LimeOS...");
    wrefresh(modal);

    // Step 1: Create partitions and format disk.
    mvwprintw(modal, 4, 3, "Partitioning %s...", store->disk);
    wrefresh(modal);
    if (INSTALL_SKIP_PARTITIONS_FLAG)
    {
        mvwprintw(modal, 4, 3, "Partitioning %s... SKIPPED", store->disk);
    }
    else if (create_partitions(store->disk, store) != 0)
    {
        mvwprintw(modal, 4, 3, "Partitioning %s... FAILED", store->disk);
        wrefresh(modal);
        return INSTALL_ERROR_DISK;
    }
    else
    {
        mvwprintw(modal, 4, 3, "Partitioning %s... OK", store->disk);
    }
    wrefresh(modal);

    // Step 2: Extract rootfs archive to target.
    mvwprintw(modal, 5, 3, "Extracting system files...");
    wrefresh(modal);
    if (INSTALL_SKIP_ROOTFS_FLAG)
    {
        mvwprintw(modal, 5, 3, "Extracting system files... SKIPPED");
    }
    else if (extract_rootfs() != 0)
    {
        mvwprintw(modal, 5, 3, "Extracting system files... FAILED");
        wrefresh(modal);
        return INSTALL_ERROR_EXTRACT;
    }
    else
    {
        mvwprintw(modal, 5, 3, "Extracting system files... OK");
    }
    wrefresh(modal);

    // Step 3: Install and configure bootloader.
    mvwprintw(modal, 6, 3, "Installing bootloader...");
    wrefresh(modal);
    if (INSTALL_SKIP_BOOTLOADER_FLAG)
    {
        mvwprintw(modal, 6, 3, "Installing bootloader... SKIPPED");
    }
    else if (setup_bootloader(store->disk) != 0)
    {
        mvwprintw(modal, 6, 3, "Installing bootloader... FAILED");
        wrefresh(modal);
        return INSTALL_ERROR_BOOTLOADER;
    }
    else
    {
        mvwprintw(modal, 6, 3, "Installing bootloader... OK");
    }
    wrefresh(modal);

    // Step 4: Configure system locale.
    mvwprintw(modal, 7, 3, "Configuring locale...");
    wrefresh(modal);
    if (INSTALL_SKIP_LOCALE_FLAG)
    {
        mvwprintw(modal, 7, 3, "Configuring locale... SKIPPED");
    }
    else
    {
        configure_locale(store->locale);
        mvwprintw(modal, 7, 3, "Configuring locale... OK");
    }
    wrefresh(modal);

    mvwprintw(modal, 9, 3, "Installation complete!");
    mvwprintw(modal, 10, 3, "Press Enter to exit...");
    wrefresh(modal);

    return INSTALL_SUCCESS;
}
