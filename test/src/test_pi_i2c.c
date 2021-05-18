// Include C standard libraries:
#include <stdlib.h> // C Standard library
#include <stdio.h>  // C Standard I/O libary
#include <time.h>   // C Standard date and time manipulation

#include <pi_i2c.h> // Pi I2C library!

// Test I2C bus scan functionality and print results to the screen
void test_scan_bus_i2c(void) {
    int i;
    int ret;

    // Address book returned by function:
    int i2c_address_book[127];

    printf("Testing scan_bus_i2c()\n");

    // Immediately return if the scan failed:
    if ((ret = scan_bus_i2c(i2c_address_book)) < 0) {
        printf("Error! scan_i2c() returned %d\n\n", ret);
        return;
    };

    printf("i2c_scan() has returned %d\n", ret);

    printf("Following address have been detected: \n");

    for (i = 0; i < 127; i+=2) {
        if (i2c_address_book[i] == 1) {
            printf("0x%X\n", i);
        }
    }

    printf("Test complete\n");
}

// Test I2C write capability
void test_write_i2c_one_byte(int slave_address, int register_address,
                             int *data, int n_bytes) {
    int ret;

    printf("Testing write_i2c()\n");
    printf("slave_address = 0x%X\n", slave_address);
    printf("register_address = 0x%X\n", register_address);
    printf("data = 0x%X\n", data[0]);
    printf("n_bytes = %d\n", n_bytes);

    // Write a byte to the slave:
    ret = write_i2c(slave_address, register_address, data, n_bytes);

    printf("write_i2c() has returned %d\n", ret);
    printf("Test complete\n");
}

// Test I2C write capability over a number of iterations
// to calculate success rate:
void test_write_i2c_iterative(int slave_address, int register_address,
                             int *data, int n_bytes, int iterations) {
    int i;
    int ret;

    printf("Testing write_i2c() iteratively\n");
    printf("slave_address = 0x%X\n", slave_address);
    printf("register_address = 0x%X\n", register_address);
    printf("data = 0x%X\n", data[0]);
    printf("n_bytes = %d\n", n_bytes);
    printf("Running %d iterations\n", iterations);

    // Write a byte to the slave multiple times in a row:
    for (i = 0; i < iterations; i++) {
        ret = write_i2c(slave_address, register_address, data, n_bytes);

        printf("%d: write_i2c() has returned %d\n", i, ret);
    }

    printf("Test complete\n");
}

// Test I2C write capability
void test_read_i2c_one_byte(int slave_address, int register_address,
                            int *data, int n_bytes) {
    int ret;

    printf("Testing read_i2c()\n");
    printf("slave_address = 0x%X\n", slave_address);
    printf("register_address = 0x%X\n", register_address);
    printf("data = 0x%X\n", data[0]);
    printf("n_bytes = %d\n", n_bytes);

    // Read a byte from the slave:
    ret = read_i2c(slave_address, register_address, data, n_bytes);

    printf("read_i2c() has returned %d\n", ret);
    printf("Byte read = 0x%X\n", data[0]);
    printf("Test complete\n");
}

// Test I2C read capability over a number of iterations
// to calculate success rate:
void test_read_i2c_iterative(int slave_address, int register_address,
                             int *data, int n_bytes, int iterations) {
    int i;
    int ret;

    printf("Testing read_i2c() iteratively\n");
    printf("slave_address = 0x%X\n", slave_address);
    printf("register_address = 0x%X\n", register_address);
    printf("data = 0x%X\n", data[0]);
    printf("n_bytes = %d\n", n_bytes);
    printf("Running %d iterations\n", iterations);

    // Read a byte from the slave multiple times in a row:
    for (i = 0; i < iterations; i++) {
        ret = read_i2c(slave_address, register_address, data, n_bytes);

        printf("%d: read_i2c() has returned %d (Byte read = 0x%X)\n",
               i, ret, data[0]);
    }

    printf("Test complete\n");
}

void test_read_i2c_multiple_bytes(int slave_address, int register_address,
                                  int *data, int n_bytes) {
    int i;
    int ret;

    printf("Testing read_i2c()\n");
    printf("slave_address = 0x%X\n", slave_address);
    printf("register_address = 0x%X\n", register_address);
    printf("data = 0x%X\n", data[0]);
    printf("n_bytes = %d\n", n_bytes);

    // Read a byte from the slave:
    ret = read_i2c(slave_address, register_address, data, n_bytes);

    printf("read_i2c() has returned %d\n", ret);

    // Print out all bytes read:
    for (i = 0; i < n_bytes; i++) {
        printf("Byte read %d = 0x%X\n", i, data[i]);
    }

    printf("Test complete\n");
}

void test_get_statistics_i2c(void) {
    printf("Testing get_statistics_i2c()\n");

    // Get all the statisitcs after testing functionality:
    struct pi_i2c_statistics statistics = get_statistics_i2c();

    printf("get_statistics_i2c() has returned the following:\n");
    printf("num_start_cond = %d\n", statistics.num_start_cond);
    printf("num_repeated_start_cond = %d\n",
           statistics.num_repeated_start_cond);
    printf("num_stop_cond = %d\n", statistics.num_stop_cond);
    printf("num_bytes_written = %d\n", statistics.num_bytes_written);
    printf("num_bytes_read = %d\n", statistics.num_bytes_read);
    printf("num_nack = %d\n", statistics.num_nack);
    printf("num_nack_rst = %d\n", statistics.num_nack_rst);
    printf("num_bad_reg = %d\n", statistics.num_bad_reg);
    printf("num_badxfr = %d\n", statistics.num_badxfr);
    printf("num_bus_resets = %d\n", statistics.num_bus_resets);
    printf("num_unknown_bus_errors = %d\n", statistics.num_unknown_bus_errors);
    printf("num_bus_lockups = %d\n", statistics.num_bus_lockups);
    printf("num_failed_start_cond = %d\n", statistics.num_failed_start_cond);
    printf("num_failed_stop_cond = %d\n", statistics.num_failed_stop_cond);
    printf("num_slave_hung = %d\n", statistics.num_slave_hung);
    printf("num_clock_stretching_timeouts = %d\n",
           statistics.num_clock_stretching_timeouts);
    printf("num_clock_stretch = %d\n", statistics.num_clock_stretch);
    printf("Test complete\n");
}

void speed_test_read_i2c(int slave_address, int register_address,
                         int *data, int n_bytes, int iterations) {
    int i;
    int j;
    int ret;

    struct timespec start;
    struct timespec end;

    float run_time;
    float round_trip_data_rate[iterations];
    float avg_round_trip_data_rate = 0;

    printf("Running speed test for read_i2c()\n");
    printf("slave_address = 0x%X\n", slave_address);
    printf("register_address = 0x%X\n", register_address);
    printf("data = 0x%X\n", data[0]);
    printf("n_bytes = %d\n", n_bytes);
    printf("iterations = %d\n", iterations);

    for (i = 0; i < iterations; i++) {
        // Grab the start time:
        clock_gettime(CLOCK_MONOTONIC, &start);

        // Read a byte from the slave:
        ret = read_i2c(slave_address, register_address, data, n_bytes);

        // Grab the stop time:
        clock_gettime(CLOCK_MONOTONIC, &end);

        printf("read_i2c() has returned %d\n", ret);

        // Print out all bytes read:
        printf("Bytes read: ");

        for (j = 0; j < n_bytes; j++) {
            printf("{%d = 0x%X} ", j, data[j]);
        }

        // Calculate the "round trip" data rate:
        run_time = (((double) end.tv_sec + 1.0e-9 * end.tv_nsec) -
                    ((double) start.tv_sec + 1.0e-9 * start.tv_nsec));

        round_trip_data_rate[i] = (float) n_bytes * 8 / run_time;
        avg_round_trip_data_rate += round_trip_data_rate[i];

        printf("\nTime to read %d bytes: %.3f seconds\n", n_bytes, run_time);
        printf("Round trip effective data rate: %.3f kbps\n",
               round_trip_data_rate[i] / 1e3);
    }

    // Finally get the averaged "round trip" data rate:
    avg_round_trip_data_rate = avg_round_trip_data_rate / iterations;

    printf("Average round trip effective data rate: %.3f kbps\n",
           avg_round_trip_data_rate / 1e3);

    printf("Test complete\n");
}

void speed_test_write_i2c(int slave_address, int register_address,
                          int *data, int n_bytes, int iterations) {
    int i;
    int ret;

    struct timespec start;
    struct timespec end;

    float run_time;
    float round_trip_data_rate[iterations];
    float avg_round_trip_data_rate = 0;

    printf("Running speed test for write_i2c()\n");
    printf("slave_address = 0x%X\n", slave_address);
    printf("register_address = 0x%X\n", register_address);
    printf("data = 0x%X\n", data[0]);
    printf("n_bytes = %d\n", n_bytes);
    printf("iterations = %d\n", iterations);

    for (i = 0; i < iterations; i++) {
        // Grab the start time:
        clock_gettime(CLOCK_MONOTONIC, &start);

        // Read a byte from the slave:
        ret = write_i2c(slave_address, register_address, data, n_bytes);

        // Grab the stop time:
        clock_gettime(CLOCK_MONOTONIC, &end);

        printf("write_i2c() has returned %d\n", ret);

        // Calculate the "round trip" data rate:
        run_time = (((double) end.tv_sec + 1.0e-9 * end.tv_nsec) -
                    ((double) start.tv_sec + 1.0e-9 * start.tv_nsec));

        round_trip_data_rate[i] = (float) n_bytes * 8 / run_time;
        avg_round_trip_data_rate += round_trip_data_rate[i];

        printf("Time to write %d bytes: %.3f seconds\n", n_bytes, run_time);
        printf("Round trip effective data rate: %.3f kbps\n",
               round_trip_data_rate[i] / 1e3);
    }

    // Finally get the averaged "round trip" data rate:
    avg_round_trip_data_rate = avg_round_trip_data_rate / iterations;

    printf("Average round trip effective data rate: %.3f kbps\n",
           avg_round_trip_data_rate / 1e3);

    printf("Test complete\n");
}

void test_get_configs_i2c(void) {
    printf("Testing test_get_configs_i2c()\n");

    // Get all the statisitcs after testing functionality:
    struct pi_i2c_configs configs = get_configs_i2c();

    printf("test_get_configs_i2c() has returned the following:\n");
    printf("scl_t_low_sleep_us = %ld\n", configs.scl_t_low_sleep_us);
    printf("scl_t_high_sleep_us = %ld\n", configs.scl_t_high_sleep_us);
    printf("scl_actual_clock_frequency_hz = %.3f\n",
           configs.scl_actual_clock_frequency_hz);
    printf("Test complete\n");
}

void main(void) {
    // Use the default I2C pins:
    // (Ensure that Raspian I2C interface is disabled via rasp-config otherwise
    //  risk unpredictable behavior)
    int sda_pin = 2;
    int scl_pin = 3;

    int speed_grade = I2C_FULL_SPEED;

    // Addresses & data to use when testing write:
    int write_slave_address = 0x1C;
    int write_register_address = 0x21;
    int write_data[1] = {0x00};
    int write_bytes = 1;
    int write_iterations = 10;

    int write_slave_address_multiple = 0x1C;
    int write_register_address_multiple = 0x23;
    int write_data_multiple[2] = {0x0, 0x0};
    int write_bytes_multiple = 2;

    int read_slave_address = 0x1C;
    int read_register_address = 0x0F;
    int read_bytes = 1;
    int read_iterations = 10;
    int read_data[1];

    int read_slave_address_multiple = 0x1C;
    int read_register_address_multiple = 0x28;
    int read_bytes_multiple = 2;
    int read_data_multiple[read_bytes_multiple];

    printf("Begin pi_i2c_test.c\n");

    printf("Configuring pi_i2c:\n");
    printf("sda_pin = %d\n", sda_pin);
    printf("sda_pin = %d\n", scl_pin);
    printf("speed_grade = %d Hz\n", speed_grade);

    // Configure at standard mode:
    config_i2c(sda_pin, scl_pin, speed_grade);

    // Return back useful numbers to know:
    test_get_configs_i2c();

    // Scan I2C bus and identify present slaves:
    test_scan_bus_i2c();

    // Test I2C write one-shot:
    test_write_i2c_one_byte(write_slave_address, write_register_address,
                            write_data, write_bytes);

    // Test I2C read one-shot:
    test_read_i2c_one_byte(read_slave_address, read_register_address,
                           read_data, read_bytes);

    // Test iterations of write to see success of consecutive writes:
    test_write_i2c_iterative(write_slave_address,
                             write_register_address,
                             write_data, write_bytes,
                             write_iterations);

    // Test iterations of read to see success of consecutive reads:
    test_read_i2c_iterative(read_slave_address,
                            read_register_address,
                            read_data, read_bytes,
                            read_iterations);

    // Test reading multiple bytes to see success:
    test_read_i2c_multiple_bytes(read_slave_address_multiple,
                                 read_register_address_multiple,
                                 read_data_multiple, read_bytes_multiple);

    // Test reading multiple bytes to find effective data rate:
    speed_test_read_i2c(read_slave_address_multiple,
                        read_register_address_multiple,
                        read_data_multiple, read_bytes_multiple, 
                        read_bytes_multiple);

    // Test writing multiple bytes to find effectve data rate:
    speed_test_write_i2c(write_slave_address_multiple,
                         write_register_address_multiple,
                         write_data_multiple, write_bytes_multiple, 
                         write_bytes_multiple);

    // Test get statistics following all of the test calls:
    test_get_statistics_i2c();

}
