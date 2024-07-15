#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Gi? l?p hàm Program_LongWord_Command
void Program_LongWord_Command(uint32_t Address, uint32_t Data) {
    printf("Writing to Flash: Address = 0x%08X, Data = 0x%08X\n", Address, Data);
}

// Chuy?n d?i ký t? hex thành giá tr? s?
uint8_t char_to_hex(uint8_t c) {
    if ('0' <= c && c <= '9') {
        return c - '0';
    } else if ('A' <= c && c <= 'F') {
        return c - 'A' + 10;
    } else if ('a' <= c && c <= 'f') {
        return c - 'a' + 10;
    } else {
        return 0xFF; // Giá tr? không h?p l?
    }
}

// Chuy?n d?i chu?i hex thành s? th?p phân
void StrtoHex(const char *str, uint32_t len, uint32_t *pDecimal) {
    *pDecimal = 0;
    uint32_t i;
    for (i = 0; i < len; i++) {
        uint8_t hex = char_to_hex(str[i]);
        *pDecimal = (*pDecimal << 4) | hex;
    }
}

// Hàm main d? test
int main() {
    // D? li?u gi? l?p t? chu?i SREC
    const char *hexData = "48656C6C6F20576F726C64"; // "Hello World" trong hex
    uint32_t sizeOfData = strlen(hexData);
    uint8_t data[sizeOfData / 2];
    
    // Chuy?n d?i chu?i hex thành m?ng byte
    uint32_t i;
    for (i = 0; i < sizeOfData; i += 2) {
        uint32_t byte;
        StrtoHex(hexData + i, 2, &byte);
        data[i / 2] = (uint8_t)byte;
    }

    // Ð?a ch? gi? l?p d? ghi vào Flash
    uint32_t address = 0x10000000;

    // Ghi d? li?u vào flash theo t?ng kh?i 4 byte t?i cùng m?t d?a ch?
    uint32_t m;
    for (m = 0; m < sizeOfData / 2; m += 4) {
        uint32_t word = 0;
        uint32_t j;
        for (j = 0; j < 4 && (m + j) < sizeOfData / 2; j++) {
            word |= data[m + j] << (8 * j);
        }
        Program_LongWord_Command(address, word);  // Ghi vào cùng m?t d?a ch?
    }

    return 0;
}


