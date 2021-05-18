// TODO
// - Config set functions
// - Config and spec get functions

// I2C speed grades define in bits/second:
#define I2C_STANDARD_MODE 100e3
#define I2C_FULL_SPEED 400e3
#define I2C_FAST_MODE 1e6
#define I2C_HIGH_SPEED 3.2e6
#define I2C_ULTRA_FAST_MODE 5e6

// Error numbers:
#define ENACK 1       // Slave did not acknowledge slave address
#define EBADXFR 2     // Slave did not acknowledge during byte transfer
                      // (read or write)
#define EBADREGADDR 3 // Slave did not acknowledge register address
#define ECLKTIMEOUT 4 // Slave not responsive after clock stretch timeout
#define EI2CNOTCFG 5  // I2C has not yet been configured
#define ENACKRST 6    // Slave did not respond after repeated start slave
                      // address (read)
#define EBUSLOCKUP 7  // Bus is locked: SDA and SCL lines are being held low
                      // by slave
#define EBUSUNKERR 8  // Bus is in an unexpected state following an unkown
                      // error
#define EFAILSTCOND 9 // Failed to write a START condition to the bus. Most
                      // likely cause is previously seen bus error occuring
                      // during STOP condition.
#define ESLAVEHUNG 10 // Slave forcing SDA line low

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
    long scl_t_low_sleep_us;
    long scl_t_high_sleep_us;
    float scl_actual_clock_frequency_hz;
};

// I2C function prototypes:
int config_i2c(int sda, int scl, int speed_grade);
int scan_bus_i2c(int *address_book);
int write_i2c(int slave_address, int register_address, int *data, int n_bytes);
int read_i2c(int slave_address, int register_address, int *data, int n_bytes);
int reset_i2c(void);
struct pi_i2c_statistics get_statistics_i2c(void);
struct pi_i2c_configs get_configs_i2c(void);