# Inter-Integrated Circuit (I2C) Library for the Raspberry Pi

pi_i2c.c provides a comprehensive I2C library for the Raspberry Pi. Unlike the Pi's built-in I2C controller, any number of GPIOs can be defined as the SCL and SDA line thus allowing flexible configuration and additional I2C busses to be defined. Additional features of this library include multi-byte read & write functions, scan function, clock stretching support, error handling, and useful statistics.

This library requires the following dependencies (projects also authored by me):
* [pi_lw_gpio.c](https://github.com/besp9510/pi_lw_gpio)
    * "Lightweight GPIO Interface Library for the Raspberry Pi"
* [pi_microsleep_hard.c](https://github.com/besp9510/pi_microsleep_hard)
    * "Hard Microsleep Library Via System Timer for the Raspberry Pi"

They are not required to compile pi_i2c.c into a shared library but are required to be linked when building your project.

![gpio](images/i2c.png)

pi_i2c.c is provided four ways for flexibility:
1. C source and header files that can be compiled along with your program
2. C shared library
3. Python package
    * Interface with pi_i2c.c using Python 3!
4. A Bash executable
    * Interface with pi_i2c.c using the CLI!

A test script is included in this repository to checkout the library working on your Pi. You can also checkout this [LIS3MDL example](https://github.com/besp9510/pi_lis3mdl_example) I wrote to test out the library on a specific device.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. These steps will configure, make, install, and provide instructions on uninstall for the C shared library, python package, and Bash executable.

### Installing

#### Obtain the Project

First, clone this repository.

```
$ git clone https://github.com/besp9510/pi_i2c.git
```

Alternatively, download the repository from Git.

#### Configure

Once obtaining a copy on your local machine, navigate to the top directory and run the configure script to generate a Makefile.

```
$ ./configure
```

By default, files will be installed under `/usr/local/`. Note that passing the option ``--help`` will display available configuration options such as installation directory prefix and debug symbols.

Next, navigate to the `cli` directory and run the configure script to generate a Makefile for the Bash executable.

```
$ cd cli
$ ./configure
```

Return to the top directory after doing so.

```
$ cd ../
```

#### Make

Compile pi_i2c.c into a shared library.

```
$ make
```

Then install files to the installation directory. You must run the following either as root or with root privileges.

```
$ sudo make install
```

Next, navigate to the `cli` directory to compile the Bash executable.

```
$ cd cli/
$ make
```

Then install files to the installation directory.

```
$ make install
```

To use pi_i2c.c in your project, simply include the header file `pi_i2c.h` and link to the shared library `-lpii2c`, `-lpilwgpio`, & `-lpimicrosleephard`. See the test script's `test_pi_i2c.c` Makefile under the `test/` directory as a example. 

**If using the Pi's default SDA & SCL pins (BCM pin 2 & 3), ensure that Raspian I2C interface is disabled via rasp-config or otherwise risk unpredictable behavior!**

#### Uninstall

At anytime, to uninstall pi_i2c.c, use the same Makefile used for compiling or a Makefile generated using the configuration script with the same options as root or with root privileges.

```
$ sudo make uninstall
```

## Running the Test

test_pi_i2c.c is a test script to check and see the I2C library working on your Pi with some I2C device. The outline of the test is:
1. Defining I2C address and registers to test with
2. Setting up pi_i2c()
3. Writing and reading 1 and multiple bytes one-shot and iteratively
4. Bit rate read and write tests
5. Testing get config and statistics functions

Prior to running the test, make sure to update the I2C address and registers for your specific device.

``` c
// Ensure that Raspian I2C interface is disabled via rasp-config otherwise
// risk unpredictable behavior!
int sda_pin = 2; // UPDATE
int scl_pin = 3; // UPDATE

int speed_grade = I2C_FULL_SPEED;

// Addresses & data to use when testing write:
int write_device_address = 0x1C;    // UPDATE
int write_register_address = 0x21;  // UPDATE
int write_data[1] = {0x00};         // UPDATE
int write_bytes = 1;                // UPDATE
int write_iterations = 10;

int write_device_address_multiple = 0x1C;   // UPDATE
int write_register_address_multiple = 0x23; // UPDATE
int write_data_multiple[2] = {0x0, 0x0};    // UPDATE
int write_bytes_multiple = 2;

int read_device_address = 0x1C;   // UPDATE
int read_register_address = 0x0F; // UPDATE
int read_bytes = 1;               // UPDATE
int read_iterations = 10;
int read_data[1];                 // UPDATE

int read_device_address_multiple = 0x1C;     // UPDATE
int read_register_address_multiple = 0x28;   // UPDATE
int read_bytes_multiple = 2;                 // UPDATE
int read_data_multiple[read_bytes_multiple]; // UPDATE
```

### Prerequisites

This test requires the following dependencies (projects also authored by me):
* [pi_lw_gpio.c](https://github.com/besp9510/pi_lw_gpio)
    * "Lightweight GPIO Interface Library for the Raspberry Pi"
* [pi_microsleep_hard.c](https://github.com/besp9510/pi_microsleep_hard)
    * "Hard Microsleep Library Via System Timer for the Raspberry Pi"

### Installing

To compile the test script, first navigate to the test directory `test/`. Next, run the configure script to generate the Makefile:

```
$ ./configure
```

By default, the pi_i2c.c shared library will be looked for under the standard directories (e.g. `/usr/local/`). If this is not the case, pass the option `--help` to learn how to specify a directory to be searched. Additionally, `--help` will also display available configuration options such as debug symbols and debug logs.

Next, compile the test script:

```
$ make
```

This will create an executable called `test_pi_i2c` under `bin/`.

## Documentation
pi_i2c.c implements I2C according to the [UM10204 I2C-bus specification and user manual](https://www.nxp.com/docs/en/user-guide/UM10204.pdf). Specifically, the following I2C bus protocol features are supported for **single controller configuration only**:
* START condition
* STOP condition
* Acknowledge
* Clock Stretching
* 7-bit device address
* Software reset
    * *See error handling for more details*
* Standard-mode and full-speed mode
* Single and multiple byte read and write functionality

Additionally, timing minimums are respected according to this table and diagram:

![gpio](images/min_timings.png)

![gpio](images/diagram.png)

### Bus Error Handling

pi_i2c.c performs bus error detection and bus recovery (if able) to safe guard against hardware damage. This is done at every step of a message transaction:
1. Transition from IDLE to BUSY bus
2. Start, middle, and end of write or read message transaction
3. Transition from BUSY to IDLE bus

If an error is detected at any of these steps and cannot be resolved, no further action is taken by pi_i2c. An error code is reported and it is then responsibility of the caller to recover the bus (power cycle of the offending component is typical).

The following table summarizes the types of bus errors detected, actions taken by pi_i2c.c, the error number returned to the caller, and suggested recovery actions. Suggested recovery actions are to be performed by the caller. 

| Bus Error | Action Taken | Error Number | Further Recovery Actions|
|-|-|-|-|
| Device did not acknowledge device address | Halt transaction and return error number | `ENACK` | Check if device is responsive; otherwise power cycle device |
| Device did not acknowledge register address | Halt transaction and return error number | `EBADREGADDR` | Check if device is responsive; otherwise power cycle device |
| Device did not acknowledge during byte transfer | Halt transaction and return error number | `EBADXFR` | Check if device is responsive; otherwise power cycle device |
| Device not responsive after clock stretch timeout (device forcing SCL line held low) | Halt transaction and return error number | `ECLKTIMEOUT` | Check if device is responsive; otherwise power cycle device |
| Device did not respond after repeated start | Halt transaction and return error number | `ENACKRST` | Check if device is responsive; otherwise power cycle device |
| Device is forcing SDA line low (device hung) | Reset bus to recover; if still hung, halt transaction and return error number | `EdeviceHUNG` | Power cycle device |
| Device is holding SDA and SCL lines low causing bus to be locked | Halt transaction and return error number | `EBUSLOCKUP` | Power cycle device |
| Device is forcing SDA and/or SCL line low causing START condition writes to fail | Halt transaction and return error number | `EFAILSTCOND` | Power cycle device |
| Bus is an unexpected state following an unknown error | Halt transaction and return error number | `EBUSUNKERR` | Power cycle device |

Bottom line: if pi_i2c.c sees the bus in an expected state at any time then the transaction will be cancelled. This is to prevent any accidental and undefined data transfers to take place which may cause hardware damage (ask me about my broken IMU if would like to know more about this).

### Notes on Bit Rate
Bit rate achievable by pi_i2c.c is primarily a function of the clock accuracy, minimum I2C timings, and I2C protocol messaging overhead:
* [pi_microsleep_hard.c](https://github.com/besp9510/pi_microsleep_hard) provides a hard microsleep function with a resolution of 1 us
* Minimum I2C timings (found in the above table) rounded up to the nearest 1 us (per pi_microsleep_hard.c)
* Messaging overhead: STOP, START, Repeated START conditions, ACKS, and device & register addresses

Using these constraints, pi_i2c.c achieves a total and useful bit rate that will be lower than ideal. I2C full-speed mode (400 kHz), for example, would have the following timings:

![gpio](images/example_timings.png)

Actual frequency is less than the desired 400 kHz due to the 1 us rounding on T_Low & T_High required by the hard microsleep function. Additionally, consider the minimum I2C timings have also been rounded resulting is longer wait times. End result is smaller bit rate at the trade of flexibility and overall features pi_i2c.c provides.

The theoretical useful bit rate can be then calculated for a n-byte read and write transaction using these achievable timings. Useful bit rate is defined as how much "useful" data is being transferred in a given read or write transaction. Not all data in an I2C message is useful in the sense that some of it is overhead and not the data we are actually trying to transfer between the controller and device. Theoretical useful bit rates are calculated and plotted for the I2C full-speed mode (400 kHz) example discussed above:

![gpio](images/example_bitrates.png)

*Note that these are theoretical useful bit rates. In practice, pi_i2c.c will provide lower rates than these theoretical values because of the operating system's overhead and scheduler.*

The asymptotic trend lines of write and read is the result of transferring more data in one transaction while messaging overhead remains constant (number of STARTs, STOPs, ACKs, and register addresses do not change). As the number of bytes read/written increases, the smaller penalty the messaging overhead imposes resulting in a useful bit rate that asymptotically approaches ~348 kbps for write and ~297 kbps for read. I2C transactions that transfer only a couple of bytes at a time are much more inefficient than ones that transfer 10s of bytes.

### Functions

#### Configure I2C Library
Setup the library prior to using pi_i2c.c. Define which GPIO pins will be used for the SDA & SCL lines and what the speed grade (bit rate) is. This function must be called prior to using any other functions or otherwise they will return the `EI2CNOTCFG` error number. Any GPIO pins can be used but if you select the Pi's default SDA & SCL pins (BCM pin 2 & 3), ensure that Raspian I2C interface is disabled via rasp-config or otherwise risk unpredictable behavior. Available speed grades to choose from are:
* `I2C_STANDARD_MODE` (100 kHz)
* `I2C_FULL_SPEED` (400 kHz)

```c
int config_i2c(unsigned int sda, unsigned int scl, unsigned int speed_grade);
```

`unsigned int sda` is the GPIO pin to use for the SDA line (BCM numbering).

`unsigned int scl` is the GPIO pin to use for the SCL line (BCM numbering).

`unsigned int speed_grade` is the speed grade (bit rate) to use for the I2C bus according to standard I2C definition.

##### Return Value
`config_i2c()` returns 0 upon success. On error, an error number is returned.

Error numbers:
* `ENOPIVER` : Could not get Pi board revision.
* `MAP_FAILED` : Memory map failed (most likely due to permissions)
* `EINVAL` : Invalid argument (e.g. sda, scl, or speed grade out of range)

#### Scan I2C Bus
Scan the bus for any I2C devices. Note that only 7-bit addresses are supported. The function requires a pointer to an address book (a 127-element integer array) to be passed as an argument. The indices of this address book correspond directly to an I2C address; for example, index 0x1 of the array corresponds to an I2C device at an address of 0x1.

```c
address_book[127] = {
    [Index/Address] = 0 (Not present) / 1 (Detected)
}
```

The results of the bus scan will be stored back into this address book. If the value of an element at index *i* equals 1 then a I2C device at an address equaling index *i* is present.

```c
int scan_bus_i2c(int *address_book);
```

The address book `int *address_book` argument is a pointer to a 127-element integer array where the results of the bus scan are stored back into.

##### Return Value
`scan_bus_i2c()` returns 0 upon success. On error, an error number is returned.

Error numbers:
* `EI2CNOTCFG` : pi_i2c has not yet been configured
* `EDEVICEHUNG` : Device forcing SDA line low
* `ECLKTIMEOUT` : Device not responsive after clock stretch timeout
* `EBUSLOCKUP` : Bus is locked: SDA and SCL lines are being held low by device
* `EBUSUNKERR` : Bus is in an unexpected state following an unknown error
* `ECLKTIMEOUT` : Device not responsive after clock stretch timeout
* `EFAILSTCOND` : Failed to write a START condition to the bus. Most likely, error occurred during a previous STOP condition.

#### Write

Write n-bytes to a device's register address. Data to write to the device's register address is passed into the function as a pointer to an n-byte integer data array.

```c
int write_i2c(unsigned int device_address, unsigned int register_address, int *data, unsigned int n_bytes);
```

The `unsigned int device_address` argument is the device's I2C address.

The `unsigned int register_address` argument is the specific register data will be written to.

Data to write to the device's register address is stored in the `int *data` argument: a `n_bytes` integer array that is passed into the function as a pointer.

Number of bytes `unsigned int n_bytes` argument is the number of bytes to write to the device's register address. `int *data` must be at least `n_bytes` large.

##### Return Value
`write_i2c()` returns 0 upon success. On error, an error number is returned.

Error numbers:
* `EI2CNOTCFG` : pi_i2c has not yet been configured
* `EDEVICEHUNG` : Device forcing SDA line low
* `ECLKTIMEOUT` : Device not responsive after clock stretch timeout
* `EBUSLOCKUP` : Bus is locked: SDA and SCL lines are being held low by device
* `EBUSUNKERR` : Bus is in an unexpected state following an unknown error
* `ECLKTIMEOUT` : Device not responsive after clock stretch timeout
* `EFAILSTCOND` : Failed to write a START condition to the bus. Most likely, error occurred during a previous STOP condition.
* `ENACK` : Device did not acknowledge device address
* `EBADXFR` : Device did not acknowledge during byte transfer (read or write)
* `EBADREGADDR` : Device did not acknowledge register address
* `EINVAL` : Invalid argument (e.g. device_address or register address out of range; negative n_bytes)

#### Read

Read n-bytes from a device's register address. Data read from the device will be stored back into the n-byte integer data array passed into the function by pointer.

```c
int read_i2c(unsigned int device_address, unsigned int register_address, int *data, unsigned int n_bytes);
```

The `unsigned int device_address` argument is the device's I2C address.

The `unsigned int register_address` argument is the specific register data will be read from. Note, when more than 1 byte is read from A register, the register address will automatically increase by 1 each time a byte is read. This results in the n-byte of data being read from `int register_address` + n (indexed from 0).

Data read from device's register address is stored back in the `int *data` argument: a `n_bytes` integer array that is passed into the function as a pointer. 

Number of bytes `unsigned int n_bytes` argument is the number of bytes to read to the device's register address. `int *data` must be at least `n_bytes` large.

##### Return Value
`read_i2c()` returns 0 upon success. On error, an error number is returned.

Error numbers:
* `EI2CNOTCFG` : Pi I2C has not yet been configured
* `EDEVICEHUNG` : Device forcing SDA line low
* `ECLKTIMEOUT` : Device not responsive after clock stretch timeout
* `EBUSLOCKUP` : Bus is locked: SDA and SCL lines are being held low by device
* `EBUSUNKERR` : Bus is in an unexpected state following an unknown error
* `ECLKTIMEOUT` : Device not responsive after clock stretch timeout
* `EFAILSTCOND` : Failed to write a START condition to the bus. Most likely, error occurred during a previous STOP condition.
* `ENACK` : Device did not acknowledge device address
* `EBADXFR` : Device did not acknowledge during byte transfer (read or write)
* `EBADREGADDR` : Device did not acknowledge register address
* `ENACKRST` : Device did not respond after repeated start device address
* `EINVAL` : Invalid argument (e.g. device_address or register address out of range; negative n_bytes)

#### Reset Bus

Reset I2C bus by issuing 9 clock pulses. Typically used to un-stuck the SDA line after a device is forcing it low. This function is automatically called in the case of error handling but is available to used at any time.

```c
int reset_i2c(void);
```

##### Return Value
`reset_i2c()` returns 0 upon success. On error, an error number is returned.

Error numbers:
* `EI2CNOTCFG` : Pi I2C has not yet been configured
* `ECLKTIMEOUT` : Device not responsive after clock stretch timeout

#### Get Statistics

Return a structure of statistics recorded by pi_i2c.c.

```c
struct pi_i2c_statistics get_statistics_i2c(void);
```

##### Return Value
`get_statistics_i2c()` always returns 0 upon success.

#### Get Configs

Return a structure of internal configurations of pi_i2c.c.

```c
struct pi_i2c_configs get_configs_i2c(void);
```

##### Return Value
`get_configs_i2c()` always returns 0 upon success.

### Bash Executable
The bash executable version of pi_i2c is a CLI interface with the C shared library of pi_i2c.c. This executable takes in options and arguments that are then passed to the respective pi_i2c.c functions (defined above). Output is then directed back to the terminal. This interface is useful for one-off debugging, inspections, or any time it makes sense to interact with a device on a more impromptu basis.

The following is the usage statement printed anytime `pi_i2c` is invoked with the help option. By default, the executable is installed to the `usr/local/bin/` directory which should be on your `$PATH` environmental variable. `pi_i2c` will be callable from any directory if that is the case.

```
$ pi_i2c --help
Usage: pi_i2c [OPTIONS]
Inter-Integrated Circuit (I2C) Library for the Raspberry Pi

Examples:
  pi_i2c --sda 2 --scl 3 -- speed-grade i2c_standard_mode --scan
  pi_i2c -r --sda 2 --scl 3 --speed-grade 400 --device 0x1C --register 0x23 --bytes 2
  pi_i2c -w --sda 2 --scl 3 --speed-grade 100 --device 0x3D --register 0x01 --bytes 1 --data 0xFF,0x1C
  pi_i2c -r -a 2 -c 3 -g i2c_full_speed -e 0x70 -i 0x0 -n 1
  pi_i2c -w -a 2 -c 3 -g 400 -e 0x70 -i 0x0 -n 1 -d 0x8F

Main operation mode:
  -r, --read         read N bytes from a device and register address
  -w, --write        write N bytes from a device and register address
  -a, --sda          GPIO pin to use for the SDA line (BCM numbering)
  -c, --scl          GPIO pin to use for the SCL line (BCM numbering)
  -g, --speed-grade  I2C bus speed grade (bit rate) as defined by pi_i2c.h
                     valid speed grades are i2c_standard_mode (100) or i2c_full_speed (400)
  -e, --device       device I2C address as a hex number (e.g., 0xFF)
  -i, --register     device's register address read from or written as a hex number (e.g., 0xFF)
  -s, --scan         scan the bus for any I2C devices
  -n, --n-bytes      number of bytes to read or write to the device's register address
  -d, --data         data to write to the device's register address in a comma delimited list
                     data type is hex (e.g., 0xFF)
  -v, --debug        display useful information
  -h, --help         display this help and exit

User's notes:
  - If using the --scan option, device, register, n_bytes, data, read, and write options are silently ignored

Need any help? find a bug? Checkout https://github.com/besp9510/pi_i2c
```

*Note, data read from a device is only printed to the terminal.*

Examples:

#### I2C Bus Scan

```
$ pi_i2c -a 2 -c 3 -g i2c_full_speed -s
```

A table is printed along with a list of detected devices.

```
pi_i2c: I2C bus scan completed
    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
00  x  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
10  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
20  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
30  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
40  x  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
50  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
60  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
70  x  x  x  -  x  -  -  -  -  -  -  -  -  -  -  -
devices detected at [0x0, 0x40, 0x70, 0x71, 0x72, 0x74]
```

#### Write

```
$ pi_i2c -w -a 2 -c 3 -g 400 -n 1 -e 0x70 -i 0x0 -d 0x2F
```

Data written to registers is echo'd to the terminal. Then, the registers are read to confirm successful write and their contents are printed to the terminal.

```
pi_i2c: wrote 1 byte(s) to device 0x70 at register 0x0
pi_i2c: wrote data = [0x2F]
pi_i2c: reading back 1 byte(s) from device 0x70 at register 0x0
pi_i2c: register values = [0x2F]
```

#### Read

```
$ pi_i2c -r -a 2 -c 3 -g i2c_full_speed -e 0x70 -i 0x0 -n 6
```

The data read from the registers are printed to the terminal.

```
pi_i2c: reading 6 byte(s) from device 0x70 at register 0x0
pi_i2c: register values = [0x2F, 0x4, 0xE2, 0xE4, 0xE8, 0xE0]
```

## Contributing
Follow the "fork-and-pull" Git workflow.
1. Fork the repo on GitHub
2. Clone the project to your own machine
3. Commit changes to your own branch
4. Push your work back up to your fork
5. Submit a Pull request so that your changes can be reviewed

Be sure to merge the latest from "upstream" before making a pull request!

Feel free to email at the email address under my account name if you have any questions.
## Authors

Benjamin Spencer

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details