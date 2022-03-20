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
#include "pi_i2c.h"                   // Speed grade, macros, and outward
                                      // function prototypes.
#include "config.h"                   // I2C timing and variable defs
#include "detect_recover_bus.h"       // Detect and recover I2C bus
#include "clock_stretching.h"         // Support clock stretching
#include <pi_lw_gpio.h>               // GPIO library for the Pi
#include <pi_microsleep_hard.h>       // Hard microsleep function for the Pi

int read_byte_from_bus(int ack_flag) {
    // Definitions:
    int i;
    int byte = 0;

    int sda_level;

    // Release SDA line for the device to use:
    gpio_set_mode(GPIO_INPUT, sda_gpio_pin);

    // Read byte from bus starting at MSB:
    for (i = 7; i >= 0; i--) {
        // Set SCL line to read bit from bus:
        gpio_set_mode(GPIO_INPUT, scl_gpio_pin);

        // Adhere to UM10204 I2C-bus specification 3.1.9:
        support_clock_stretching();

        // Keep SCL set while SCL high period time elapses. Not waiting may
        // violate I2C timing requirements.
        microsleep_hard(scl_t_high_sleep_us);

        // Here we can read bit from the bus:
        sda_level = gpio_read_level(sda_gpio_pin);

        // Save bit by OR'ing onto byte read so far:
        byte = byte | (sda_level << i);

        // End clock pulse by clearing SCL:
        gpio_set_mode(GPIO_OUTPUT, scl_gpio_pin);

        // Keep SCL cleared while SCL low period time elapses. Not waiting may
        // violate I2C timing requirements.
        microsleep_hard(scl_t_low_sleep_us);
    }

    // If the SDA line has not yet been released then we assume that
    // the device is unresponsive and now need to recover the bus:
    if (!(gpio_read_level(sda_gpio_pin))) {
        return -EDEVICEHUNG;
    }

    // ACK if flag is set so more data can be from the bus; otherwise,
    // NACK to tell the device that we are done reading and wrap it up:
    if (ack_flag) {
        // ACK by clearing SDA line:
        gpio_set_mode(GPIO_OUTPUT, sda_gpio_pin);
    }

    // ACK or NACK by setting SCL line:
    gpio_set_mode(GPIO_INPUT, scl_gpio_pin);

    // Adhere to UM10204 I2C-bus specification 3.1.9:
    support_clock_stretching();

    // Keep SCL set while SCL high period time elapses. Not waiting may
    // violate I2C timing requirements.
    microsleep_hard(scl_t_high_sleep_us);

    // End clock pulse by clearing SCL:
    gpio_set_mode(GPIO_OUTPUT, scl_gpio_pin);

    // Keep SCL cleared while SCL low period time elapses. Not waiting may
    // violate I2C timing requirements.
    microsleep_hard(scl_t_low_sleep_us);

    // If we have NACK'd, now clear SDA line so that we can generate a STOP
    // condition:
    if (!(ack_flag)) {
        // Clear SDA by reclaiming the SDA line:
        gpio_set_mode(GPIO_OUTPUT, sda_gpio_pin);
    }

    return byte;
}