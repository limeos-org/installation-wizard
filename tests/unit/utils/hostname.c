/**
 * This code is responsible for testing the hostname utility module.
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

/** Verifies get_default_hostname_suffix() returns non-NULL string. */
static void test_get_default_hostname_suffix_returns_string(void **state)
{
    (void)state;

    const char *suffix = get_default_hostname_suffix();

    assert_non_null(suffix);
    assert_true(strlen(suffix) > 0);
}

/** Verifies get_default_hostname_suffix() returns valid value. */
static void test_get_default_hostname_suffix_valid_values(void **state)
{
    (void)state;

    const char *suffix = get_default_hostname_suffix();

    // Should return either "laptop" or "pc".
    int is_laptop = (strcmp(suffix, "laptop") == 0);
    int is_pc = (strcmp(suffix, "pc") == 0);
    assert_true(is_laptop || is_pc);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_get_default_hostname_suffix_returns_string, setup, teardown),
        cmocka_unit_test_setup_teardown(test_get_default_hostname_suffix_valid_values, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
