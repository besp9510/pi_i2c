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

import numpy as np

import ctypes
from ctypes import RTLD_GLOBAL

from .libpii2c_errno import libpii2c_errno_list
from .libpii2c_header import pi_i2c_statistics, pi_i2c_configs

# Required dependencies:
libpimicrosleephard = ctypes.CDLL("libpimicrosleephard.so", mode=RTLD_GLOBAL)
libpilwgpio = ctypes.CDLL("libpilwgpio.so", mode=RTLD_GLOBAL)

libpii2c = ctypes.CDLL("libpii2c.so", mode=RTLD_GLOBAL)

# Define argument types for automatic type checking:
libpii2c.config_i2c.argtypes = (ctypes.c_uint, ctypes.c_uint, ctypes.c_uint)
libpii2c.scan_bus_i2c.argtypes = (ctypes.POINTER(ctypes.c_int),)
libpii2c.write.argtypes = (ctypes.c_uint, ctypes.c_uint,
                           ctypes.POINTER(ctypes.c_int), ctypes.c_uint)
libpii2c.read.argtypes = (ctypes.c_uint, ctypes.c_uint,
                          ctypes.POINTER(ctypes.c_int), ctypes.c_uint)

# Return the structure from C by value (and not reference/pointer)
libpii2c.get_statistics_i2c.restype = pi_i2c_statistics
libpii2c.get_configs_i2c.restype = pi_i2c_configs


def check_errno(result):
    # Check returned errno value and raise exception if required:
    for err in libpii2c_errno_list:
        if -err["value"] == result:
            raise err["raise"](err["message"])


# Wrapper for config_i2c() function
def config_i2c(sda, scl, speed_grade):
    ''' Configure Pi I2C by defining SDA and SCL pins & speed grade'''

    # Data types are messaged to int as C is expecting int:
    errno = libpii2c.config_i2c(ctypes.c_uint(int(sda)), ctypes.c_uint(int(scl)),
                                ctypes.c_uint(int(speed_grade)))
    check_errno(errno)


# Wrapper for scan_bus_i2c()
def scan_bus_i2c():
    ''' Scan I2C bus for present devices and return an address book '''

    # C is expecting a pointer to an integer array:
    # (127 elements as only 7-bit addressing is supported)
    address_book = np.zeros((127,), dtype=int)
    address_book_pointer = address_book.ctypes.data_as(ctypes.POINTER(ctypes.c_int))

    errno = libpii2c.scan_bus_i2c(address_book_pointer)
    check_errno(errno)

    return address_book


# Wrapper for write_i2c()
def write_i2c(device_address, register_address, data, n_bytes):
    ''' Write variable number of bytes to an I2C device's register address'''

    if not isinstance(device_address, int):
        raise TypeError("Device address must be an int")

    if not isinstance(register_address, int):
        raise TypeError("Register address must be an int")

    if not isinstance(n_bytes, int):
        raise TypeError("Number of bytes must be an int")

    # Constrain input:
    # - Int if n_bytes == 1 else array
    # - Type numpy array (results in easy conversion to pointer to int array)
    # - Size equal to n_bytes
    if n_bytes == 1:
        if not isinstance(data, int):
            raise TypeError("Data input must be an int if number of bytes equals 1")

        # C is expecting a pointer so cast integer to pointer to integer:
        data = np.array([data], dtype=int)
        data_pointer = data.ctypes.data_as(ctypes.POINTER(ctypes.c_int))
    else:
        if not isinstance(data, (np.ndarray, np.generic)):
            raise TypeError("Input array must be of NumPy array type")

        if data.shape != (n_bytes,):
            raise TypeError("Input array must be of shape (n_bytes,0)")

        # C is expecting a pointer to an integer array:
        data = data.astype(int)
        data_pointer = data.ctypes.data_as(ctypes.POINTER(ctypes.c_int))

    errno = libpii2c.write_i2c(ctypes.c_uint(int(device_address)),
                               ctypes.c_uint(int(register_address)), data_pointer,
                               ctypes.c_uint(int(n_bytes)))
    check_errno(errno)


# Wrapper for scan_bus_i2c()
def read_i2c(device_address, register_address, n_bytes):
    ''' Read variable number of bytes from an I2C devices's register address'''

    if not isinstance(device_address, int):
        raise TypeError("Device address must be an int")

    if not isinstance(register_address, int):
        raise TypeError("Register address must be an int")

    if not isinstance(n_bytes, int):
        raise TypeError("Number of bytes must be an int")

    # C is expecting a pointer to an integer array:
    data = np.zeros((n_bytes,), dtype=int)
    data_pointer = data.ctypes.data_as(ctypes.POINTER(ctypes.c_int))

    errno = libpii2c.read_i2c(ctypes.c_uint(int(device_address)),
                              ctypes.c_uint(int(register_address)), data_pointer,
                              ctypes.c_uint(int(n_bytes)))
    check_errno(errno)

    if n_bytes == 1:
        return int(data[0])
    else:
        return data

def reset_i2c():
    '''Reset the I2C bus by issuing 9 clock pulses'''

    errno = libpii2c.reset_i2c()
    check_errno(errno)


def get_statistics_i2c():
    '''Return a dictionary of statistics recorded by Pi I2C'''

    statistics_struct = libpii2c.get_statistics_i2c()
    statistics_dict = dict((field, getattr(statistics_struct, field)) for field, _ in statistics_struct._fields_)

    return statistics_dict

def get_configs_i2c():
    '''Return a dictionary of internal configurations of Pi I2C'''

    configs_struct = libpii2c.get_configs_i2c()
    configs_dict = dict((field, getattr(configs_struct, field)) for field, _ in configs_struct._fields_)

    return configs_dict
