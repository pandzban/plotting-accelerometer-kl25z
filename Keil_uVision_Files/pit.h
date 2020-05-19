
#include "MKL25Z4.h" 
#include "extra.h"
#include "leds.h"

volatile extern int last_received;
volatile extern int interval;
volatile extern int inter_counter;
volatile extern uint8_t i2c_routine;


void pitInitialize(void);
void systickInit(void);


