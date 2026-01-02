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
    if (create_partitions() != 0)
    {
        mvwprintw(modal, 4, 3, "Partitioning %s... FAILED", store->disk);
        wrefresh(modal);
        return INSTALL_ERROR_DISK;
    }
    mvwprintw(modal, 4, 3, "Partitioning %s... OK", store->disk);
    wrefresh(modal);

    // Step 2: Extract rootfs archive to target.
    mvwprintw(modal, 5, 3, "Extracting system files...");
    wrefresh(modal);
    if (extract_rootfs() != 0)
    {
        mvwprintw(modal, 5, 3, "Extracting system files... FAILED");
        wrefresh(modal);
        return INSTALL_ERROR_EXTRACT;
    }
    mvwprintw(modal, 5, 3, "Extracting system files... OK");
    wrefresh(modal);

    // Step 3: Install and configure bootloader.
    mvwprintw(modal, 6, 3, "Installing bootloader...");
    wrefresh(modal);
    if (setup_bootloader() != 0)
    {
        mvwprintw(modal, 6, 3, "Installing bootloader... FAILED");
        wrefresh(modal);
        return INSTALL_ERROR_BOOTLOADER;
    }
    mvwprintw(modal, 6, 3, "Installing bootloader... OK");
    wrefresh(modal);

    // Step 4: Configure system locale.
    mvwprintw(modal, 7, 3, "Configuring locale...");
    wrefresh(modal);
    configure_locale();
    mvwprintw(modal, 7, 3, "Configuring locale... OK");
    wrefresh(modal);

    close_dry_run_log();

    mvwprintw(modal, 9, 3, "Installation complete!");
    mvwprintw(modal, 10, 3, "Press Enter to exit...");
    wrefresh(modal);

    return INSTALL_SUCCESS;
}
