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
#include "write_conditions_to_bus.h"  // I2C START and STOP function protos
#include "config.h"                   // I2C timing and variable defs
#include "clock_stretching.h"         // Support clock stretching
#include <pi_lw_gpio.h>               // GPIO library for the Pi
#include <pi_microsleep_hard.h>       // Hard microsleep function for the Pi

// Detect if bus is locked up **assuming the expected condition is IDLE**
// and attempt to recover depending on the error
int detect_recover_bus(void) {
    int i;
    int ret;

    // Exit if in IDLE as that is the expected condition:
    if (gpio_read_level(sda_gpio_pin) && gpio_read_level(scl_gpio_pin)) {
        return 0;
    }

    // Detect if only SDA line is held low which indicates controller and device
    // are out of sync for some reason. Resolution is to issue 9 clock cycles
    // and check if SDA line is released.
    if (!(gpio_read_level(sda_gpio_pin)) && gpio_read_level(scl_gpio_pin)) {
        for (i = 0; i < 9; i++) {
            // End clock pulse by clearing SCL:
            gpio_set_mode(GPIO_OUTPUT, scl_gpio_pin);

            // Previously ended a clock cycle so we must elapse SCL low period:
            microsleep_hard(scl_t_low_sleep_us);

            // Transmit bit by setting SCL line:
            gpio_set_mode(GPIO_INPUT, scl_gpio_pin);

            // Keep SCL set while SCL high period time elapses. Not waiting may
            // violate I2C timing requirements.
            microsleep_hard(scl_t_high_sleep_us);

            // Adhere to UM10204 I2C-bus specification 3.1.9:
            if ((ret = support_clock_stretching()) < 0) {
                // In the case clock stretching ends in a time out, immeadiately
                // exit as device needs to be power cycled:
                return ret;
            }

            // Check that SDA lines has been released by the device:
            if (gpio_read_level(sda_gpio_pin)) {
                // Keep track of statistics for any caller interested in those
                // kind of numbers:
                statistics.num_bus_resets++;

                return 0;
            }
        }

        // SDA line still not released by the device. Power cycle if possible!

        // Keep track of statistics for any caller interested in those
        // kind of numbers:
        statistics.num_device_hung++;

        return -EDEVICEHUNG;
    }

    // Detect if only SCL line is held low which indicates the device has most
    // likely become unresponsive. Resolution is to power cycle device if
    // possible!
    if (gpio_read_level(sda_gpio_pin) && !(gpio_read_level(scl_gpio_pin))) {
        // Keep track of statistics for any caller interested in those
        // kind of numbers:
        statistics.num_clock_stretching_timeouts++;

        return -ECLKTIMEOUT;
    }

    // Detect if SDA and SCL lines are held low by the device which indicates
    // that the bus is completely locked up. Resolution is power cycle the
    // device if possible!
    if (!(gpio_read_level(sda_gpio_pin)) && !(gpio_read_level(scl_gpio_pin))) {
        // Keep track of statistics for any caller interested in those
        // kind of numbers:
        statistics.num_bus_lockups++;

        return -EBUSLOCKUP;
    }

    // Have reached an unkown error if you have reached this point!

    // Keep track of statistics for any caller interested in those
    // kind of numbers:
    statistics.num_unknown_bus_errors++;

    return -EBUSUNKERR;
}