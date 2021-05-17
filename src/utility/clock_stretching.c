// Include C standard libraries:
#include <time.h>  // C Standard get and manipulate time library

// Include header files:
#include "pi_i2c.h"                   // Speed grade, macros, and outward
                                      // function prototypes.
#include "config.h"                   // I2C timing and variable defs
#include "detect_recover_bus.h"       // Detect and recover I2C bus
#include <pi_lw_gpio.h>               // GPIO library for the Pi

// Support UM10204 I2C-bus specification 3.1.9 before breaking another device
int support_clock_stretching(void) {
    // Elapsed time in micro seconds:
    int clock_stretching_elapsed_us = 0;

    // Implement a wait to avoid a false positive SCL stuck low:
    nanosleep(&scl_response_time, NULL);

    // Check if SCL line has actually gone high after it was released; if not,
    // slave has requested clock stretching:
    if (!(gpio_read_level(scl_gpio_pin))) {
        // Keep track of statistics for any caller interested in those
        // kind of numbers:
        statistics.num_clock_stretch++;

        // Wait for SCL to go high within the timeout period; if it goes
        // high, then slave is ready for master to continue.
        while ((clock_stretching_elapsed_us < CLOCK_STRETCHING_TIMEOUT_US)) {
            // Wait for slave to release SCL line:
            nanosleep(&clock_stretching_sleep, NULL);

            // If SCL line has been released then master can continue:
            if (gpio_read_level(scl_gpio_pin)) {
                return 0;
            }

            // Continue clock stretching:
            clock_stretching_elapsed_us += (clock_stretching_sleep.tv_nsec
                                            / 1e3);
        }

        // Slave has not responded within the timeout!

        // Keep track of statistics for any caller interested in those
        // kind of numbers:
        statistics.num_clock_stretching_timeouts++;

        return -ECLKTIMEOUT;
    }

    return 0;
}