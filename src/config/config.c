// Include C standard libraries:
#include <time.h>  // C Standard get and manipulate time library

// Include header files:
#include "pi_i2c.h"                   // Speed grade, macros, and outward
                                      // function prototypes.
#include "write_conditions_to_bus.h"  // I2C START and STOP function protos
#include "config.h"                   // I2C timing and variable defs
#include <pi_lw_gpio.h>               // GPIO library for the Pi
#include <pi_microsleep_hard.h>       // Hard microsleep function for the Pi

// I2C timing compliance nanosleep timespec definitions
int min_t_hdsta_sleep_us = CEILING(MIN_T_HDSTA * 1e6);

int min_t_susto_sleep_us = CEILING(MIN_T_SUSTO * 1e6);

int min_t_buf_sleep_us = CEILING(MIN_T_BUF * 1e6);

int scl_t_low_sleep_us = 0;
int scl_t_high_sleep_us = 0;

int scl_response_time_us = CEILING(SCL_RESPONSE_TIME);

// Config global variables defined and initialized to 0:
int config_i2c_flag = 0;

int sda_gpio_pin = 0;
int scl_gpio_pin = 0;

int scl_clock_frequency_hz = 0;
float scl_actual_clock_frequency_hz = 0;

struct pi_i2c_statistics statistics = {
    .num_start_cond = 0,
    .num_repeated_start_cond = 0,
    .num_stop_cond = 0,
    .num_bytes_written = 0,
    .num_bytes_read = 0,
    .num_nack = 0,
    .num_nack_rst = 0,
    .num_bad_reg = 0,
    .num_badxfr = 0,
    .num_bus_resets = 0,
    .num_unknown_bus_errors = 0,
    .num_bus_lockups = 0,
    .num_failed_start_cond = 0,
    .num_failed_stop_cond = 0,
    .num_slave_hung = 0,
    .num_clock_stretch = 0
};

// Configure pi_i2c
int config_i2c(int sda, int scl, int speed_grade) {
    // Definitions:
    int scl_clock_period_us;

    // Setup microsleep function to eliminate additional over head at first
    // sleep function call:
    setup_microsleep_hard();

    // Set data and clock GPIO pin mappings:
    sda_gpio_pin = sda;
    scl_gpio_pin = scl;

    // Get bus into known state by using STOP condition:
    write_stop_condition_to_bus();

    // Set clock frequency given input speed grade:
    scl_clock_frequency_hz = speed_grade; // (clock frequency in Hz = bps)
    scl_clock_period_us = CEILING((1.0 / scl_clock_frequency_hz) * 1e6);

    // Assign SCL low and high period sleep times unevenly. The time it takes
    // for a GPIO pin to change state is ignored until that time can be
    // measured or researched accurately.
    //
    // Uneven allocation of period reflects I2C timing mimimums for T_LOW and
    // T_HIGH. T_LOW minimum is larger than T_HIGH but the ratio between the
    // two periods changes with speed grades. See the table below.
    //
    // +-----------+--------------------+-----------------+------------------+
    // |           |        Clock       |      T_LOW      |      T_HIGH      |
    // |   Mode    +===========+========+========+========+=========+========+
    // |           | Frequency | Period |   Min  |  % of  |   Min   |  % of  |
    // |           |           |        |        | Period |         | Period |
    // +-----------+-----------+--------+--------+--------+---------+--------+
    // | Standard  |  100 KHz  |  10 us | 4.7 us |   47%  |  4.0 us |   40%  |
    // +-----------+-----------+--------+--------+--------+---------+--------+
    // | Full      |  400 KHz  | 2.5 us | 1.3 us |   52%  |  0.6 us |   24%  |
    // +-----------+-----------+--------+--------+--------+---------+--------+
    // | Fast      |  1000 KHz | 1.0 us | 0.5 us |   50%  | 0.26 us |   26%  |
    // +-----------+-----------+--------+--------+--------+---------+--------+
    //
    // +-----------+--------------------+
    // |           |  Pin Response Time |
    // |   Mode    | (Falling & Rising) |
    // |           +===========+========+
    // |           |  Reserved |  % of  |
    // |           |           | Period |
    // +-----------+-----------+--------+
    // | Standard  |   1.3 us  |   13%  |
    // +-----------+-----------+--------+
    // | Fast      |   0.6 us  |   24%  |
    // +-----------+-----------+--------+
    // | Fast Plus |  0.24 us  |   24%  |
    // +-----------+-----------+--------+
    //
    // Choosing 66.6% of period for T_LOW and 33.3% of period for T_HIGH as
    // these ratios will work for all speed grades. Rounding required so actual
    // frequency achieved is not guaranteed to equal input:
    scl_t_low_sleep_us = CEILING((2.0 / 3.0) * scl_clock_period_us);
    scl_t_high_sleep_us = CEILING((1.0 / 3.0) * scl_clock_period_us);

    scl_actual_clock_frequency_hz = (1.0 / \
        ((scl_t_low_sleep_us + scl_t_high_sleep_us) * 1e-6));

    // Set configuration flag to allow functionality:
    config_i2c_flag = 1;

    return 0;
}