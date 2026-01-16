/**
 * This code is responsible for testing the system utility module,
 * including chassis detection and RAM detection.
 */

#include "../../all.h"

/** Sets up the test environment before each test. */
static int setup(void **state)
{
    (void)state;
    return 0;
}

/** Cleans up the test environment after each test. */
static int teardown(void **state)
{
    (void)state;
    return 0;
}

/** Verifies detect_system_chassis() returns valid enum value. */
static void test_detect_system_chassis_returns_valid_enum(void **state)
{
    (void)state;

    ChassisType type = detect_system_chassis();

    // Should return one of the valid enum values.
    int is_valid = (type == CHASSIS_DESKTOP) ||
                   (type == CHASSIS_LAPTOP) ||
                   (type == CHASSIS_UNKNOWN);
    assert_true(is_valid);
}

/** Verifies get_system_ram() returns a reasonable value. */
static void test_get_system_ram_returns_value(void **state)
{
    (void)state;

    unsigned long long ram = get_system_ram();

    // Should return non-zero on most systems (at least 128MB).
    // May return 0 if /proc/meminfo is unavailable.
    if (ram > 0)
    {
        assert_true(ram >= 128ULL * 1024 * 1024);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_detect_system_chassis_returns_valid_enum, setup, teardown),
        cmocka_unit_test_setup_teardown(test_get_system_ram_returns_value, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
