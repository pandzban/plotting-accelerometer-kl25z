#ifndef SEND
#define SEND

#include "MKL25Z4.h" 
#include "extra.h"


#define SEND_QUEUE_SIZE 128

struct SendQueue{     											// Send and Rcv Buffer queue, used for irq communication with PC
	uint8_t qData[SEND_QUEUE_SIZE];
	unsigned int qHead;
	unsigned int qSize;
	unsigned int qTail;
};

extern struct SendQueue SndBuff, RcvBuff;

void Q_Initialize(void);
void QS_Init(struct SendQueue * q);
int QS_Empty(struct SendQueue * q);
int QS_Full(struct SendQueue * q);
int QS_Enqueue(struct SendQueue * q, uint8_t d);
uint8_t QS_Dequeue(struct SendQueue * q);
int QS_Send(struct SendQueue * q, int uart_num);

int Q_Receive(volatile uint8_t * word, int uart_num);
int Q_Send(int uart_num, uint8_t word);

#endif
