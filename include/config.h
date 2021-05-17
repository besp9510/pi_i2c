// I2C timings
#define MIN_T_LOW 1.3e-6   // SCL Low Period [seconds]
#define MIN_T_HIGH 0.6e-6  // SCL High Period [seconds]
#define MIN_T_SUDAT 0.1e-6 // SDA Setup Time [seconds]
#define MIN_T_HDDAT 0      // SDA Hold Time [seconds]
#define MIN_T_SUSTA 0.6e-6 // Setup Time for a repeated Start condition [seconds]
#define MIN_T_HDSTA 0.6e-6 // Hold Time for a Start condition [seconds]
#define MIN_T_SUSTO 0.6e-6 // Setup time for a Stop condition [seconds]
#define MIN_T_BUF 1.3e-6   // Time before a new transmission can start [seconds]

#define SCL_RESPONSE_TIME 1e-9 // Time for SCL to change after set/clear =

#define CLOCK_STRETCHING_TIMEOUT_US 500e3 // Clock stretching timeout [micro seconds]

#define ACK 0  // Slave ACK
#define NACK 1 // Slave NACK

#define WRITE_FLAG 0 // Master write
#define READ_FLAG 1  // Master read

#define CONTINUE_FLAG 0 // No STOP condition at end of write
#define STOP_FLAG 1     // STOP condition at end of write

// Global variables:
extern int sda_gpio_pin; // Data line
extern int scl_gpio_pin; // Clock line

extern int scl_clock_frequency_hz;          // Desired clock frequency
extern float scl_actual_clock_frequency_hz; // Actual clock frequency

extern int config_i2c_flag; // I2C lines and timings defined?

extern struct pi_i2c_statistics statistics;

// I2C timing compliance:
extern struct timespec min_t_hdsta_sleep;      // Hold Time for START condition
extern struct timespec min_t_susto_sleep;      // Setup time for STOP condition
extern struct timespec min_t_buf_sleep;        // Time before new transmission
extern struct timespec scl_t_low_sleep;        // SCL Low Period
extern struct timespec scl_t_high_sleep;       // SCL High Period
extern struct timespec clock_stretching_sleep; // Support clock stretching
extern struct timespec scl_response_time;      // Time for SCL to change