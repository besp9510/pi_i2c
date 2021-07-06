# This is the I2C library
import pi_i2c

import numpy as np


def test_scan_bus_i2c():
    print("Testing scan_bus_i2c()")

    address_book = pi_i2c.scan_bus_i2c()

    print("scan_bus_i2c() has returned an address book")
    print("Following address have been detected: ")

    for i in range(0, len(address_book)):
        if address_book[i]:
            print("0x%X" % i)

    print("Test complete")


def test_write_i2c_one_bytes(slave_address, register_address, data, n_bytes):
    print("Testing write_i2c()")

    print("slave_address = 0x%X" % slave_address)
    print("register_address = 0x%X" % register_address)
    print("data = 0x%X" % data)
    print("n_bytes = %d" % n_bytes)

    pi_i2c.write_i2c(slave_address, register_address, data, n_bytes)

    print("write_i2c() has returned")
    print("Test complete")


def test_write_i2c_multiple_bytes(slave_address, register_address, data, n_bytes):
    print("Testing write_i2c()")

    print("slave_address = 0x%X" % slave_address)
    print("register_address = 0x%X" % register_address)

    for i in range(0, len(data)):
        print("write data %d = 0x%X" % (i, data[i]))

    print("n_bytes = %d" % n_bytes)

    pi_i2c.write_i2c(slave_address, register_address, data, n_bytes)

    print("write_i2c() has returned")
    print("Test complete")


def test_read_i2c_one_byte(slave_address, register_address, n_bytes):
    print("Testing read_i2c()")

    print("slave_address = 0x%X" % slave_address)
    print("register_address = 0x%X" % register_address)
    print("n_bytes = %d" % n_bytes)

    read_data = pi_i2c.read_i2c(slave_address, register_address, n_bytes)

    print("read_i2c() has returned")

    print("byte read = 0x%X" % read_data)
    print("Test complete")


def test_read_i2c_multiple_bytes(slave_address, register_address, n_bytes):
    print("Testing read_i2c()")

    print("slave_address = 0x%X" % slave_address)
    print("register_address = 0x%X" % register_address)
    print("n_bytes = %d" % n_bytes)

    read_data = pi_i2c.read_i2c(slave_address, register_address, n_bytes)

    print("read_i2c() has returned")

    for i in range(0, len(read_data)):
        print("byte read %d = 0x%X" % (i, read_data[i]))
    print("Test complete")


def test_get_configs_i2c():
    print("Testing get_configs_i2c()")

    configs = pi_i2c.get_configs_i2c()

    print(configs)
    print("Test complete")


def test_get_statistics_i2c():
    print("Testing get_statistics_i2c()")

    statistics = pi_i2c.get_statistics_i2c()

    print(statistics)
    print("Test complete")


# Use the default I2C pins:
# Ensure that Raspian I2C interface is disabled via rasp-config otherwise
# risk unpredictable behavior!
sda_pin = 2  # UPDATE
scl_pin = 3  # UPDATE

speed_grade = pi_i2c.I2C_FULL_SPEED

# Addresses & data to use when testing write:
write_slave_address = 0x1C  # UPDATE
write_register_address = 0x21  # UPDATE
write_data_int = 0x0  # UPDATE
write_bytes_int = 1  # UPDATE
write_data_array = np.array([0x0, 0x0, 0x0], dtype=int)  # UPDATE
write_bytes_array = 3  # UPDATE

read_slave_address = 0x1C  # UPDATE
read_register_address = 0x0F  # UPDATE
read_bytes_int = 1  # UPDATE
read_bytes_array = 3  # UPDATE

print("Begin test_pi_i2c.py")
print("Configuring pi_i2c")
print("sda_pin = %d" % sda_pin)
print("sda_pin = %d" % scl_pin)
print("speed_grade = %d Hz" % speed_grade)

# Configure at standard mode:
pi_i2c.config_i2c(sda_pin, scl_pin, speed_grade)

# Return back useful numbers to know:
test_get_configs_i2c()

# Scan I2C bus and identify present slaves:
test_scan_bus_i2c()

# Test I2C write with one byte:
test_write_i2c_one_bytes(write_slave_address, write_register_address, write_data_int, write_bytes_int)

# Test I2C write with multiple bytes:
test_write_i2c_multiple_bytes(write_slave_address, write_register_address, write_data_array, write_bytes_array)

# Test I2C read with one byte:
test_read_i2c_one_byte(read_slave_address, read_register_address, read_bytes_int)

# Test I2C read with multiple bytes:
test_read_i2c_multiple_bytes(read_slave_address, read_register_address, read_bytes_array)

# Test get statistics following all of the test calls:
test_get_statistics_i2c()
