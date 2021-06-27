// Include C standard libraries:
#include <time.h>  // C Standard get and manipulate time library

// Include header files:
#include "pi_i2c.h"                   // Speed grade, macros, and outward
                                      // function prototypes.
#include "config.h"                   // I2C timing and variable defs
#include "detect_recover_bus.h"       // Detect and recover I2C bus
#include <pi_lw_gpio.h>               // GPIO library for the Pi
#include <pi_microsleep_hard.h>       // Hard microsleep function for the Pi

// Write I2C START condition to bus (bus busy)
int write_start_condition_to_bus(void) {
    // Return immediately if bus is not IDLE:
    if (!(gpio_read_level(sda_gpio_pin)) && !(gpio_read_level(scl_gpio_pin))) {
        return 0;
    }

    // Clear SDA first to initiate START:
    gpio_set_mode(GPIO_OUTPUT, sda_gpio_pin);

    // Ensure that output mode means that the GPIO is cleared:
    gpio_clear(sda_gpio_pin);

    // Wait setup time required for START condition condition otherwise risk
    // slaves not understanding:
    microsleep_hard(min_t_hdsta_sleep_us);

    // Set SDA to complete STOP:
    // (Bus is now busy)
    gpio_set_mode(GPIO_OUTPUT, scl_gpio_pin);

    // Ensure that output mode means that the GPIO is cleared:
    gpio_clear(scl_gpio_pin);

    // Must elapse SCL low period before allowing another function
    // to use the bus:
    microsleep_hard(scl_t_low_sleep_us);

    // Check if START condition was actually written to the bus:
    if (gpio_read_level(sda_gpio_pin) && gpio_read_level(scl_gpio_pin)) {
        // Keep track of statistics for any caller interested in those
        // kind of numbers:
        statistics.num_failed_start_cond++;

        return -EFAILSTCOND;
    }

    // Keep track of statistics for any caller interested in those
    // kind of numbers:
    statistics.num_start_cond++;

    return 0;
}

// Write STOP condition to bus (bus idle)
int write_stop_condition_to_bus(void) {
    int ret;

    // Return immediately if bus is already IDLE:
    if (gpio_read_level(sda_gpio_pin) && gpio_read_level(scl_gpio_pin)) {
        return 0;
    }

    // Begin STOP condition by setting SCL:
    gpio_set_mode(GPIO_INPUT, scl_gpio_pin);

    // Wait setup time required for STOP condition otherwise
    // risk slaves not understanding:
    microsleep_hard(min_t_susto_sleep_us);

    // Set SDA to complete STOP condition:
    // (Bus is now idle)
    gpio_set_mode(GPIO_INPUT, sda_gpio_pin);

    // Wait minimum time before a new transmission can start in case another
    // I2C message queued:
    microsleep_hard(min_t_buf_sleep_us);

    // Detect if bus is not IDLE and attempt to recover the bus:
    if ((ret = detect_recover_bus()) < 0) {
        // Keep track of statistics for any caller interested in those
        // kind of numbers:
        statistics.num_failed_stop_cond++;

        return ret;
    }

    // Keep track of statistics for any caller interested in those
    // kind of numbers:
    statistics.num_stop_cond++;

    return 0;
}

int write_repeated_start_condition_to_bus(void) {
    // Return immeadiately if SDA or SCL lines are not cleared:
    if (gpio_read_level(sda_gpio_pin) || gpio_read_level(scl_gpio_pin)) {
        return -1;
    }

    // Set SDA line first as to not produce a STOP condition accidentally:
    gpio_set_mode(GPIO_INPUT, sda_gpio_pin);

    // Set SCL line next; we will now be in a state where a START
    // condition can be written to the bus:
    gpio_set_mode(GPIO_INPUT, scl_gpio_pin);

    // Wait setup time required for repeated START condition:
    microsleep_hard(min_t_susta_sleep_us);

    // Ready for repeated start condition:
    write_start_condition_to_bus();

    // Keep track of statistics for any caller interested in those
    // kind of numbers:
    statistics.num_repeated_start_cond++;

    return 0;
}