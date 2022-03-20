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
#include <pi_lw_gpio.h>               // GPIO library for the Pi
#include <pi_microsleep_hard.h>       // Hard microsleep function for the Pi

// Support UM10204 I2C-bus specification 3.1.9 before breaking another device
int support_clock_stretching(void) {
    // Elapsed time in micro seconds:
    int clock_stretching_elapsed_us = 0;
    int clock_stretching_sleep_us = CLOCK_STRETCHING_TIMEOUT_US / 10;

    // Implement a wait to avoid a false positive SCL stuck low:
    microsleep_hard(scl_response_time_us);

    // Check if SCL line has actually gone high after it was released; if not,
    // device has requested clock stretching:
    if (!(gpio_read_level(scl_gpio_pin))) {
        // Keep track of statistics for any caller interested in those
        // kind of numbers:
        statistics.num_clock_stretch++;

        // Wait for SCL to go high within the timeout period; if it goes
        // high, then device is ready for controller to continue.
        while ((clock_stretching_elapsed_us < CLOCK_STRETCHING_TIMEOUT_US)) {
            // Wait for device to release SCL line:
            microsleep_hard(clock_stretching_sleep_us);

            // If SCL line has been released then controller can continue:
            if (gpio_read_level(scl_gpio_pin)) {
                return 0;
            }

            // Continue clock stretching:
            clock_stretching_elapsed_us += (clock_stretching_sleep_us);
        }

        // Device has not responded within the timeout!

        // Keep track of statistics for any caller interested in those
        // kind of numbers:
        statistics.num_clock_stretching_timeouts++;

        return -ECLKTIMEOUT;
    }

    return 0;
}