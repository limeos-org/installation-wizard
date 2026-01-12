/**
 * This code is responsible for letting the user choose between
 * Easy (automatic) and Manual partitioning methods.
 */

#include "../all.h"

int run_method_step(WINDOW *modal)
{
    Store *store = get_store();

    // Define method options with descriptions.
    StepOption options[2] = {
        {"easy", "Easy (Recommended) - Auto partition based on disk size"},
        {"manual", "Manual - Full control over partition layout"}
    };
    int count = 2;

    // Mark previously selected method if any.
    int selected = (store->partition_method == METHOD_EASY) ? 0 : 1;
    if (store->partition_method == METHOD_EASY)
    {
        strcat(options[0].label, " *");
    }
    else if (store->partition_count > 0)
    {
        // Only mark manual if user has configured partitions.
        strcat(options[1].label, " *");
    }

    // Run selection step for method choice.
    int result = run_selection_step(
        modal,
        "Partition Method",
        2,
        "Choose how to configure partitions:",
        options,
        count,
        &selected,
        1
    );

    if (result)
    {
        // Store the selected method.
        if (selected == 0)
        {
            store->partition_method = METHOD_EASY;
        }
        else
        {
            store->partition_method = METHOD_MANUAL;
        }
    }

    return result;
}
