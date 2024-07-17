/*
 * MOCK_Driver.c
 *
 *  Created on: Jul 11, 2024
 *      Author: Admin
 */
#include <MOCK_Driver.h>

static volatile Clear_t Clear_Flag_PORTA;
static volatile Clear_t Clear_Flag_PORTC;
static volatile Clear_t Clear_Flag_PORTD;
static uint32_t PROGRAM_START_ADDRESS = 0x20400;
static uint32_t count = 0;

extern volatile bool newData;
extern volatile uint8_t MODE;
extern volatile uint8_t rxBuffer[BUFFER_SIZE];
extern volatile uint8_t rxIndex;
extern volatile bool newLine;
extern volatile Queue srecQueue;
extern volatile bool eofReceived;

void Port_Innit(PORT_Config_Type *PIN_Info){
	switch ((uint32_t)PIN_Info->PORTx){
		case PORTA_BASE:{
			SIM->SCGC5 |= (1u << (SIM_SCGC5_PORTA_SHIFT));
			break;
		}
		case PORTB_BASE:{
			SIM->SCGC5 |= (1u << (SIM_SCGC5_PORTB_SHIFT));
			break;
		}
		case PORTC_BASE:{
			SIM->SCGC5 |= (1u << (SIM_SCGC5_PORTC_SHIFT));
			break;
		}
		case PORTD_BASE:{
			SIM->SCGC5 |= (1u << (SIM_SCGC5_PORTD_SHIFT));
			break;
		}
		case PORTE_BASE:{
			SIM->SCGC5 |= (1u << (SIM_SCGC5_PORTE_SHIFT));
			break;
		}
	}
	(PIN_Info->PORTx)->PCR[PIN_Info->PIN] &= ~(PORT_PCR_MUX_MASK);
	(PIN_Info->PORTx)->PCR[PIN_Info->PIN] |= (PORT_PCR_MUX(PIN_Info->MUX));

	//Config pull select
	(PIN_Info->PORTx)->PCR[PIN_Info->PIN] &= ~(3u);
	(PIN_Info->PORTx)->PCR[PIN_Info->PIN] |= (PIN_Info->PS);

	(PIN_Info->PORTx)->PCR[PIN_Info->PIN] &= ~(PORT_PCR_IRQC_MASK);
	(PIN_Info->PORTx)->PCR[PIN_Info->PIN] |= (PORT_PCR_IRQC(PIN_Info->Interupt));
	//if PORT has interrupt
	if(PIN_Info->Interupt != PORT_Interupt_Disable){
		//identify which PORT send interrupt signal
		if((PIN_Info->PORTx) == PORTA){
			//Get call back function address from upper layer
//			Interrupt_PORTA_Add = PIN_Info->CallBack;
			//Config NVIC peripheral
			NVIC->ISER[0] |= (1u << 30);
			//Clear flag if user want to clear after interrupt executed
			Clear_Flag_PORTA = PIN_Info->Clear_Flag;
		}
		else if((PIN_Info->PORTx) == PORTC ){
			//Get call back function address from upper layer
//			Interrupt_PORTCD_Add = PIN_Info->CallBack;
			//Config NVIC peripheral
			NVIC->ISER[0] |= (1u << 31);
			//Clear flag if user want to clear after interrupt executed
			Clear_Flag_PORTC = PIN_Info->Clear_Flag;
		}
		else if((PIN_Info->PORTx) == PORTD){
//			Interrupt_PORTCD_Add = PIN_Info->CallBack;
			NVIC->ISER[0] |= (1u << 31);
			Clear_Flag_PORTD = PIN_Info->Clear_Flag;
		}
	}
}

void GPIO_Innit(GPIO_Config_Type* GPIO_Config){
	if(GPIO_Config->Direction == Input){
		(GPIO_Config->GPIOx)->PDDR &= ~(1u << (GPIO_Config->PIN));
	}
	else if(GPIO_Config->Direction == OutPut_0){
		(GPIO_Config->GPIOx)->PDDR |= (1u << (GPIO_Config->PIN));
		(GPIO_Config->GPIOx)->PCOR = (1u << (GPIO_Config->PIN));
	}
	else if(GPIO_Config->Direction == OutPut_1){
		(GPIO_Config->GPIOx)->PDDR |= (1u << (GPIO_Config->PIN));
		(GPIO_Config->GPIOx)->PSOR = (1u << (GPIO_Config->PIN));
	}
}

void PORTC_PORTD_IRQHandler(void){
	//check if SW3 is holded for 5s
	count = 0;
	while(((GPIOC->PDIR & (1u << 12)) == 0)){
		count++;
		if(count > 1000000){
			//check if Reset Button is pressed
			if((GPIOC->PDIR & (1u << 3)) == 0){
			MODE = 1;
			//delete flag
			PORTC->ISFR = (1u << 12);
			return;
			}
		}
	}
}

void Innit_Button_SW3(){
	PORT_Config_Type SW3;
	SW3.PIN = 12;
	SW3.PORTx = PORTC;
	SW3.MUX = GPIO;
	SW3.PS = Pull_Up;
	SW3.Interupt = PORT_Interupt_Falling_Edge;
	SW3.Clear_Flag = Clear;
	Port_Innit(&SW3);

	PORT_Config_Type ResetSW;
	ResetSW.PIN = 3;
	ResetSW.PORTx = PORTC;
	ResetSW.MUX = GPIO;
	ResetSW.PS = Pull_Up;
	ResetSW.Interupt = PORT_Interupt_Disable;
	ResetSW.Clear_Flag = Clear;
	Port_Innit(&ResetSW);

	GPIO_Config_Type Button3;
	Button3.Direction = Input;
	Button3.GPIOx = GPIOC;
	Button3.PIN = 12;
	GPIO_Innit(&Button3);

	GPIO_Config_Type ResetButton;
	ResetButton.Direction = Input;
	ResetButton.GPIOx = GPIOC;
	ResetButton.PIN = 3;
	GPIO_Innit(&ResetButton);
}

void UART_Initialize() {

    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

    // Configure PTA1 as UART0_RX and PTA2 as UART0_TX
    PORTA->PCR[1] = PORT_PCR_MUX(2U);
    PORTA->PCR[2] = PORT_PCR_MUX(2U);

    // Enable pull-up resistors on PTA1 and PTA2
    PORTA->PCR[1] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PORTA->PCR[2] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

    // Disable UART0 transmitter and receiver before configuration
    UART0->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);

    // Configure MCGIRCLK as UART0 clock source
    MCG->C1 |= MCG_C1_IRCLKEN_MASK;
    MCG->C2 |= MCG_C2_IRCS_MASK;
    MCG->SC &= ~MCG_SC_FCRDIV_MASK;

    // Select MCGIRCLK as UART0 clock source
    SIM->SOPT2 |= SIM_SOPT2_UART0SRC(3U);


    uint16_t sbr = 26;
    UART0->BDH = (sbr >> 8) & UART0_BDH_SBR_MASK;
    UART0->BDL = sbr & UART0_BDL_SBR_MASK;

    // Configure oversampling ratio to 16
    UART0->C4 = (UART0->C4 & ~UART0_C4_OSR_MASK) | UART0_C4_OSR(15);

    // Enable both edge sampling
    UART0->C5 |= UART0_C5_BOTHEDGE_MASK;

    // Configure 8-bit data, no parity, 1 stop bit
    UART0->C1 &= ~(UART0_C1_M_MASK | UART0_C1_PE_MASK);
    UART0->BDH &= ~UART0_BDH_SBNS_MASK;

    // Enable UART0 transmitter and receiver
    UART0->C2 |= UART0_C2_TE_MASK | UART0_C2_RE_MASK;

    // Enable UART0 receive interrupt
    UART0->C2 |= UART0_C2_RIE_MASK;

    // Enable UART0 interrupt in NVIC
    NVIC_EnableIRQ(UART0_IRQn);
}

void UART_SendChar(char c) {
    // Wait for transmit data register empty flag
    while (!(UART0->S1 & UART0_S1_TDRE_MASK));
    // Send character
    UART0->D = c;
    // Wait for transmission complete flag
    while (!(UART0->S1 & UART0_S1_TC_MASK));
}

void UART_TransmitString(uint8_t *ptr, uint8_t length) {
    for (uint8_t i = 0; i < length; i++) {
        UART_SendChar(ptr[i]);
    }
}

void UART0_IRQHandler(void) {
    if (UART0->S1 & UART0_S1_RDRF_MASK) {
        uint8_t receivedChar = UART0->D;
        rxBuffer[rxIndex++] = receivedChar;
        if (rxIndex >= BUFFER_SIZE || receivedChar == '\n') {
            newData = true;
        }
    }
}

void Queue_Enqueue(Queue *q, const char *data, uint16_t length) {
    if (q->count < QUEUE_SIZE) {
        q->items[q->head].length = length;
        memcpy(q->items[q->head].data, data, length);
        q->items[q->head].data[length] = '\0';
        q->head = (q->head + 1) % QUEUE_SIZE;
        q->count++;
    }
}

bool Queue_Dequeue(Queue *q, QueueItem *item) {
    if (q->count == 0) {
        return false;
    } else {
        *item = q->items[q->tail];
        q->tail = (q->tail + 1) % QUEUE_SIZE;
        q->count--;
        return true;
    }
}


uint8_t char_to_hex(uint8_t c) {
    if ('0' <= c && c <= '9') {
        return c - '0';
    } else if ('A' <= c && c <= 'F') {
        return c - 'A' + 10;
    } else {
        return HEX_INVALID;
    }
}

uint8_t StrtoHex(const char *str, uint8_t len, uint32_t *pDecimal) {
    *pDecimal = 0;
    for (uint32_t idx = 0; idx < len; idx++) {
        uint8_t hex = char_to_hex(str[idx]);
        if (hex == HEX_INVALID) {
            return SYNTAX_ERROR;
        }
        *pDecimal = (*pDecimal << 4) | hex;
    }
    return SUCCESS;
}

uint8_t calculate_checksum(const char *line, uint32_t sizeForLine) {
    uint8_t checksum = 0;
    for (uint32_t i = 2; i < sizeForLine - 1; i += 2) {
        uint32_t convertTwoByte;
        StrtoHex(&line[i], 2, &convertTwoByte);
        checksum += convertTwoByte;
    }
    return ~checksum & 0xFF;
}

SrecStatus_t ReadCheckLine(const char *line, Srec *line1) {
    if (line1 == NULL) {
        return BYTE_COUNT_ERROR;
    }

    if (line[0] != 'S') {
        return SYNTAX_ERROR;
    }

    uint8_t a2Number = char_to_hex(line[1]);
    if (a2Number == HEX_INVALID) {
        return SYNTAX_ERROR;
    }

    switch (a2Number) {
        case 1: case 5: case 9:
            line1->sizeOfAddress = 4;
            break;
        case 2: case 6: case 8:
            line1->sizeOfAddress = 6;
            break;
        default:
            line1->sizeOfAddress = 8;
    }
    line1->recordType = a2Number;

    uint32_t byteCount;
    if (StrtoHex(line + 2, 2, &byteCount) != SUCCESS) {
        return SYNTAX_ERROR;
    }
    line1->ByteCount = byteCount;

    uint8_t lengthAddress = line1->sizeOfAddress;
    line1->Address = (char *)malloc(lengthAddress + 1);
    if (line1->Address == NULL) {
        return BYTE_COUNT_ERROR;
    }
    memcpy(line1->Address, &line[4], lengthAddress);
    line1->Address[lengthAddress] = '\0';

    uint32_t pAddress;
    if (StrtoHex(line1->Address, lengthAddress, &pAddress) != SUCCESS) {
        free(line1->Address);
        return SYNTAX_ERROR;
    }
    line1->numberAddress = pAddress;

    uint32_t numberDataByte = byteCount - 1 - (lengthAddress / 2);
    line1->sizeOfData = numberDataByte * 2;
    line1->Data = (char *)malloc(line1->sizeOfData + 1);
    if (line1->Data == NULL) {
        free(line1->Address);
        return BYTE_COUNT_ERROR;
    }
    memcpy(line1->Data, &line[4 + lengthAddress], line1->sizeOfData);
    line1->Data[line1->sizeOfData] = '\0';

    for (uint32_t j = 0; j < line1->sizeOfData; j++) {
        if (char_to_hex(line1->Data[j]) == HEX_INVALID) {
            free(line1->Address);
            free(line1->Data);
            return SYNTAX_ERROR;
        }
    }

    uint32_t checkSum;
    if (StrtoHex(line + 4 + lengthAddress + line1->sizeOfData, 2, &checkSum) != SUCCESS) {
        free(line1->Address);
        free(line1->Data);
        return SYNTAX_ERROR;
    }
    line1->checkSum = checkSum;

    uint32_t sizeForLine = byteCount * 2 + 2;
    char lineForCaculate[sizeForLine + 1];
    strncpy(lineForCaculate, line, sizeForLine);
    lineForCaculate[sizeForLine] = '\0';
    if (calculate_checksum(lineForCaculate, sizeForLine) != line1->checkSum) {
        free(line1->Address);
        free(line1->Data);
        return CHECK_SUM_ERROR;
    }

    return SREC_TRUE;
}

Srec* parse_srec(const char *line) {
    Srec *srec = (Srec *)malloc(sizeof(Srec));
    if (srec == NULL) {
        return NULL;
    }

    if (ReadCheckLine(line, srec) != SREC_TRUE) {
        free(srec);
        return NULL;
    }

    if (srec->recordType != 1 && srec->recordType != 2 && srec->recordType != 3) {
        free(srec);
        return NULL;
    }

    return srec;
}

void Program_Flash(uint32_t startAddress, char* data, uint32_t length) {

    uint32_t address = startAddress;
    uint32_t dataWord;
    uint32_t fullWords = length / 8;
    uint32_t remainingBytes = length % 8;
    char buffer[9] = {0};

    for (uint32_t i = 0; i < fullWords * 8; i += 8) {
        if (StrtoHex(&data[i], 8, &dataWord) == SUCCESS) {
            Program_LongWord_Command(address, dataWord);
            address += 4;
        }
    }

    if (remainingBytes > 0) {
        strncpy(buffer, &data[fullWords * 8], remainingBytes);
        memset(buffer + remainingBytes, '0', 8 - remainingBytes);
        buffer[8] = '\0';

        if (StrtoHex(buffer, 8, &dataWord) == SUCCESS) {
            Program_LongWord_Command(address, dataWord);
        }
    }
}


void Process_SREC(char *line) {
    Srec *srec = parse_srec(line);
    if (srec != NULL) {
        Program_Flash(srec->numberAddress, srec->Data, srec->sizeOfData);
        free(srec->Address);
        free(srec->Data);
        free(srec);
    }
}


void Program_LongWord_Command(uint32_t Address, uint32_t Data){
	//Wait previous command to finished
	while((FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == 0);

	//Check Error and Protection flags
	if(((FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) & FTFA_FSTAT_FPVIOL_MASK) != 0){
		FTFA->FSTAT = 0x30;
	}

	//Command
	FTFA->FCCOB0 = 0x06;

	//Address
	FTFA->FCCOB3 = (uint8_t)(Address & 0xFF);
	FTFA->FCCOB2 = (uint8_t)((Address & 0xFF00) >> 8);
	FTFA->FCCOB1 = (uint8_t)((Address & 0xFF0000) >> 16);


	FTFA->FCCOB4 = (uint8_t)Data;
	FTFA->FCCOB5 = (uint8_t)(Data >> 8);
	FTFA->FCCOB6 = (uint8_t)(Data >> 16);
	FTFA->FCCOB7 = (uint8_t)(Data >> 24);
	//Clear CCIF
	FTFA->FSTAT = 0x80;
}

void Erase_Flash_Sector(uint32_t Address){
	//Wait previous command to finished
	while((FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == 0);

	//Check Error and Protection flags
	if(((FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) & FTFA_FSTAT_FPVIOL_MASK) != 0){
		FTFA->FSTAT = 0x30;
	}

	//Command
	FTFA->FCCOB0 = 0x09;

	//Address
	FTFA->FCCOB3 = (uint8_t)(Address & 0xFF);
	FTFA->FCCOB2 = (uint8_t)((Address & 0xFF00) >> 8);
	FTFA->FCCOB1 = (uint8_t)((Address & 0xFF0000) >> 16);

	//Clear CCIF
	FTFA->FSTAT = 0x80;
}

void Set_Priority(uint8_t IRQ_Number, uint8_t Priority){
	if(Priority > 3){
		Priority = 3;
	}
	uint8_t N = IRQ_Number / 4;
	uint8_t M = IRQ_Number % 4;
	NVIC->IP[N] |= ( Priority << (6 + 8 * M));
}

void Jump_to_Firmware(){

	__disable_irq();
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;

	//Set msp
	__set_MSP(*((volatile uint32_t*)PROGRAM_START_ADDRESS));
	SCB->VTOR = (PROGRAM_START_ADDRESS);

	//Set pc
	void(*reset_handler)(void) = (void(*)(void))(*((volatile uint32_t *)(PROGRAM_START_ADDRESS + 4)));
	reset_handler();
}

bool rxBufferContainsEOF(volatile uint8_t* buffer, uint8_t length) {
    // Check if buffer contains a line starting with 'S8' or 'S9'
    for (int i = 0; i < length - 1; i++) {
        if (buffer[i] == 'S' && (buffer[i + 1] == '8' || buffer[i + 1] == '9')) {
            return true;
        }
    }
    return false;
}

// Interrupt handler or function where data writing completes
void on_data_write_complete() {
    eofReceived = true;
}


