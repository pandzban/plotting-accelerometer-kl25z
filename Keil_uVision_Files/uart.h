#ifndef uart
#define uart


#include "MKL25Z4.h" 
#include "extra.h"
#include "send.h"
#include "leds.h"


void uart12Initialize(void);
void uart0Initialize(void);
volatile extern uint8_t started_receiving;
volatile extern int last_received;
volatile extern int last_send;
#endif
