

#include "i2c.h"

struct Accumulator acc;

void acc_clear(struct Accumulator * acc){
	int i;
	if ((acc->Ext_Mode_On)==0){
		for (i = 0; i < acc->Current_Max_Size; i++){
			acc->Data[0][i] = 0;
			acc->Data[1][i] = 0;
			acc->Data[2][i] = 0;
		}
	} else {
		for (i = 0; i < acc->Current_Max_Size; i++){
			acc->Data[0][i] = 0;
			acc->Data[1][i] = 0;
			acc->Data[2][i] = 0;
		}	
	}
	acc->Size = 0;
}

void acc_clear_all(struct Accumulator * acc){
	int i;
	if ((acc->Ext_Mode_On)== 0){
		for (i = 0; i < ACC_MAX_SIZE; i++){
			acc->Data[0][i] = 0;
			acc->Data[1][i] = 0;
			acc->Data[2][i] = 0;
		}
	} else {
		for (i = 0; i < ACC_MAX_SIZE; i++){
			acc->Data[0][i] = 0;
			acc->Data[1][i] = 0;
			acc->Data[2][i] = 0;
		}	
	}
	acc->Size = 0;
}

void acc_init(){
	configuration[0] = 250;
	if (OnlyMSB == 1){
		acc.Ext_Mode_On = 0;
	} else {
		acc.Ext_Mode_On = 1;
	}
	acc.Current_Max_Size = acc_val;
	acc_clear_all(&acc);
}

void send_configuration(){
	configuration[0] = CONFIG_START;
	uint8_t temp1 = 0;
	temp1 += lownoise;
	if (resolution == 2){
			temp1 += 0 << 1;
	} else if(resolution == 4){
		temp1 += 1 << 1;
	} else if(resolution == 8){
			temp1 += 2 << 1;
	}
	if (OnlyMSB == 1){
		temp1 += 0;
	} else {
		temp1 += (1 << 3);
	}
	temp1 += ((acc_val/ACC_UNIT)-1) << 4;
	temp1 += acc_en << 7;
	configuration[1] = temp1;
	temp1 = 0;
	temp1 = ((interval/INTER_UNIT)-1);
	configuration[2] = temp1;
	int i;
	ledblueBlink(4,25);
	for (i = 0; i < 3; i++){
		while(!QS_Enqueue( &SndBuff, configuration[i])){}
	}
}

void acc_change_size(struct Accumulator * acc, unsigned int new_size){
	acc->Current_Max_Size = new_size;
	acc_clear_all(acc);
}

void signal(void){
	int i;
	for (i = 0; i < 5; i++){
		while(!QS_Enqueue( &SndBuff, 45)){}
	}
	while(!QS_Enqueue( &SndBuff, 25)){}
	while(!QS_Enqueue( &SndBuff, 45)){}
}

void acc_change_mode(struct Accumulator * acc, int mode){
	if ((mode==0) && (acc->Ext_Mode_On == 0)){
		acc->Ext_Mode_On = 1;
		acc_clear_all(acc);
	} else if (((mode)==1) && (acc->Ext_Mode_On == 1)) {
		acc->Ext_Mode_On = 0;
		acc_clear_all(acc);
	}
	signal();
}

int uzup_do_dwoch(uint8_t val){
	int temp;
	if (val > 127){
		val = val & 0x7F;
		temp = val - 128;
	} else {
		temp = val;
	}
	return temp;
}		

int uzup_do_dwoch2(uint16_t val){
	int temp;
	if (val > 0x1FFF){
		val = val & 0x1FFF;
		temp = val - 0x2000;
	} else {
		temp = val;
	}
	return temp;
}

int acc_empty(struct Accumulator * acc) {
	return acc->Size == 0;
}

int acc_full(struct Accumulator * acc) {
	return acc->Size == acc->Current_Max_Size;
}

int saturate( int val){
	if (val > 127){
		val = 127;
	} else if (val < -128){
		val = -128;
	}
	return val;
}

int saturate2(int val){
	if (val > 0x1FFF){
		val = 0x1FFF;
	} else if (val < -0x2000){
		val = -0x2000;
	}
	return val;
}


uint8_t sm2tc(int val) {
	uint8_t temp;
	val = saturate(val);
	if (val < 0){
		val = val + 128;
		val = val | 0x80;
	}
	temp = val;
	return temp;
}

uint16_t sm2tc2(int val) {
	uint16_t temp;
	val = saturate2(val);
	if (val < 0){
		val = val + 0x2000;
		val = val | 0x2000;
	}
	temp = val;
	return temp;
}



int acc_calc_and_send(struct Accumulator * acc){
	int temp[6];
	int i;
	uint8_t tempx = 0;
	uint16_t tempex = 0;
	float tempf = 0;
	for (i = 0; i < 6; i++){
		temp[i] = 0;
	}
	if (acc->Ext_Mode_On==1){
		for (i = 0; i < acc->Current_Max_Size; i++){
			temp[0] += acc->Data[0][i];
			temp[2] += acc->Data[1][i];
			temp[4] += acc->Data[2][i];
		}
		for (i = 0; i < 6; i += 2){
			tempf = ((float)(temp[i])) / ((float)(acc->Size));
			temp[i] = (int)tempf;
			tempex = sm2tc2( temp[i]);
			tempx = (tempex & 0x3FC0) >> 6;
			while(!QS_Enqueue( &SndBuff, tempx)){}
			tempx = (tempex & 0x003F) << 2;
			while(!QS_Enqueue( &SndBuff, tempx)){}
		}
	} else {
		for (i = 0; i < acc->Current_Max_Size; i++){
			temp[0] += acc->Data[0][i];
			temp[2] += acc->Data[1][i];
			temp[4] += acc->Data[2][i];
		}
		for (i = 0; i < 6; i += 2){
			tempf = ((float)(temp[i])) / ((float)(acc->Size));
			temp[i] = (int)tempf;
			tempx = sm2tc(temp[i]);
			while(!QS_Enqueue( &SndBuff, tempx)){}
		}
	}
	UART0->C2 |= UART0_C2_TIE_MASK;
	acc_clear(acc);
	return 1;
}

uint16_t move(uint8_t left, uint8_t right){
	uint8_t temp;
	uint16_t tempx;
	temp = right >> 2;
	tempx = left << 6;
	tempx += temp;
	return tempx;
}

int acc_enqueue(struct Accumulator * acc, uint8_t * d) {
	uint16_t tempx;
	if (acc_full(acc)==0) {
		if (acc->Ext_Mode_On==1){
			tempx = move(d[0], d[1]);
			acc->Data[0][acc->Size] = uzup_do_dwoch2(tempx);
			tempx = move(d[2], d[3]);
			acc->Data[1][acc->Size] = uzup_do_dwoch2(tempx);
			tempx = move(d[4], d[5]);
			acc->Data[2][acc->Size++] = uzup_do_dwoch2(tempx);
		} else {
			acc->Data[0][acc->Size] = uzup_do_dwoch(d[0]);
			acc->Data[1][acc->Size] = uzup_do_dwoch(d[2]);
			acc->Data[2][acc->Size++] = uzup_do_dwoch(d[4]);
		}
		
		if (acc->Size == acc->Current_Max_Size){
			acc_calc_and_send(acc);
		}
		return 1; // success
	} else
		return 0; // failure
}

void i2c_init(void){
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	PORTE->PCR[24] |= PORT_PCR_MUX(5);
	PORTE->PCR[25] |= PORT_PCR_MUX(5);
	
	// baudrate 400k
	// baud = bus freq/(scl_div + mul)
	// 24MHz / 400kHz = 60; icr = 0x11 => scl_div = 56
	I2C0->F = I2C_F_ICR(0x11) | I2C_F_MULT(0);
	
	I2C0->C1 |= I2C_C1_IICEN_MASK; // set to master mode
	
	I2C0->C2 |= I2C_C2_HDRS_MASK;
}

uint8_t i2c_read_byte(uint8_t dev, uint8_t reg){
	uint8_t data;
	
	I2C_TRAN;
	I2C_M_START;
	I2C0->D = dev;
	I2C_WAIT
	
	I2C0->D = reg;
	I2C_WAIT
	
	I2C_M_RSTART;
	I2C0->D = (dev|0x01);  // send dev address
	I2C_WAIT
	
	I2C_REC;
	NACK;		// NACK adter read
	
	data = I2C0->D;  // dummy read
	I2C_WAIT 
	
	I2C_M_STOP;
	data = I2C0->D;
	
	return data;
}

int i2c_read_bytes(uint8_t dev_adx, uint8_t reg_adx, uint8_t * data, int8_t data_count){
	uint8_t dummy;
	int8_t num_bytes_read = 0;
	I2C_TRAN;
	I2C_M_START;
	I2C0->D = dev_adx;
	I2C_WAIT
	I2C0->D = reg_adx;
	I2C_WAIT
	I2C_M_RSTART;
	I2C0->D = dev_adx|0x01;
	I2C_WAIT
	I2C_REC;
	ACK;
	dummy = I2C0->D;
	I2C_WAIT
	do {
		ACK;
		data[num_bytes_read++] = I2C0->D;
		I2C_WAIT
	} while (num_bytes_read < data_count - 2);
	NACK;
	data[num_bytes_read++] = I2C0->D;
	I2C_WAIT
	I2C_M_STOP;
	return 1;
}

void read_full_xyz(){
	if (active_read == 1){
		int i;
		uint8_t data[6];
		if (acc_en == 0){
			if (OnlyMSB == 0){
				i2c_read_bytes(MMA_ADDR, REG_XHI, data, 6);
				for (i = 0; i < 6; i++){
					while(!QS_Enqueue(&SndBuff, data[i])){}
				}
				UART0->C2 |= UART0_C2_TIE_MASK; // send mask
			} else { 
				i2c_read_bytes(MMA_ADDR, REG_XHI, data, 6);
				for (i = 0; i < 6; i += 2){
					while(!QS_Enqueue(&SndBuff, data[i])){}
				}
				UART0->C2 |= UART0_C2_TIE_MASK; // send mask
			}
		} else {
				i2c_read_bytes(MMA_ADDR, REG_XHI, data, 6); // if accumulation it hands over data to special buffer
				acc_enqueue( &acc, data);
		}
	}
}

void configure_resolution(uint8_t res){
	switch(res){
		case 0:
			if (resolution != 2){
				resolution = 2;
				i2c_write_byte(MMA_ADDR, XYZ_DATA_CFG, 0x00);
				delay_mc(40);
			}
			resolution = 2;
			break;
		case 1:
			if (resolution != 4){
				resolution = 4;
				i2c_write_byte(MMA_ADDR, XYZ_DATA_CFG, 0x01);
				delay_mc(40);
			}
			resolution = 4;
			break;
		case 2:
			if (resolution != 8){
				if (lownoise == 0){
					resolution = 8;
					i2c_write_byte(MMA_ADDR, XYZ_DATA_CFG, 0x02);
					delay_mc(40);
				}
			}
			break;
		default:
			break;
	}
}

void configure_lownoise( uint8_t delay){
	if (lownoise == 1){
	if (resolution == 8){
		i2c_write_byte(MMA_ADDR, XYZ_DATA_CFG, 0x01);
		delay_mc(delay);
		resolution = 4;
	}
	i2c_write_byte(MMA_ADDR, REG_CTRL1, 0x04);
	delay_mc(delay);
	i2c_write_byte(MMA_ADDR, REG_CTRL1, 0x05);
	delay_mc(delay);
	} else {
		i2c_write_byte(MMA_ADDR, REG_CTRL1, 0x01);
		delay_mc(delay);
	}
}



void check_for_config(void){
	if ((started_receiving == 1) && (last_received > TIME_WAIT)){
		started_receiving = 0;
		uint8_t data[3];
		int i = 0;
		data[0] = 0;
		data[0] = QS_Dequeue( &RcvBuff);
		if (data[0] == CONFIG_START){
			QS_Init(&SndBuff);
			if (active_read == 1){
				i2c_write_byte(MMA_ADDR, REG_CTRL1, 0x00); // sensor is being turned off
				delay_mc(40);
			}
			for (i = 1; i < 2; i++){
				data[i] = 0;
			}
			data[1] = QS_Dequeue( &RcvBuff);
			data[2] = QS_Dequeue( &RcvBuff);
			for (i = 0; i < 3; i++){
				configuration[i] = data[i];
			}
			if ((data[1] & 0x01) == 1){
				lownoise = 1;
				if (resolution == 8){
					resolution = 4;
				}
			} else {
				lownoise = 0;
			}
			uint8_t resx = (data[1] & 0x06) >> 1;
			configure_resolution(resx);
			uint8_t inter = data[2];
			interval = inter * INTER_UNIT + INTER_UNIT;
			inter_counter = 0;
			uint8_t accx = (data[1] & 112) >> 4;
			acc_val = accx * ACC_UNIT + ACC_UNIT;
			acc_change_size( &acc, acc_val);
			if (((data[1] & 8) >> 3)==1){
				OnlyMSB = 0;
			} else {
				OnlyMSB = 1;
			}
			acc_change_mode(&acc, OnlyMSB);
			acc_en = data[1] >> 7;
			if (active_read == 1){
				configure_lownoise(40);
			}
			ledblueBlink(1,80);
			inter_counter = 0;
		} else if(data[0] == SHOW_CONF){
			QS_Init(&SndBuff);
			send_configuration();
			UART0->C2 |= UART0_C2_TIE_MASK; // allows to write over uart
		} else if(data[0] == STOP){
			if (active_read == 1){
				active_read = 0;
				i2c_write_byte(MMA_ADDR, REG_CTRL1, 0x00);
				ledredBlink(1,50);
				delay_mc(10);
			} else {
				active_read = 1;
				configure_lownoise(40);
				ledgreenBlink(1,50);
			}
		}
	}
}

int init_mma(){
	if (i2c_read_byte(MMA_ADDR, REG_WHOAMI) == WHOAMI){ // check device
		delay_mc(40);
		configure_lownoise(40);
		ledgreenBlink(3,50);
		return 1;	
	} else {
		ledredBlink(3,50);
		return 0;
	}
}

void i2c_write_byte(uint8_t dev, uint8_t reg, uint8_t data){
	I2C_TRAN;
	I2C_M_START;
	I2C0->D = dev;
	I2C_WAIT
	I2C0->D = reg;
	I2C_WAIT
	I2C0->D = data;
	I2C_WAIT
	I2C_M_STOP;
}


