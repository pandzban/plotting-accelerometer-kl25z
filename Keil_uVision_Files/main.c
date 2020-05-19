/*
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\
\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\
		Intermodular Communications Standards project, Made by Adam Gawlik, Christopher Dudzik and Matthias Nawrocki
\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\
\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
*/

#include "MKL25Z4.h"                    	/* Device header */
#include "leds.h"														  
#include "extra.h"	
#include "pit.h"
#include "uart.h"
#include "send.h"
#include "i2c.h"


volatile uint8_t started_receiving = 0;
volatile int last_received = 0;

volatile uint8_t config = 0;
volatile uint8_t OnlyMSB = 1;
volatile uint8_t resolution = 2; //g
volatile uint8_t lownoise = 1;
volatile int interval = 20;
volatile uint8_t i2c_routine = 0;
volatile int inter_counter = 0;
volatile int acc_val = 8;
volatile uint8_t configuration[3];
volatile uint8_t acc_en = 0;
volatile uint8_t active_read = 0;


void Initialization_List(void){
	ledsInitialize();
	pitInitialize();
	systickInit();
	Q_Initialize();
	uart0Initialize();
}

int main(void){
	Initialization_List();
	i2c_init();
	acc_init();
	ledsOff();
	init_mma();
	__enable_irq();

	while(1){
		check_for_config();  // checks if configuration has arrived over uart from PC
		if (i2c_routine == 1){   // after interval time pit asserts it
			read_full_xyz();  // reads and processes data from accelerometer
			i2c_routine = 0;
		}
	}
}

