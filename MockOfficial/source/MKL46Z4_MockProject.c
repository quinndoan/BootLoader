#include <library.h>
#include <MOCK_Driver.h>

volatile uint8_t MODE  =  2;
static uint8_t Status1 = 0;
static uint8_t Status2 = 0;
volatile uint8_t rxIndex = 0;
volatile bool newData = false;
volatile uint8_t rxBuffer[BUFFER_SIZE];
volatile Queue srecQueue = {{0}, 0, 0, 0};
volatile bool eofReceived = false;

void main() {
    UART_Initialize();
    Innit_Button_SW3();
    Set_Priority(12, 1);
    Erase_Flash_Sector(0x020400);
    Erase_Flash_Sector(0x020800);
    Erase_Flash_Sector(0x020C00);
    Erase_Flash_Sector(0x021000);

    while (1) {
        if (MODE == 2) {
            // check data in flash
            uint32_t *ptr = (uint32_t *)0x20400;
            if (*ptr == 0xFFFFFFFF) {
                if (Status1 == 0) {
                    UART_TransmitString((uint8_t *)"NO DATA\n", 8);
                    Status1 = 1;
                }
            } else {
                // jump to Application
                Jump_to_Firmware();
            }
        } else if (MODE == 1) {
            // jump to load APP
            if (Status2 == 0) {
                UART_TransmitString((uint8_t *)"LOAD DATA\n", 10);
                Status2 = 1;
            }
            while (1) {
                if (newData) {
                    newData = false;
                    // Add received data to queue
                    Queue_Enqueue(&srecQueue, (char *)rxBuffer, rxIndex);
                    if (rxBufferContainsEOF(rxBuffer, rxIndex)) {
                        eofReceived = true;
                    }
                    rxIndex = 0; // Reset buffer index
                }

                QueueItem item;
                if (Queue_Dequeue(&srecQueue, &item)) {
                    Process_SREC(item.data);
                } else {
                    // If EOF is received, switch back to mode 2
                    if (eofReceived) {
                        MODE = 2;
                        eofReceived = false; // Reset flag for future operations
                        break;
                    }
                }
            }
        }
    }
}
