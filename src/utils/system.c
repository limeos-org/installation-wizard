/**
 * This code is responsible for providing system information utilities
 * such as detecting hardware specifications.
 */

#include "../all.h"

ChassisType detect_system_chassis(void)
{
    // Open the chassis type file from DMI information.
    FILE *file = fopen("/sys/class/dmi/id/chassis_type", "r");
    if (!file)
    {
        return CHASSIS_UNKNOWN;
    }

    // Read the chassis type code.
    int chassis_code = 0;
    if (fscanf(file, "%d", &chassis_code) != 1)
    {
        fclose(file);
        return CHASSIS_UNKNOWN;
    }
    fclose(file);

    // Return chassis type based on code.
    switch (chassis_code)
    {
        case 8:   // Portable
        case 9:   // Laptop
        case 10:  // Notebook
        case 14:  // Sub Notebook
        case 31:  // Convertible
        case 32:  // Detachable
            return CHASSIS_LAPTOP;

        case 3:   // Desktop
        case 4:   // Low Profile Desktop
        case 6:   // Mini Tower
        case 7:   // Tower
        case 13:  // All-in-One
        case 35:  // Mini PC
            return CHASSIS_DESKTOP;

        default:
            return CHASSIS_UNKNOWN;
    }
}

unsigned long long get_system_ram(void)
{
    // Open /proc/meminfo to read memory information.
    FILE *file = fopen("/proc/meminfo", "r");
    if (file == NULL)
    {
        return 0;
    }

    char line[256];
    unsigned long long mem_total_kb = 0;

    // Read lines until we find MemTotal.
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "MemTotal: %llu kB", &mem_total_kb) == 1)
        {
            break;
        }
    }

    fclose(file);

    // Convert from kB to bytes.
    return mem_total_kb * 1024ULL;
}
