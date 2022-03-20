// Inter-Integrated Circuit (I2C) Library for the Raspberry Pi
//
// ... but this time in the CLI
//
// Copyright (c) 2022 Benjamin Spencer
// ============================================================================
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// ============================================================================

// Include C standard libraries:
#include <stdlib.h> // C Standard library
#include <stdio.h> // C Standard I/O libary
#include <string.h> // C Standard string manipulation libary

// Include C POSIX libraries:
#include <unistd.h> // Symbolic constants and types library
#include <getopt.h> // Command-line arguments parser
#include <ctype.h>  // C compatible data types

// Include header files:
#include "parse_data.h"      // Parse input string list of hex numbers
                             // into an integer array
#include "check_if_number.h" // Check if a string only contains numbers
#include <pi_i2c.h>          // Pi I2C library!

#define MAX_DATA_BYTES 100 // The maximum number of bytes to write (arbritary)

// The lovely help message
static void help(void) {
    printf("Usage: pi_i2c [OPTIONS] \n");
    printf("Inter-Integrated Circuit (I2C) Library for the Raspberry Pi\n\n");

    printf("Examples:\n");
    printf("  pi_i2c --sda 2 --scl 3 -- speed-grade i2c_standard_mode --scan\n");
    printf("  pi_i2c -r --sda 2 --scl 3 --speed-grade 400 --device 0x1C --register 0x23 --bytes 2\n");
    printf("  pi_i2c -w --sda 2 --scl 3 --speed-grade 100 --device 0x3D --register 0x01 --bytes 1 --data 0xFF,0x1C\n");
    printf("\n");

    printf("Main operation mode:\n");
    printf("  -r, --read         read N bytes from a device and register address\n");
    printf("  -w, --write        write N bytes from a device and register address\n");
    printf("  -a, --sda          GPIO pin to use for the SDA line (BCM numbering)\n");
    printf("  -c, --scl          GPIO pin to use for the SCL line (BCM numbering)\n");
    printf("  -g, --speed-grade  I2C bus speed grade (bit rate) as defined by pi_i2c.h\n");
    printf("                     valid speed grades are i2c_standard_mode (100) or i2c_full_speed (400)\n");
    printf("  -n, --n-bytes      number of bytes to read or write to the device's register address\n");
    printf("  -d, --data         data to write to the device's register address in a comma delimited list\n");
    printf("                     data type is hex (0xNN)\n");
    printf("  -v, --verbose      display logs and other useful information\n");
    printf("  -h, --help         display this help and exit\n");
}

int main(int argc, char **argv) {
    char *base;

    // Assign values such that we can confidently check if they were not set:
    unsigned int sda_gpio_pin = 99;
    unsigned int scl_gpio_pin = 99;
    unsigned int speed_grade = 0;
    unsigned int n_bytes = 0;
    char *data_string = NULL;

    int data_parsed[MAX_DATA_BYTES]; // Cannot write more data than this

    int address_book[127];

    unsigned int n_bytes_parsed;

    int getopt_ret;
    int option_index = 0;

    int i;
    int j;
    int ret;

    int verbose = 0;
    int write = 0;
    int read = 0;
    int scan = 0;

    // Getopt long options defined here:
    static struct option long_options[] = {
        {"help",        no_argument,       NULL, 'h'},
        {"sda",         required_argument, NULL, 'a'},
        {"scl",         required_argument, NULL, 'c'},
        {"verbose",     no_argument,       NULL, 'v'},
        {"speed-grade", required_argument, NULL, 'g'},
        {"n-bytes",     required_argument, NULL, 'n'},
        {"read",        no_argument,       NULL, 'r'},
        {"write",       no_argument,       NULL, 'w'},
        {"data",        required_argument, NULL, 'd'},
        {"scan",        no_argument,       NULL, 's'},
        {NULL,          0,                 NULL, 0}
    };

    // If no options were passed then give them some help:
    if (argc < 2) {
        help();
        return -1;
    }

    // Getopt loop (will exit when no more options)
    while (1) {
        getopt_ret = getopt_long(argc, argv, "-:ha:c:vg:n:rwd:",
                                 long_options, &option_index);

        if (getopt_ret == -1)
            break;

        switch (getopt_ret) {
            // --sda
            case 'a':
                // Convert to integer from the input string:
                if (check_if_number(optarg)) {
                    sda_gpio_pin = strtol(optarg, &base, 10);
                } else {
                    printf("pi_i2c: --sda option must be a number within 0 and 31\n");
                    printf("pi_i2c: error is not recoverable; exiting now\n");
                    return -1;
                }

                break;

            // --scl
            case 'c':
                // Convert to integer from the input string:
                if (check_if_number(optarg)) {
                    scl_gpio_pin = strtol(optarg, &base, 10);
                } else {
                    printf("pi_i2c: --scl option must be a number within 0 and 31\n");
                    printf("pi_i2c: error is not recoverable; exiting now\n");
                    return -1;
                }

                break;

            // --speed-grade
            case 'g':
                // Convert to integer from the input string:
                if (check_if_number(optarg)) {
                    speed_grade = strtol(optarg, &base, 10) * 1e3;
                // Match input string to integer instead:
                } else if (strcmp(optarg, "i2c_standard_mode") == 0) {
                    speed_grade = I2C_STANDARD_MODE;
                } else if (strcmp(optarg, "i2c_full_speed") == 0) {
                    speed_grade = I2C_FULL_SPEED;
                } else {
                    printf("pi_i2c: --speed-grade option must i2c_standard_mode (100) or i2c_full_speed (400)\n");
                    printf("pi_i2c: error is not recoverable; exiting now\n");
                    return -1;
                }

                break;

            // --n-bytes
            case 'n':
                // Convert to integer from the input string:
                if (check_if_number(optarg)) {
                    n_bytes = strtol(optarg, &base, 10);
                }

                break;

            // --data
            case 'd':
                // Save contents for later parsing:
                data_string = optarg;
                break;

            // --read
            case 'r':
                read = 1;
                break;

            // --write
            case 'w':
                write = 1;
                break;

            case 's':
                scan = 1;
                break;

            // --help
            case 'h':
                help();
                return 0;

            // --verbose
            case 'v':
                verbose = 1;
                break;

            case '?':
                printf("pi_i2c: unknown option %c\n", optopt);
                printf("pi_i2c: try 'pi_i2c --help' for more information\n");
                return -1;

            case ':':
                printf("pi_i2c: missing option for %c\n", optopt);
                printf("pi_i2c: try 'pi_i2c --help' for more information\n");
                return -1;

            default:
                help();
                return -1;
            }
    }

    if (sda_gpio_pin >= 31) {
        printf("pi_i2c: --sda option must be within 0 and 31\n");
        printf("pi_i2c: error is not recoverable; exiting now\n");
        return -1;
    }

    if (scl_gpio_pin >= 31) {
        printf("pi_i2c: --scl option must be within 0 and 31\n");
        printf("pi_i2c: error is not recoverable; exiting now\n");
        return -1;
    }

    if ((speed_grade != I2C_STANDARD_MODE) && (speed_grade != I2C_FULL_SPEED)) {
        printf("pi_i2c: --speed-grade option must i2c_standard_mode (100) or i2c_full_speed (400)\n");
        printf("pi_i2c: error is not recoverable; exiting now\n");
        return -1;
    }

    // Can configure pi_i2c at this point:
    if (ret = config_i2c(sda_gpio_pin, scl_gpio_pin, speed_grade) < 0) {
        printf("pi_i2c: config_i2c returned an error %d\n", ret);
        printf("pi_i2c: error is not recoverable; exiting now\n");
    };

    if (scan) {
        if (ret = scan_bus_i2c(address_book) < 0) {
            printf("pi_i2c: scan_bus_i2c returned an error %d\n", ret);
            printf("pi_i2c: error is not recoverable; exiting now\n");
            return -1;
        };

        // Print the scan results in a nice table:
        printf("    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
        for (i = 0; i < 8; i++) {
            printf("%d0 ", i);

            for (j = 0; j < 16; j++) {
                if (address_book[i*16 + j] == 1) {
                    printf(" x ");
                } else {
                    printf(" - ");
                }
            }
            printf("\n");
        }

        printf("Address detected: [");
        for (i = 0; i < 127; i+=2) {
            if (address_book[i] == 1) {
                printf("0x%X, ", i);
            }
        }
        printf("\b\b]\n");

        return 0;
    }

    // Option argument check prior to any pi_i2c calls. Don't allow any
    // non-sensical arguments get through so error out if found:
    if ((read && write) || (!read && !write)) {
        printf("pi_i2c: must choose --read or --write but not both or none\n");
        printf("pi_i2c: error is not recoverable; exiting now\n");
        return -1;
    }

    if (n_bytes == 0) {
        printf("pi_i2c: number of bytes must be grater than 0\n");
        printf("pi_i2c: error is not recoverable; exiting now\n");
        return -1;
    }

    if ((write) && (data_string == NULL)) {
        printf("pi_i2c: must include --data option and provide arguments in a comma delimited list\n");
        printf("pi_i2c: error is not recoverable; exiting now\n");
        return -1;
    }

    // Parse input data into an integer array; error out if parsing failed
    // due to formatting issues:
    if (!(n_bytes_parsed = parse_data(data_string, data_parsed))) {
        printf("pi_i2c: --data arguments must be a comma delimited list with proper hex numbers (0xNN)\n");
        printf("pi_i2c: error is not recoverable; exiting now\n");
        return -1;
    };

    // Error out if number of bytes parsed does not match the option argument:
    if (n_bytes_parsed != n_bytes) {
        printf("pi_i2c: --n-bytes must match the size of the --data list provided\n");
        printf("pi_i2c: error is not recoverable; exiting now\n");
        return -1;
    }

    if (verbose) {
        printf("pi_i2c: [OPTIONS]\n");
        printf("  --read        = %d\n", read);
        printf("  --write       = %d\n", write);
        printf("  --sda         = %d\n", sda_gpio_pin);
        printf("  --scl         = %d\n", scl_gpio_pin);
        printf("  --speed-grade = %d\n", speed_grade);
        printf("  --n-bytes     = %d\n", n_bytes);
        printf("  --data        = ");
        for (i = 0; i < n_bytes; i++) {
            printf("0x%X ", data_parsed[i]);
        }
        printf("\n");
        printf("pi_i2c: parsing\n");
        printf("  n_bytes_parsed  = %d\n", n_bytes_parsed);
    }

    return 0;
}