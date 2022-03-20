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

// The lovely help message
void help_option(void) {
    printf("Usage: pi_i2c [OPTIONS] \n");
    printf("Inter-Integrated Circuit (I2C) Library for the Raspberry Pi\n\n");

    printf("Examples:\n");
    printf("  pi_i2c --sda 2 --scl 3 -- speed-grade i2c_standard_mode --scan\n");
    printf("  pi_i2c -r --sda 2 --scl 3 --speed-grade 400 --device 0x1C --register 0x23 --bytes 2\n");
    printf("  pi_i2c -w --sda 2 --scl 3 --speed-grade 100 --device 0x3D --register 0x01 --bytes 1 --data 0xFF,0x1C\n");
    printf("  pi_i2c -r -a 2 -c 3 -g i2c_full_speed -e 0x70 -i 0x0 -n 1\n");
    printf("  pi_i2c -w -a 2 -c 3 -g 400 -e 0x70 -i 0x0 -n 1 -d 0x8F\n\n");

    printf("Main operation mode:\n");
    printf("  -r, --read         read N bytes from a device and register address\n");
    printf("  -w, --write        write N bytes from a device and register address\n");
    printf("  -a, --sda          GPIO pin to use for the SDA line (BCM numbering)\n");
    printf("  -c, --scl          GPIO pin to use for the SCL line (BCM numbering)\n");
    printf("  -g, --speed-grade  I2C bus speed grade (bit rate) as defined by pi_i2c.h\n");
    printf("                     valid speed grades are i2c_standard_mode (100) or i2c_full_speed (400)\n");
    printf("  -e, --device       device I2C address as a hex number (e.g., 0xFF)\n");
    printf("  -i, --register     device's register address read from or written as a hex number (e.g., 0xFF)\n");
    printf("  -s, --scan         scan the bus for any I2C devices\n");
    printf("  -n, --n-bytes      number of bytes to read or write to the device's register address\n");
    printf("  -d, --data         data to write to the device's register address in a comma delimited list\n");
    printf("                     data type is hex (e.g., 0xFF)\n");
    printf("  -v, --debug        display useful information\n");
    printf("  -h, --help         display this help and exit\n\n");

    printf("User's notes:\n");
    printf("  - If using the --scan option, device, register, n_bytes, data, " \
           "read, and write options are silently ignored\n\n");

    printf("Need any help? find a bug? Checkout https://github.com/besp9510/pi_i2c\n");
}