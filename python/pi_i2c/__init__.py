'''Comprehensive I2C library for the Raspberry Pi [Now in Python]'''

from .libpii2c import config_i2c, scan_bus_i2c, write_i2c, read_i2c, reset_i2c, get_statistics_i2c, get_configs_i2c
from .libpii2c_header import I2C_STANDARD_MODE, I2C_FULL_SPEED
