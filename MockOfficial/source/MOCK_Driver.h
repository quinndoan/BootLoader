/*
 * MOCK_Driver.h
 *
 *  Created on: Jul 11, 2024
 *      Author: Admin
 */

#ifndef MOCK_DRIVER_H_
#define MOCK_DRIVER_H_

#include <library.h>

#define BUFFER_SIZE 256
#define QUEUE_SIZE 15
#define HEX_INVALID 255
#define SREC_ERROR 0
#define SUCCESS 1
//volatile uint8_t rxBuffer[BUFFER_SIZE];
//volatile uint8_t rxIndex = 0;
//volatile bool newLine = false;

typedef enum {
    SREC_TRUE = 0x0,
    CHECK_SUM_ERROR = 0x1,
    SYNTAX_ERROR = 0x2,
    BYTE_COUNT_ERROR = 0x3
} SrecStatus_t;

typedef struct Srec {
    uint8_t recordType;
    uint8_t sizeOfAddress;
    uint32_t ByteCount;
    uint32_t sizeOfData;
    uint32_t checkSum;
    uint32_t numberAddress; // dùng cho TH address 4 bytes, chuyển sẵn Address sang từ String sang Hex
    char *Address;
    char *Data;
    uint8_t checksum;
} Srec;

typedef struct {
    char data[BUFFER_SIZE];
    uint16_t length;
} QueueItem;

typedef struct {
    QueueItem items[QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
} Queue;


void UART_Initialize();
void UART_SendChar(char c);
void UART_TransmitString(uint8_t *ptr, uint8_t length);
void UART0_IRQHandler(void);
void Queue_Enqueue(Queue *q, const char *data, uint16_t length);
bool Queue_Dequeue(Queue *q, QueueItem *item);
void SendProcessedData(struct Srec *srecLine);



uint8_t char_to_hex(uint8_t c);
uint8_t StrtoHex(const char *str, uint8_t len, uint32_t *pDecimal);
uint8_t calculate_checksum(const char *line, uint32_t sizeForLine);
SrecStatus_t ReadCheckLine(const char *line, struct Srec *line1);
Srec* parse_srec(const char *line);
void Program_Flash(uint32_t startAddress, char* data, uint32_t length);
void Process_SREC(char *line);
void on_data_write_complete();

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


void Innit_Button_SW3();
void Jump_to_Firmware();
void Program_LongWord_Command(uint32_t Address, uint32_t Data);
void Erase_Flash_Sector(uint32_t Address);
void Set_Priority(uint8_t IRQ_Number, uint8_t Priority);
bool rxBufferContainsEOF(volatile uint8_t* buffer, uint8_t length);

#endif /* MOCK_DRIVER_H_ */
