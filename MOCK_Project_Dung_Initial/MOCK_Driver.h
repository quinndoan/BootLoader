/*
 * MOCK_Driver.h
 *
 *  Created on: Jul 11, 2024
 *      Author: Admin
 */

#ifndef MOCK_DRIVER_H_
#define MOCK_DRIVER_H_

#include <library.h>

typedef enum{
	Analog,				//0
	GPIO,				//1
	Alternate2,			//2
	Alternate3,			//3
	Alternate4,			//4
	Alternate5,			//5
	Alternate6,			//6
	Alternate7,			//7
}MUX_t;

typedef enum{
	Pull_Disable	= 0,
	Pull_Down = 2,
	Pull_Up = 3,
}PS_t;

typedef enum{
	PORT_Interupt_Disable,
	PORT_Interupt_Logic_0 = 0b1000,
	PORT_Interupt_Rising_Edge = 0b1001,
	PORT_Interupt_Falling_Edge = 0b1010,
	PORT_Interupt_Either_Edge = 0b1011,
	PORT_Interupt_Logic_1 = 0b1100,
}Interupt_t;

typedef enum{
	No_Clear,
	Clear,
}Clear_t;

typedef struct{
	PORT_Type * PORTx;
	uint8_t PIN;
	MUX_t MUX;
	PS_t PS;
	Interupt_t Interupt;
	Clear_t Clear_Flag;
}PORT_Config_Type;

typedef enum{
	Input, 		//0
	OutPut_0,	//1
	OutPut_1,	//2
}Direction_t;

typedef struct{
	GPIO_Type *GPIOx;
	uint8_t PIN;
	Direction_t Direction;
}GPIO_Config_Type;

typedef enum{
	_8_bits,
	_9_bits,
}UART0_Data_Length;

typedef enum{
	No_Parity = 1,
	Even_Parity = 2,
	Odd_Parity = 3,
}Parity_t;

typedef enum{
	LSB,
	MSB,
}Transmit_Order_t;

typedef enum{
	No_Interrupt,
	Interrupt,
}UART0_Interrupt_t;

typedef enum{
	Receiver,
	Transmitter,
}UART0_Direction_t;

typedef struct{
	uint8_t UART0_Stop_Bit;
	uint16_t Baud_Rate;
	UART0_Data_Length Data_Length;
	UART0_Interrupt_t UART0_Interrupt;
	UART0_Direction_t Direction;
	Parity_t Parity;
	Transmit_Order_t Transmit_Order;
}UART0_Innit_t;



void Innit_Button_SW3();
void UART_Data_To_PC_Innit();
void UART0_SendStr(uint8_t *ptr, uint8_t len);
void UART0_SendChar(uint8_t Char);
void Program_LongWord_Command(uint32_t Address, uint32_t Data);
void Erase_Flash_Sector(uint32_t Address);

#endif /* MOCK_DRIVER_H_ */
