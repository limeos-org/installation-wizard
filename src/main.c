/**
 * This code is responsible for the main entry point of the installation
 * wizard. It initializes the UI, runs through each installation step, and
 * executes the installation process.
 */

#include "all.h"

static const char *libraries[] = {
    "libncurses.so.6"
};

static const char *commands[] = {
    // Partitioning.
    "parted",
    "mkfs.ext4",
    "mkfs.vfat",
    "mkswap",
    "mount",
    "umount",
    "swapon",
    "swapoff",
    "mkdir",
    // Rootfs extraction.
    "tar",
    // Locale configuration.
    "sed"
};

int main(int argc, char *argv[])
{
    Store *store = get_store();

    // Ensure that the required libraries are available.
    const int library_count = sizeof(libraries) / sizeof(libraries[0]);
    for (int i = 0; i < library_count; i++)
    {
        if (!is_library_available(libraries[i]))
        {
            fprintf(stderr, "Missing library \"%s\".\n", libraries[i]);
            exit(EXIT_FAILURE);
        }
    }

    // Ensure that the required commands are available.
    const int command_count = sizeof(commands) / sizeof(commands[0]);
    for (int i = 0; i < command_count; i++)
    {
        if (!is_command_available(commands[i]))
        {
            fprintf(stderr, "Missing command \"%s\".\n", commands[i]);
            exit(EXIT_FAILURE);
        }
    }

    // Parse command-line arguments.
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--dry") == 0)
        {
            store->dry_run = 1;
        }
    }

    // Initialize ncurses UI.
    initialize_ui();

    // Create the centered modal window for wizard content.
    WINDOW *modal = create_modal("Installation Wizard");

    // A loop that runs throughout the entire wizard process and waits for user
    // input at each step, allowing back-and-forth navigation between steps.
    int step = 1;

    while (step <= 5)
    {
        int result = 0;

        switch (step)
        {
            case 1:
                result = run_locale_step(modal);
                break;
            case 2:
                result = run_method_step(modal);
                break;
            case 3:
                result = run_disk_step(modal);
                break;
            case 4:
                // Run auto or manual partition step based on method.
                if (store->partition_method == METHOD_EASY)
                {
                    result = run_auto_partition_step(modal);
                }
                else
                {
                    result = run_partition_step(modal);
                }
                break;
            case 5:
                result = run_confirmation_step(modal);
                break;
        }

        if (result)
        {
            step++;
        }
        else if (step > 1)
        {
            step--;
        }
        // If step == 1 and result == 0, stay on step 1.
    }

    // Run installation loop with failure recovery.
    int result;
    int retry = 1;

    while (retry)
    {
        // Run installation using settings from global state.
        result = run_install(ncurses_install_progress, modal);

        if (result == 0)
        {
            // Installation succeeded.
            retry = 0;
        }
        else
        {
            // Show failure recovery dialog.
            clear_modal(modal);
            wattron(modal, A_BOLD);
            mvwprintw(modal, 2, 3, "Installation Failed");
            wattroff(modal, A_BOLD);

            if (store->partition_method == METHOD_EASY)
            {
                render_error(modal, 5, 3,
                    "Automatic partitioning failed.\n"
                    "Please read the wiki and try Manual mode."
                );
                mvwprintw(modal, 10, 3, "Your configuration:");
                mvwprintw(modal, 11, 3, "  Disk: %s", store->disk);
                mvwprintw(modal, 12, 3, "  Method: Easy (Automatic)");
            }
            else
            {
                render_error(modal, 5, 3,
                    "Installation failed with your configuration.\n"
                    "Review your partition settings below."
                );

                // Show partition configuration summary.
                mvwprintw(modal, 10, 3, "Your configuration:");
                mvwprintw(modal, 11, 3, "  Disk: %s", store->disk);
                mvwprintw(modal, 12, 3, "  Partitions:");

                int y = 13;
                for (int i = 0; i < store->partition_count && y < MODAL_HEIGHT - 5; i++)
                {
                    char size_str[32];
                    format_disk_size(store->partitions[i].size_bytes, size_str, sizeof(size_str));

                    const char *flags = "";
                    if (store->partitions[i].flag_esp) flags = " [ESP]";
                    else if (store->partitions[i].flag_bios_grub) flags = " [BIOS]";
                    else if (store->partitions[i].flag_boot) flags = " [boot]";

                    mvwprintw(modal, y, 5, "%d. %s %s%s",
                        i + 1,
                        store->partitions[i].mount_point,
                        size_str,
                        flags
                    );
                    y++;
                }
            }

            const char *footer[] = {"[Enter] Retry", "[Esc] Back to steps", NULL};
            render_footer(modal, footer);
            wrefresh(modal);

            // Wait for user choice.
            int key;
            while ((key = getch()) != '\n' && key != 27);

            if (key == '\n')
            {
                // Retry installation immediately.
                continue;
            }

            // Go back to steps.
            if (store->partition_method == METHOD_EASY)
            {
                step = 2; // Go to method selection to switch to manual.
            }
            else
            {
                step = 4; // Go to partition configuration.
            }

            // Re-enter the wizard loop.
            while (step <= 5)
            {
                int res = 0;

                switch (step)
                {
                    case 1:
                        res = run_locale_step(modal);
                        break;
                    case 2:
                        res = run_method_step(modal);
                        break;
                    case 3:
                        res = run_disk_step(modal);
                        break;
                    case 4:
                        if (store->partition_method == METHOD_EASY)
                        {
                            res = run_auto_partition_step(modal);
                        }
                        else
                        {
                            res = run_partition_step(modal);
                        }
                        break;
                    case 5:
                        res = run_confirmation_step(modal);
                        break;
                }

                if (res)
                {
                    step++;
                }
                else if (step > 1)
                {
                    step--;
                }
            }
            // Loop will continue and retry installation.
        }
    }

    // Wait for final input before exiting.
    await_step_input(modal);

    // Cleanup ncurses resources before exit.
    destroy_modal(modal);
    cleanup_ui();

    return result;
}
