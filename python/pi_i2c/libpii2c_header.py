# Inter-Integrated Circuit (I2C) Library for the Raspberry Pi
#
# Copyright (c) 2021 Benjamin Spencer
# =============================================================================
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
# =============================================================================

import ctypes

# I2C speed grades define in bits/second:
I2C_STANDARD_MODE = 100e3
I2C_FULL_SPEED = 400e3


# Structure definitions
class pi_i2c_statistics(ctypes.Structure):
    _fields_ = [('num_start_cond', ctypes.c_int), ('num_repeated_start_cond', ctypes.c_int),
                ('num_stop_cond', ctypes.c_int), ('num_bytes_written', ctypes.c_int),
                ('num_bytes_read', ctypes.c_int), ('num_nack', ctypes.c_int),
                ('num_nack_rst', ctypes.c_int), ('num_bad_reg', ctypes.c_int),
                ('num_badxfr', ctypes.c_int), ('num_bus_resets', ctypes.c_int),
                ('num_unknown_bus_errors', ctypes.c_int), ('num_bus_lockups', ctypes.c_int),
                ('num_failed_start_cond', ctypes.c_int), ('num_failed_stop_cond', ctypes.c_int),
                ('num_device_hung', ctypes.c_int), ('num_clock_stretching_timeouts', ctypes.c_int),
                ('num_clock_stretch', ctypes.c_int)]


class pi_i2c_configs(ctypes.Structure):
    _fields_ = [('scl_t_low_sleep_us', ctypes.c_int), ('scl_t_high_sleep_us', ctypes.c_int),
                ('scl_actual_clock_frequency_hz', ctypes.c_float),
                ('min_t_hdsta_sleep_us', ctypes.c_int), ('min_t_susta_sleep_us', ctypes.c_int),
                ('min_t_susto_sleep_us', ctypes.c_int), ('min_t_buf_sleep_us', ctypes.c_int)]
