/**
 * This code is responsible for the main entry point of the installation
 * wizard. It initializes the UI, runs through each installation step, and
 * executes the installation process.
 */

#include "all.h"

int main(int argc, char *argv[])
{
    Store *store = get_store();

    // Parse command-line arguments.
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--dry") == 0)
        {
            store->dry_run = 1;
        }
    }

    // Initialize ncurses UI.
    init_ui();

    // Create the centered modal window for wizard content.
    WINDOW *modal = create_modal("Installation Wizard");

    // A loop that runs throughout the entire wizard process and waits for user
    // input at each step, allowing back-and-forth navigation between steps.
    int step = 1;
    while (step <= 4) {
        int result = 0;

        switch (step) {
            case 1:
                result = run_locale_step(modal);
                break;
            case 2:
                result = run_disk_step(modal);
                break;
            case 3:
                result = run_partition_step(modal);
                break;
            case 4:
                result = run_confirmation_step(modal);
                break;
        }

        if (result) {
            step++; // Move to next step.
        } else if (step > 1) {
            step--; // Go back to previous step.
        }
        // If step == 1 and result == 0, stay on step 1.
    }

    // Run installation using settings from global state.
    int result = run_install(modal);

    // Wait for final input before exiting.
    await_step_input(modal);

    // Cleanup ncurses resources before exit.
    destroy_modal(modal);
    cleanup_ui();

    return result;
}
