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
//
// I2C Protocol
//
// Start: Data transmission on the bus begins with a high to low transition on
//        the SDA line while SCL is held high (start condition (S)
//        indicated by I²C bus controller). Once the START signal is issued
//        by the controller, the bus is considered busy.
//
//        SDA
//        ________
//                |_________
//               Clear
//
//        SCL
//        _____________
//                     |______
//                   Clear
//
// Stop: Each data transfer should be terminated by a Stop signal (P)
//       generated by the controller. The STOP condition is a low to
//       HIGH transition on SDA line while SCL is held high.
//
//       SDA
//                  ___________
//       __________|
//                Set
//
//       SCL
//          ___________________
//       __|
//        Set
//
// ACK: Each byte of data transferred must be acknowledged. It is indicated
//      by an acknowledge bit sent by the receiver. The transmitter must
//      release the SDA line (no pull down) during the acknowledge pulse while
//      the receiver must then pull the SDA line low so that it remains
//      stable low during the high period of the acknowledge clock cycle.
//
// IDLE: SDA and SCL set
//
// I2C message format
//
// +------------------------------------------------------------------------+
// |                                 Message                                |
// +=======+========+======+======+=========+======+=========+======+=======+
// | Start | 7 Bits |  RW  |  ACK |  8 Bits |  ACK |  8 Bits |  ACK |  Stop |
// | Cond. |        | Flag | NACK |         | NACK |         | NACK | Cond. |
// +-------+--------+------+------+---------+------+---------+------+-------+
// |       |  Addr. |      |      |   Data  |      |   Data  |      |       |
// |       |  Frame |      |      | Frame 1 |      | Frame 2 |      |       |
// +-------+--------+------+------+---------+------+---------+------+-------+

// Include C standard libraries:
#include <time.h>  // C Standard get and manipulate time library
#include <errno.h> // C Standard for error conditions

// Include header files:
#include "pi_i2c.h"                   // Speed grade, macros, and outward
                                      // function prototypes.
#include "write_conditions_to_bus.h"  // I2C START and STOP function protos
#include "write_bus.h"                // I2C write to bus functions
#include "read_bus.h"                 // I2C read to bus functions
#include "config.h"                   // I2C timing and variable defs
#include "detect_recover_bus.h"       // Detect and recover I2C bus
#include "clock_stretching.h"         // Support clock stretching
#include <pi_lw_gpio.h>               // GPIO library for the Pi
#include <pi_microsleep_hard.h>       // Hard microsleep function for the Pi

// Read N number of bytes from the specified register address of a device
int read_i2c(unsigned int device_address, unsigned int register_address,
             int *data, unsigned int n_bytes) {
    // Definitions:
    int byte;
    int i;
    int ret;

    int write_status;

    int ack_flag = 1;

    // Check if I2C has been configured for use; otherwise bail as important
    // timings are not yet defined:
    if (!config_i2c_flag) {
        return -EI2CNOTCFG;
    }

    // Only 7-bit addressing is supported:
    if (device_address > 0x7F) {
        return -EINVAL;
    }

    // Register address cannot be greater than 0xFF (11111111)
    // (i.e. can only be an 8-bit number):
    if (register_address > 0xFF) {
        return -EINVAL;
    }

    // Zero makes no sense caller:
    if (n_bytes == 0) {
        return -EINVAL;
    }

    // Get bus into known state by using STOP condition:
    if ((ret = write_stop_condition_to_bus()) < 0) {
        return ret;
    }

    // Make bus busy with START condition so devices know to expect message:
    if ((ret = write_start_condition_to_bus()) < 0) {
        return ret;
    }

    // Write address frame to bus and begin message with device:
    write_status = write_address_frame_to_bus(device_address, WRITE_FLAG);

    if (write_status == NACK) {
        // In case a STOP condition cannot be written and bus
        // encounters an error
        if ((ret = write_stop_condition_to_bus()) < 0) {
            return ret;
        }
        // Keep track of statistics for any caller interested in those
        // kind of numbers:
        statistics.num_nack++;

        return -ENACK;
    }

    // Write register address to device:
    write_status = write_data_frame_to_bus(register_address);

    if (write_status == NACK) {
        // In case a STOP condition cannot be written and bus
        // encounters an error
        if ((ret = write_stop_condition_to_bus()) < 0) {
            return ret;
        }
        // Keep track of statistics for any caller interested in those
        // kind of numbers:
        statistics.num_bad_reg++;

        return -EBADREGADDR;
    }

    // A repeated start condition is required prior to reading off data:
    if ((ret = write_repeated_start_condition_to_bus() < 0)) {
        return ret;
    }

    // Write address frame to bus and begin message with the device:
    write_status = write_address_frame_to_bus(device_address, READ_FLAG);

    if (write_status == NACK) {
        // In case a STOP condition cannot be written and bus
        // encounters an error
        if ((ret = write_stop_condition_to_bus()) < 0) {
            return ret;
        }
        // Keep track of statistics for any caller interested in those
        // kind of numbers:
        statistics.num_nack_rst++;

        return -ENACKRST;
    }

    // Read data from the specified register:
    for (i = 0; i < n_bytes; i++) {
        // Only NACK if it is the last byte to be read:
        if (i == (n_bytes - 1)) {
            ack_flag = 0;
        }

        // Read one byte from the device. ACK or NACK depending on if
        // more bytes will be subsequently read:
        byte = read_byte_from_bus(ack_flag);

        // Consider a NACK during data transfer to be a bad transfer; device
        // stopped responding to write for some reason:
        if (byte == NACK) {
            return -EBADXFR;
        }
        // Keep track of statistics for any caller interested in those
        // kind of numbers:
        statistics.num_bytes_read++;

        data[i] = byte;
    }

    // Complete message by transition the bus to IDLE:
    if ((ret = write_stop_condition_to_bus()) < 0) {
        return ret;
    }

    return 0;
}

// Write N number of bytes to the specified register address of a device
int write_i2c(unsigned int device_address, unsigned int register_address,
              int *data, unsigned int n_bytes) {
    // Definitions:
    int write_status;
    int i;
    int ret;

    // Check if I2C has been configured for use; otherwise bail as important
    // timings are not yet defined:
    if (!config_i2c_flag) {
        return -EI2CNOTCFG;
    }

    // Only 7-bit addressing is supported:
    if (device_address > 0x7F) {
        return -EINVAL;
    }

    // Register address cannot be greater than 0xFF (11111111)
    // (i.e. can only be an 8-bit number):
    if (register_address > 0xFF) {
        return -EINVAL;
    }

    // Zero makes no sense caller:
    if (n_bytes == 0) {
        return -EINVAL;
    }

    // Get bus into known state by using STOP condition:
    if ((ret = write_stop_condition_to_bus()) < 0) {
        return ret;
    }

    // Make bus busy with START condition so devices know to expect message:
    if ((ret = write_start_condition_to_bus()) < 0) {
        return ret;
    }

    // Write address frame to bus and begin message with the device:
    write_status = write_address_frame_to_bus(device_address, WRITE_FLAG);

    if (write_status == NACK) {
        // In case a STOP condition cannot be written and bus
        // encounters an error
        if ((ret = write_stop_condition_to_bus()) < 0) {
            return ret;
        }
        // Keep track of statistics for any caller interested in those
        // kind of numbers:
        statistics.num_nack++;

        return -ENACK;
    }

    // Write register address to the device:
    write_status = write_data_frame_to_bus(register_address);

    if (write_status == NACK) {
        // In case a STOP condition cannot be written and bus
        // encounters an error
        if ((ret = write_stop_condition_to_bus()) < 0) {
            return ret;
        }
        // Keep track of statistics for any caller interested in those
        // kind of numbers:
        statistics.num_bad_reg++;

        return -EBADREGADDR;
    }

    // Write data to specified register:
    for (i = 0; i < n_bytes; i++) {
        write_status = write_data_frame_to_bus(data[i]);

        // Consider a NACK during data transfer to be a bad transfer; device
        // stopped responding to write for some reason:
        if (write_status == NACK) {
            // In case a STOP condition cannot be written and bus
            // encounters an error
            if ((ret = write_stop_condition_to_bus()) < 0) {
                return ret;
            }
            // Keep track of statistics for any caller interested in those
            // kind of numbers:
            statistics.num_badxfr++;

            return -EBADXFR;
        }
        // Keep track of statistics for any caller interested in those
        // kind of numbers:
        statistics.num_bytes_written++;
    }

    // Complete message by transition the bus to IDLE:
    if ((ret = write_stop_condition_to_bus()) < 0) {
        return ret;
    }

    return write_status;
}

// Scan bus for devices (only supporting 7-bit addressing)
int scan_bus_i2c(int *address_book) {
    // Definitions:
    int i;
    int ret;

    int write_status;

    // Check if I2C has been configured for use; otherwise bail as important
    // timings are not yet defined:
    if (!config_i2c_flag) {
        return -EI2CNOTCFG;
    }

    // Initialize array to zero to prevent any confusion for caller:
    for (i = 0; i < 127; i++) {
        address_book[i] = 0x0;
    }

    // Get bus into known state by using STOP condition:
    if ((ret = write_stop_condition_to_bus()) < 0) {
        return ret;
    }

    for (i = 0; i < 128; i++) {
        // Make bus busy with START condition so devices know to expect message:
        if ((ret = write_start_condition_to_bus()) < 0) {
            return ret;
        }

        // Index will be I2C address to scan:
        write_status = write_address_frame_to_bus(i, WRITE_FLAG);

        // Transition bus back to IDLE in case the device has ACK'd during scan:
        if ((ret = write_stop_condition_to_bus()) < 0) {
            return ret;
        }

        // If device responded, update i2c address book to say if a
        // device was detected:
        if (write_status == ACK) {
            address_book[i] = 0x1;
        }
    }

    return 0;
}

// Reset bus by issuing 9 clock pulses. Typically used to un-stuck the SDA line
// after a device is forcing it low
int reset_i2c(void) {
    int i;
    int ret;

    // Check if I2C has been configured for use; otherwise bail as important
    // timings are not yet defined:
    if (!config_i2c_flag) {
        return -EI2CNOTCFG;
    }

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
    }
    // Keep track of statistics for any caller interested in those
    // kind of numbers:
    statistics.num_bus_resets++;

    return 0;
}

// Return a structure of statistics recorded by Pi I2C
struct pi_i2c_statistics get_statistics_i2c(void) {
    return statistics;
}

// Return internal configuration values
struct pi_i2c_configs get_configs_i2c(void) {
    struct pi_i2c_configs configs = {
        .scl_t_low_sleep_us = scl_t_low_sleep_us,
        .scl_t_high_sleep_us = scl_t_high_sleep_us,
        .scl_actual_clock_frequency_hz = scl_actual_clock_frequency_hz,
        .min_t_hdsta_sleep_us = min_t_hdsta_sleep_us,
        .min_t_susta_sleep_us = min_t_susta_sleep_us,
        .min_t_susto_sleep_us = min_t_susto_sleep_us,
        .min_t_buf_sleep_us = min_t_buf_sleep_us
    };

    return configs;
}
