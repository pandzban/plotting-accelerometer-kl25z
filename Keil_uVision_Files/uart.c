
 
 #include "uart.h"
 #include "send.h"
 #define BUS_CLOCK 24e6
 #define XTAL 8e6
 
 uint32_t baud_rate0 = 19200; // 9600 // 19200
 uint32_t baud_rate1 = 9600;
 uint32_t baud_rate2 = 9600;
 
 void uart0Initialize(void){
	  uint32_t divisor = 0;
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;		//clock assignment
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;		
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(2);
	PORTA->PCR[1] = PORT_PCR_MUX(2u);			//PTA1=RX_D
	PORTA->PCR[2] = PORT_PCR_MUX(2u);			//PTA2=TX_D
	UART0->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK );		//blockade of tx and rx
	divisor = XTAL/(baud_rate0*32);
	UART0->BDH = UART_BDH_SBR(divisor>>8); 
	UART0->BDL = UART_BDL_SBR(divisor);
	UART0->BDH &= ~UART0_BDH_SBNS_MASK; // 1 bit of stop
	UART0->C4 |= UART0_C4_OSR(31);
	UART0->C5 |= UART0_C5_BOTHEDGE_MASK;
	UART0->C2 |= UART_C2_RIE_MASK | UART_C2_TIE_MASK;
	UART0->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);		//enable tx and rx
	NVIC_EnableIRQ(UART0_IRQn);
	NVIC_ClearPendingIRQ(UART0_IRQn);
 }
 
 
 void uart12Initialize(void){
 uint32_t divisor = 0;
	SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; 
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTC->PCR[4] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[3] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[2] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[3] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[4] |= PORT_PCR_MUX(3); // ALT3 = UART
	PORTC->PCR[3] |= PORT_PCR_MUX(3);
	PORTD->PCR[2] |= PORT_PCR_MUX(3);
	PORTD->PCR[3] |= PORT_PCR_MUX(3);
	UART1->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );	
	UART2->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );	
	divisor = BUS_CLOCK/(baud_rate1*16);
	UART1->BDH = UART_BDH_SBR(divisor>>8);
	UART1->BDL = UART_BDL_SBR(divisor);
	divisor = BUS_CLOCK/(baud_rate2*16);
	UART2->BDH = UART_BDH_SBR(divisor>>8);
	UART2->BDL = UART_BDL_SBR(divisor);
	UART1->BDH &= ~UART_BDH_SBNS_MASK; // 1 bit of stop
	UART2->BDH &= ~UART_BDH_SBNS_MASK;
	UART1->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);	
	UART2->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);	
}

void UART0_IRQHandler() { 
	if (UART0->S1 & UART0_S1_RDRF_MASK){ // chceck RX mask of UART
		if (!QS_Full(&RcvBuff)){ // check if rx buffer is not full
			char d = UART0->D; // if not take the word from uart's register
			QS_Enqueue(&RcvBuff, d); // and write it to buffer :: RcvBuff
			started_receiving = 1;
			last_received = 0;
		}
	}
	if ((UART0->C2 & UART_C2_TIE_MASK) && (UART0->S1 & UART0_S1_TDRE_MASK)){ // masks of tx readiness and it's setting being enabled
		if (!QS_Empty(&SndBuff)){ // if tx buffer is not empty
			QS_Send(&SndBuff, 0); // we write one of it's words to uart shift register
		} else {
			UART0->C2 &= ~UART0_C2_TIE_MASK; // removal of mask enabling interrupts from empty send register
		}        // to not induce irq while buffer is empty // should be set by function enqueueing data to send buffer
	}
	NVIC_ClearPendingIRQ(UART0_IRQn);
}

