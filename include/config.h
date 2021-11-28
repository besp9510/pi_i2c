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

// I2C timings
#define MIN_T_LOW 1.3e-6   // SCL Low Period [seconds]
#define MIN_T_HIGH 0.6e-6  // SCL High Period [seconds]
#define MIN_T_SUDAT 0.1e-6 // SDA Setup Time [seconds]
#define MIN_T_HDDAT 0      // SDA Hold Time [seconds]
#define MIN_T_SUSTA 0.6e-6 // Setup Time for a repeated Start condition [seconds]
#define MIN_T_HDSTA 0.6e-6 // Hold Time for a Start condition [seconds]
#define MIN_T_SUSTO 0.6e-6 // Setup time for a Stop condition [seconds]
#define MIN_T_BUF 1.3e-6   // Time before a new transmission can start [seconds]

#define SCL_RESPONSE_TIME 1e-6 // Time for SCL to change after set/clear

#define CLOCK_STRETCHING_TIMEOUT_US 500e3 // Clock stretching timeout [micro seconds]

#define ACK 0  // Slave ACK
#define NACK 1 // Slave NACK

#define WRITE_FLAG 0 // Master write
#define READ_FLAG 1  // Master read

#define CONTINUE_FLAG 0 // No STOP condition at end of write
#define STOP_FLAG 1     // STOP condition at end of write

// Some useful functions
#define CEILING(n) (((n - (int)(n)) != 0) ? ((int)(n) + 1) : ((int)(n)))

// Global variables:
extern int sda_gpio_pin; // Data line
extern int scl_gpio_pin; // Clock line

extern int scl_clock_frequency_hz;          // Desired clock frequency
extern float scl_actual_clock_frequency_hz; // Actual clock frequency

extern int config_i2c_flag; // I2C lines and timings defined?

extern struct pi_i2c_statistics statistics;

// I2C timing compliance:
extern int min_t_hdsta_sleep_us;      // Hold time for START condition
extern int min_t_susto_sleep_us;      // Setup time for STOP condition
extern int min_t_susta_sleep_us;      // Setup time for repeated START condition
extern int min_t_buf_sleep_us;        // Time before new transmission
extern int scl_t_low_sleep_us;        // SCL Low Period
extern int scl_t_high_sleep_us;       // SCL High Period
extern int scl_response_time_us;      // Time for SCL to change