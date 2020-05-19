
#ifndef I2C
#define I2C

#include "MKL25Z4.h" 
#include "extra.h"
#include "leds.h"
#include "send.h"

#define I2C_M_START		I2C0->C1 |= I2C_C1_MST_MASK
#define I2C_M_STOP		I2C0->C1 &= ~I2C_C1_MST_MASK
#define I2C_M_RSTART	I2C0->C1 |= I2C_C1_RSTA_MASK
#define I2C_TRAN			I2C0->C1 |= I2C_C1_TX_MASK
#define I2C_REC				I2C0->C1 &= ~I2C_C1_TX_MASK
#define I2C_WAIT			while((I2C0->S & I2C_S_IICIF_MASK)==0) {} \
											I2C0->S |= I2C_S_IICIF_MASK;
#define NACK					I2C0->C1 |= I2C_C1_TXAK_MASK
#define ACK						I2C0->C1 &= ~I2C_C1_TXAK_MASK
	
#define REG_WHOAMI 0x0d
#define WHOAMI 0x1a
#define MMA_ADDR (0x1d << 1) // shifted device address
#define REG_CTRL1 0x2a
#define REG_XHI 0x01
#define XYZ_DATA_CFG 0x0e
	
#define ACC_MAX_SIZE 64
#define STOP 115
#define SHOW_CONF 113
#define ACC_UNIT 8
#define INTER_UNIT 2
#define CONFIG_START 250
#define TIME_WAIT 100
	
struct Accumulator{   // averaging samples holding structure
	int Data[3][ACC_MAX_SIZE];
	uint8_t Ext_Mode_On;
	int Size;
	int Current_Max_Size;
};

extern struct Accumulator acc; // it's name
	
volatile extern uint8_t config;
volatile extern uint8_t OnlyMSB;
volatile extern uint8_t resolution;
volatile extern uint8_t lownoise;

volatile extern uint8_t started_receiving;
volatile extern int last_received;
volatile extern int interval;
volatile extern int inter_counter;
volatile extern int acc_val;
volatile extern uint8_t acc_en;
volatile extern uint8_t configuration[3];
	
volatile extern uint8_t active_read;
	
void i2c_init(void);
void acc_init();
void i2c_write_byte(uint8_t dev, uint8_t reg, uint8_t data);
int i2c_read_bytes(uint8_t dev_adx, uint8_t reg_adx, uint8_t * data, int8_t data_count);
uint8_t i2c_read_byte(uint8_t dev, uint8_t reg);
void check_for_config(void);
void read_full_xyz();
uint8_t sm2tc(int val);
int uzup_do_dwoch(uint8_t val);
int init_mma();
	
#endif
