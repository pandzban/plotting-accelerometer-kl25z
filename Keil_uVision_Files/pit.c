
 
#include "pit.h"


	//CLK = 1MHz
#define COUNT_FREQ 1000000

#define PIT_FREQ1 1000 // in Hertz
#define PIT_FREQ2 1000






void systickInit(void){
	  NVIC_SetPriority(SysTick_IRQn,8);
    NVIC_ClearPendingIRQ(SysTick_IRQn);
    NVIC_EnableIRQ(SysTick_IRQn);
		
    SysTick_Config(SystemCoreClock / 1000);  // Configure SysTick to generate an interrupt every millisecond 
}

void SysTick_Handler(void)  {                          
    NVIC_ClearPendingIRQ(SysTick_IRQn);
		inter_counter++;	
	
		if (inter_counter == interval){
			i2c_routine = 1;
			inter_counter = 0;
		}
		last_received++;
		if (last_received > 50000){
			last_received = 500;
		}                      
}



void pitInitialize(void){     

	int LDVAL1 = COUNT_FREQ / PIT_FREQ1 - 1 ;
	int LDVAL2 = COUNT_FREQ / PIT_FREQ2 - 1 ;
	
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	PIT->MCR &= ~PIT_MCR_MDIS_MASK; // enabled by writing 0
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	//PIT->CHANNEL[0].LDVAL = 0x123456;//PIT_LDVAL_TSV(starting_value);
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(LDVAL1); 
	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_CHN_MASK; // no chain
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK; // generate interrupts for channel 0
	
	PIT->CHANNEL[1].LDVAL = PIT_LDVAL_TSV(LDVAL2);
	PIT->CHANNEL[1].TCTRL &= ~PIT_TCTRL_CHN_MASK;
	//PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;

	NVIC_ClearPendingIRQ(PIT_IRQn);  	
	NVIC_EnableIRQ(PIT_IRQn);			
	
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK; // enable timer channel
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK; // enable timer channel
	PIT->MCR &= ~PIT_MCR_FRZ_MASK; // disable freeze
}


void PIT_IRQHandler(void){
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) { 
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK; 
	}
 if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {
		PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK;
	}
 	NVIC_ClearPendingIRQ(PIT_IRQn);
}

