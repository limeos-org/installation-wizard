/**
 * This code is responsible for testing the disk utility functions, including
 * size formatting, partition size calculations, and device path construction.
 */

#include "../all.h"

/**
 * Sets up the test environment before each test.
 */
static int setup(void **state)
{
    (void)state;
    reset_store();
    return 0;
}

/**
 * Cleans up the test environment after each test.
 */
static int teardown(void **state)
{
    (void)state;
    return 0;
}

/**
 * Verifies format_disk_size() correctly formats byte values.
 */
static void test_format_disk_size_bytes(void **state)
{
    (void)state;
    char buffer[32];

    format_disk_size(512, buffer, sizeof(buffer));

    assert_string_equal("512 B", buffer);
}

/**
 * Verifies format_disk_size() handles zero bytes.
 */
static void test_format_disk_size_zero_bytes(void **state)
{
    (void)state;
    char buffer[32];

    format_disk_size(0, buffer, sizeof(buffer));

    assert_string_equal("0 B", buffer);
}

/**
 * Verifies format_disk_size() correctly formats megabyte values.
 */
static void test_format_disk_size_megabytes(void **state)
{
    (void)state;
    char buffer[32];

    format_disk_size(500000000, buffer, sizeof(buffer));

    assert_string_equal("500 MB", buffer);
}

/**
 * Verifies format_disk_size() correctly formats one gigabyte.
 */
static void test_format_disk_size_one_gigabyte(void **state)
{
    (void)state;
    char buffer[32];

    format_disk_size(1000000000, buffer, sizeof(buffer));

    assert_string_equal("1 GB", buffer);
}

/**
 * Verifies format_disk_size() correctly formats 500 gigabytes.
 */
static void test_format_disk_size_500_gigabytes(void **state)
{
    (void)state;
    char buffer[32];

    format_disk_size(500000000000ULL, buffer, sizeof(buffer));

    assert_string_equal("500 GB", buffer);
}

/**
 * Verifies format_disk_size() correctly formats one terabyte.
 */
static void test_format_disk_size_one_terabyte(void **state)
{
    (void)state;
    char buffer[32];

    format_disk_size(1000000000000ULL, buffer, sizeof(buffer));

    assert_string_equal("1 TB", buffer);
}

/**
 * Verifies format_disk_size() correctly formats 4 terabytes.
 */
static void test_format_disk_size_4_terabytes(void **state)
{
    (void)state;
    char buffer[32];

    format_disk_size(4000000000000ULL, buffer, sizeof(buffer));

    assert_string_equal("4 TB", buffer);
}

/**
 * Verifies format_disk_size() output fits in a minimal buffer.
 */
static void test_format_disk_size_small_buffer(void **state)
{
    (void)state;
    char buffer[8];

    // Verify output fits in minimal buffer size.
    format_disk_size(1000000000000ULL, buffer, sizeof(buffer));

    assert_string_equal("1 TB", buffer);
}

/**
 * Verifies sum_partition_sizes() returns zero for empty partition array.
 */
static void test_sum_partition_sizes_empty(void **state)
{
    (void)state;
    Partition partitions[1] = {0};

    unsigned long long total = sum_partition_sizes(partitions, 0);

    assert_int_equal(0, (int)total);
}

/**
 * Verifies sum_partition_sizes() correctly sums a single partition.
 */
static void test_sum_partition_sizes_single(void **state)
{
    (void)state;
    Partition partitions[1] = {
        {.size_bytes = 1000000000}
    };

    unsigned long long total = sum_partition_sizes(partitions, 1);

    assert_true(total == 1000000000ULL);
}

/**
 * Verifies sum_partition_sizes() correctly sums multiple partitions.
 */
static void test_sum_partition_sizes_multiple(void **state)
{
    (void)state;
    Partition partitions[3] = {
        {.size_bytes = 512000000},
        {.size_bytes = 1000000000},
        {.size_bytes = 2000000000}
    };

    unsigned long long total = sum_partition_sizes(partitions, 3);

    assert_true(total == 3512000000ULL);
}

/**
 * Verifies sum_partition_sizes() handles terabyte-scale values without
 * overflow.
 */
static void test_sum_partition_sizes_large_values(void **state)
{
    (void)state;
    // Test with terabyte-scale partitions to verify no overflow.
    Partition partitions[2] = {
        {.size_bytes = 2000000000000ULL},
        {.size_bytes = 2000000000000ULL}
    };

    unsigned long long total = sum_partition_sizes(partitions, 2);

    assert_true(total == 4000000000000ULL);
}

/**
 * Verifies get_partition_device() constructs correct path for first SATA
 * partition.
 */
static void test_get_partition_device_sata_first(void **state)
{
    (void)state;
    char buffer[64];

    get_partition_device("/dev/sda", 1, buffer, sizeof(buffer));

    assert_string_equal("/dev/sda1", buffer);
}

/**
 * Verifies get_partition_device() handles double-digit partition numbers.
 */
static void test_get_partition_device_sata_tenth(void **state)
{
    (void)state;
    char buffer[64];

    // Verify double-digit partition numbers work correctly.
    get_partition_device("/dev/sda", 10, buffer, sizeof(buffer));

    assert_string_equal("/dev/sda10", buffer);
}

/**
 * Verifies get_partition_device() works with secondary SATA disk.
 */
static void test_get_partition_device_sdb(void **state)
{
    (void)state;
    char buffer[64];

    get_partition_device("/dev/sdb", 2, buffer, sizeof(buffer));

    assert_string_equal("/dev/sdb2", buffer);
}

/**
 * Verifies get_partition_device() uses 'p' separator for NVMe devices.
 */
static void test_get_partition_device_nvme(void **state)
{
    (void)state;
    char buffer[64];

    // NVMe devices require 'p' separator before partition number.
    get_partition_device("/dev/nvme0n1", 1, buffer, sizeof(buffer));

    assert_string_equal("/dev/nvme0n1p1", buffer);
}

/**
 * Verifies get_partition_device() handles second NVMe partition.
 */
static void test_get_partition_device_nvme_second(void **state)
{
    (void)state;
    char buffer[64];

    get_partition_device("/dev/nvme0n1", 2, buffer, sizeof(buffer));

    assert_string_equal("/dev/nvme0n1p2", buffer);
}

/**
 * Verifies get_partition_device() works with second NVMe controller.
 */
static void test_get_partition_device_nvme_high_number(void **state)
{
    (void)state;
    char buffer[64];

    // Test second NVMe controller with higher partition number.
    get_partition_device("/dev/nvme1n1", 5, buffer, sizeof(buffer));

    assert_string_equal("/dev/nvme1n1p5", buffer);
}

/**
 * Verifies get_partition_device() uses 'p' separator for MMC devices.
 */
static void test_get_partition_device_mmc(void **state)
{
    (void)state;
    char buffer[64];

    // MMC devices also require 'p' separator like NVMe.
    get_partition_device("/dev/mmcblk0", 1, buffer, sizeof(buffer));

    assert_string_equal("/dev/mmcblk0p1", buffer);
}

/**
 * Verifies get_partition_device() handles second MMC partition.
 */
static void test_get_partition_device_mmc_second(void **state)
{
    (void)state;
    char buffer[64];

    get_partition_device("/dev/mmcblk0", 2, buffer, sizeof(buffer));

    assert_string_equal("/dev/mmcblk0p2", buffer);
}

/**
 * Verifies get_partition_device() uses standard naming for VirtIO devices.
 */
static void test_get_partition_device_vda(void **state)
{
    (void)state;
    char buffer[64];

    // VirtIO devices follow standard naming without 'p' separator.
    get_partition_device("/dev/vda", 1, buffer, sizeof(buffer));

    assert_string_equal("/dev/vda1", buffer);
}

/**
 * Verifies get_partition_device() uses standard naming for Xen devices.
 */
static void test_get_partition_device_xvda(void **state)
{
    (void)state;
    char buffer[64];

    // Xen virtual devices follow standard naming.
    get_partition_device("/dev/xvda", 3, buffer, sizeof(buffer));

    assert_string_equal("/dev/xvda3", buffer);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_format_disk_size_bytes, setup, teardown),
        cmocka_unit_test_setup_teardown(test_format_disk_size_zero_bytes, setup, teardown),
        cmocka_unit_test_setup_teardown(test_format_disk_size_megabytes, setup, teardown),
        cmocka_unit_test_setup_teardown(test_format_disk_size_one_gigabyte, setup, teardown),
        cmocka_unit_test_setup_teardown(test_format_disk_size_500_gigabytes, setup, teardown),
        cmocka_unit_test_setup_teardown(test_format_disk_size_one_terabyte, setup, teardown),
        cmocka_unit_test_setup_teardown(test_format_disk_size_4_terabytes, setup, teardown),
        cmocka_unit_test_setup_teardown(test_format_disk_size_small_buffer, setup, teardown),
        cmocka_unit_test_setup_teardown(test_sum_partition_sizes_empty, setup, teardown),
        cmocka_unit_test_setup_teardown(test_sum_partition_sizes_single, setup, teardown),
        cmocka_unit_test_setup_teardown(test_sum_partition_sizes_multiple, setup, teardown),
        cmocka_unit_test_setup_teardown(test_sum_partition_sizes_large_values, setup, teardown),
        cmocka_unit_test_setup_teardown(test_get_partition_device_sata_first, setup, teardown),
        cmocka_unit_test_setup_teardown(test_get_partition_device_sata_tenth, setup, teardown),
        cmocka_unit_test_setup_teardown(test_get_partition_device_sdb, setup, teardown),
        cmocka_unit_test_setup_teardown(test_get_partition_device_nvme, setup, teardown),
        cmocka_unit_test_setup_teardown(test_get_partition_device_nvme_second, setup, teardown),
        cmocka_unit_test_setup_teardown(test_get_partition_device_nvme_high_number, setup, teardown),
        cmocka_unit_test_setup_teardown(test_get_partition_device_mmc, setup, teardown),
        cmocka_unit_test_setup_teardown(test_get_partition_device_mmc_second, setup, teardown),
        cmocka_unit_test_setup_teardown(test_get_partition_device_vda, setup, teardown),
        cmocka_unit_test_setup_teardown(test_get_partition_device_xvda, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
