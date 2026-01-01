/**
 * This code is responsible for the main entry point of the installation
 * wizard. It initializes the UI, runs through each installation step, and
 * executes the installation process.
 */

#include "all.h"

int main(void)
{
    init_ui();

    // Create the centered modal window for wizard content.
    WINDOW *modal = create_modal("Installation Wizard");

    // Step-based navigation loop.
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
