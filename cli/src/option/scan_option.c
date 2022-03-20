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
#include <stdio.h> // C Standard I/O libary

// Include header files:
#include <pi_i2c.h> // Pi I2C library!

// Scan the I2C bus and print the results nicely to the terminal
int scan_option(void) {
    int i;
    int j;
    int ret;
    int address_book[127];

    // Scan the I2C bus
    if ((ret = scan_bus_i2c(address_book) < 0)) {
        return ret;
    };

    // Print the scan results in a nice table:
    printf("pi_i2c: I2C bus scan completed\n");
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

    printf("devices detected at [");
    for (i = 0; i < 127; i++) {
        if (address_book[i] == 1) {
            printf("0x%X, ", i);
        }
    }
    printf("\b\b]\n");

    return 0;
}