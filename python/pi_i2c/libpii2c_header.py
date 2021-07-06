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
                ('num_slave_hung', ctypes.c_int), ('num_clock_stretching_timeouts', ctypes.c_int),
                ('num_clock_stretch', ctypes.c_int)]


class pi_i2c_configs(ctypes.Structure):
    _fields_ = [('scl_t_low_sleep_us', ctypes.c_int), ('scl_t_high_sleep_us', ctypes.c_int),
                ('scl_actual_clock_frequency_hz', ctypes.c_float),
                ('min_t_hdsta_sleep_us', ctypes.c_int), ('min_t_susta_sleep_us', ctypes.c_int),
                ('min_t_susto_sleep_us', ctypes.c_int), ('min_t_buf_sleep_us', ctypes.c_int)]
