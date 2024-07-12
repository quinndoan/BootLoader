#include <library.h>
#include <MOCK_Driver.h>

volatile uint8_t MODE  =  2;
uint32_t Address = 0x2800;
static uint8_t Status1 = 0;
static uint8_t Status2 = 0;
void main(){
	UART_Data_To_PC_Innit();
	Innit_Button_SW3();
	while(1){
		if(MODE == 2){
			// check data in flash
			uint32_t *ptr = (uint32_t *)0x2800;
			if(*ptr == 0xFFFFFFFF){
				if(Status1 == 0){
					UART0_SendStr((uint8_t *)"NO DATA\n", 8);
					Status1 = 1;
				}
			}
			else{
				//jump to App

			}
		}
		else if(MODE == 1){
			//jump to load APP
			if(Status2 == 0){
				UART0_SendStr((uint8_t *)"LOAD DATA\n", 10);
				Status2 = 1;
			}
		}

			//check if mode
			/*Mode 1 : SW pressed
			 *  jump to Load-App
			 *  after Load succesfull -> change to mode 2 (app mode)
			 *
			 */

			/*Mode 2 : App mode
			 * read data in flash memory (use uint32_t *ptr = 0x2800(sector 10))
			 * if have data -> jump to app (sp = *0x2800)
			 * if not  -> Send to PC string "No Data availble"
			 *
			 */
	}
}
