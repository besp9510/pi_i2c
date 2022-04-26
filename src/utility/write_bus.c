// Inter-Integrated Circuit (I2C) Library for the Raspberry Pi
//
// Copyright (c) 2021 Benjamin Spencer
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
#include <time.h>  // C Standard get and manipulate time library

// Include header files:
#include "config.h"                   // I2C timing and variable defs
#include "detect_recover_bus.h"       // Detect and recover I2C bus
#include "clock_stretching.h"         // Support clock stretching
#include <pi_lw_gpio.h>               // GPIO library for the Pi
#include <pi_microsleep_hard.h>       // Hard microsleep function for the Pi

int write_byte_to_bus(int byte) {
    // Definitions:
    int i;
    int bit;

    int sda_level;

    // Loop through byte starting at MSB:
    for (i = 7; i >= 0; i--) {
        // Recover value of bit to either set or clear SDA line:
        //
        // Example: Recover value of bit 5
        //   x x x n x x x x
        // & 0 0 0 1 0 0 0 0       (1 shift left 5 times ir 0x1 << 5)
        // = 0 0 0 n 0 0 0 0
        //   0 0 0 n 0 0 0 0 >> 5
        // = 0 0 0 0 0 0 0 n       (n shifted right 5 times)
        bit = (byte & (0x1 << i)) >> i;

        // Immediately change SDA line if required. Choosing to change SDA
        // right after new clock pulse out of convience.
        if (bit) {
            gpio_set_mode(GPIO_INPUT, sda_gpio_pin);
        } else {
            gpio_set_mode(GPIO_OUTPUT, sda_gpio_pin);
        }

        // Keep SCL cleared while SCL low period time elapses. Not waiting may
        // violate I2C timing requirements.
        microsleep_hard(scl_t_low_sleep_us);

        // Transmit bit by setting SCL line:
        gpio_set_mode(GPIO_INPUT, scl_gpio_pin);

        // Adhere to UM10204 I2C-bus specification 3.1.9:
        support_clock_stretching();

        // Keep SCL set while SCL high period time elapses. Not waiting may
        // violate I2C timing requirements.
        microsleep_hard(scl_t_high_sleep_us);

        // End clock pulse by clearing SCL:
        gpio_set_mode(GPIO_OUTPUT, scl_gpio_pin);
    }

    // Release SDA line so that device can ACK or NACK data transfer:
    gpio_set_mode(GPIO_INPUT, sda_gpio_pin);

    // Previously ended a clock cycle so we must elapse SCL low period:
    microsleep_hard(scl_t_low_sleep_us);

    // Device will have ACK'd by now; let's set SCL to read off pin:
    gpio_set_mode(GPIO_INPUT, scl_gpio_pin);

    // Adhere to UM10204 I2C-bus specification 3.1.9:
    support_clock_stretching();

    // Determine if device ACK'd data transfer by reading pin value
    //     ACK = 1: NACK
    //     ACK = 0: ACK
    sda_level = gpio_read_level(sda_gpio_pin);

    microsleep_hard(scl_t_high_sleep_us);

    // End clock pulse by clearing SCL:
    gpio_set_mode(GPIO_OUTPUT, scl_gpio_pin);

    // Reclaim SDA line as device is done using it:
    gpio_set_mode(GPIO_OUTPUT, sda_gpio_pin);

    return sda_level;
}

int write_address_frame_to_bus(int device_address, int write_flag) {
    // Definitions:
    int device_address_write_flag_byte;
    int write_status;

    // Some magic to allow the write_byte function to write both the 7-bit
    // address and RW flag in one go.
    //
    // Example:
    //    0 X X X X X X X
    // || F 0 0 0 0 0 0 0 (RW Flag << 8)
    //  = F X X X X X X X
    device_address_write_flag_byte = (device_address << 1) | (write_flag);

    // Begin message by addressing device at input address for read/write:
    write_status = write_byte_to_bus(device_address_write_flag_byte);

    return write_status;
}

int write_data_frame_to_bus(int data) {
    // Definitions:
    int write_status;

    // Begin message by addressing device at input address for read/write:
    write_status = write_byte_to_bus(data);

    return write_status;
}