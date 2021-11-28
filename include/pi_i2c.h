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

// I2C speed grades define in bits/second:
#define I2C_STANDARD_MODE 100e3
#define I2C_FULL_SPEED 400e3

// Error numbers:
#define ENOPIVER 140    // Could not get PI board revision
#define ENACK 141       // Slave did not acknowledge slave address
#define EBADXFR 142     // Slave did not acknowledge during byte transfer
                        // (read or write)
#define EBADREGADDR 143 // Slave did not acknowledge register address
#define ECLKTIMEOUT 144 // Slave not responsive after clock stretch timeout
#define EI2CNOTCFG 145  // pi_i2c has not yet been configured
#define ENACKRST 146    // Slave did not respond after repeated start slave
                        // address (read)
#define EBUSLOCKUP 147  // Bus is locked: SDA and SCL lines are being held low
                        // by slave
#define EBUSUNKERR 148  // Bus is in an unexpected state following an unknown
                        // error
#define EFAILSTCOND 149 // Failed to write a START condition to the bus. Most
                        // likely cause is previously seen bus error occurring
                        // during STOP condition.
#define ESLAVEHUNG 150  // Slave forcing SDA line low

// Structure definitions:
struct pi_i2c_statistics {
    int num_start_cond;
    int num_repeated_start_cond;
    int num_stop_cond;
    int num_bytes_written;
    int num_bytes_read;
    int num_nack;
    int num_nack_rst;
    int num_bad_reg;
    int num_badxfr;
    int num_bus_resets;
    int num_unknown_bus_errors;
    int num_bus_lockups;
    int num_failed_start_cond;
    int num_failed_stop_cond;
    int num_slave_hung;
    int num_clock_stretching_timeouts;
    int num_clock_stretch;
};

struct pi_i2c_configs {
    int scl_t_low_sleep_us;
    int scl_t_high_sleep_us;
    float scl_actual_clock_frequency_hz;
    int min_t_hdsta_sleep_us;
    int min_t_susta_sleep_us;
    int min_t_susto_sleep_us;
    int min_t_buf_sleep_us;
};

// I2C function prototypes:
int config_i2c(unsigned int sda, unsigned int scl, unsigned int speed_grade);
int scan_bus_i2c(int *address_book);
int write_i2c(unsigned int slave_address, unsigned int register_address,
              int *data, unsigned int n_bytes);
int read_i2c(unsigned int slave_address, unsigned int register_address,
             int *data, unsigned int n_bytes);
int reset_i2c(void);
struct pi_i2c_statistics get_statistics_i2c(void);
struct pi_i2c_configs get_configs_i2c(void);