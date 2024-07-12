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
static uint32_t count = 0;

extern volatile uint8_t MODE;

static PORT_Config_Type UART0_PIN_TX = {
			.PORTx = PORTA,
			.PIN = 2,
			.MUX = Alternate2,
			.PS = Pull_Up,
		};
static PORT_Config_Type UART0_PIN_RX = {
			.PORTx = PORTA,
			.PIN = 1,
			.MUX = Alternate2,
			.PS = Pull_Up,
		};


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
	MODE = 1;
//	while(((GPIOC->PDIR & (1u << 12)) == 0)){
//		count++;
//		if(count > 5){
//			//check if Reset Button is pressed
//			if((GPIOA->PDIR & (1u << 20)) == 0){
//			MODE = 1;
//			break;
//			}
//		}
//	}
	PORTC->ISFR = (1u << 12);
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
	ResetSW.PIN = 20;
	ResetSW.PORTx = PORTA;
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
	ResetButton.GPIOx = GPIOA;
	ResetButton.PIN = 20;
	GPIO_Innit(&ResetButton);
}


void UART0_Innit(UART0_Innit_t *UART0_Info){
	//Enable Clock for UART0
	SIM->SCGC4 |= (SIM_SCGC4_UART0(1u));

	//Choose clock source for UART0
	SIM->SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(3u);			//MCGIRCLK
	MCG->C1 |= MCG_C1_IRCLKEN(1u);					//Clock_Gate
	MCG->C2 |= MCG_C2_IRCS(1u);						//Fast internal reference clock selected.
	MCG->SC &= ~(MCG_SC_FCRDIV_MASK);				//FRDIV = 1
	//PORT MUX UART0
	Port_Innit(&UART0_PIN_TX);
	Port_Innit(&UART0_PIN_RX);

	// disable TE, RE to write UART registers
	UART0->C2 &= ~ UART0_C2_TE_MASK;
	UART0->C2 &= ~UART0_C2_RE_MASK;

	//Number of stop bits
	UART0->BDH |= (UART_BDH_SBNS(UART0_Info->UART0_Stop_Bit));

	//Baud Rate Divisor
	uint16_t SBR = 4000000/(UART0_Info->Baud_Rate * 16);
	UART0->BDH &= ~(UART_BDH_SBR_MASK);
	UART0->BDH |= (UART_BDH_SBR(SBR >> 8));

	UART0->BDL &= ~(UART0_BDL_SBR_MASK);
	UART0->BDL |= (UART0_BDL_SBR(SBR));

	//Data Length
	UART0->C1 |= (UART0_C1_M(UART0_Info->Data_Length));

	//Parity
	UART0->C1 |= (UART0_Info->Parity);

	//Interrupt
	if(UART0_Info->UART0_Interrupt == Interrupt){
		if(UART0_Info->Direction == Receiver){
			UART0->C2 |= (UART0_C2_RIE(1u));
		}
		else{
			UART0->C2 |= (UART0_C2_TIE(1u));
		}
		NVIC->ISER[0] |= (1u << 12);
	}

	//Transmit order
	UART0->S2 |= (UART0_Info->Transmit_Order);

	//Sampling Ratio
	UART0->C4 &= ~(UART0_C4_OSR_MASK);
	UART0->C4 |= (UART0_C4_OSR(15u));

	//Direction
	if(UART0_Info->Direction == Receiver){
		UART0->C2 |= (UART0_C2_RE(1u));
	}
	else if(UART0_Info->Direction == Transmitter){
		UART0->C2 |= (UART0_C2_TE(1u));
	}
}

void UART0_SendChar(uint8_t Char){
	//Enable Transmitter
	UART0->C2 |= (UART0_C2_TE(1u));
	while((UART0->S1 & (1 << 7)) == 0);

	UART0->D = Char;

	while((UART0->S1 & (1 << 6)) == 0);

	//Disable Transmitter
	UART0->C2 &= ~(UART0_C2_TE_MASK);
}

void UART0_SendStr(uint8_t *ptr, uint8_t len){
	//Enable Transmitter
	uint8_t i = 0;
	UART0->C2 |= (UART0_C2_TE(1u));
	for( i = 0; i < len; i++){
		while((UART0->S1 & (1 << 7)) == 0);
		UART0->D = ptr[i];
		while((UART0->S1 & (1 << 6)) == 0);
	}
	//Disable Transmitter
	UART0->C2 &= ~(UART0_C2_TE_MASK);
}

void UART_Data_To_PC_Innit(){
	UART0_Innit_t UART0_Info;
	UART0_Info.UART0_Stop_Bit = 1u;
	UART0_Info.Baud_Rate = 9600;
	UART0_Info.Data_Length = _8_bits;
	UART0_Info.Parity = No_Parity;
	UART0_Info.UART0_Interrupt = No_Interrupt;
	UART0_Info.Direction = Transmitter;
	UART0_Info.Transmit_Order = LSB;
	UART0_Innit(&UART0_Info);
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

	//Data
	uint32_t *ptr = (uint32_t *)0x40020008;
	*ptr = Data;

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


