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
    for (i= 0; i < len; i++) {
        uint8_t hex = char_to_hex(str[i]);
        *pDecimal = (*pDecimal << 4) | hex;
    }
}

// Hàm main d? test
int main() {
    // D? li?u gi? l?p t? chu?i SREC
    const char *hexData = "48656C6C6F20576F726C642121"; // "Hello World!!" trong hex
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

    // Ghi d? li?u vào flash t?i cùng m?t d?a ch?, t?ng kh?i 4 byte
    uint32_t numBytes = sizeOfData / 2;
    uint32_t numWords = (numBytes + 3) / 4;  // S? kh?i 4 byte
    uint8_t buffer[4] = {0};  // B? d?m 4 byte

    for (i = 0; i < numWords; i++) {
        // Làm r?ng b? d?m
        memset(buffer, 0, sizeof(buffer));
        // Sao chép d? li?u vào b? d?m
        uint32_t j;
        for (j = 0; j < 4 && (i * 4 + j) < numBytes; j++) {
            buffer[j] = data[i * 4 + j];
        }
        // K?t h?p 4 byte thành m?t t? 32 bit
        uint32_t word = 0;
        for (j = 0; j < 4; j++) {
            word |= buffer[j] << (8 * j);
        }
        // Ghi t? vào flash
        Program_LongWord_Command(address, word);  // Ghi vào cùng m?t d?a ch?
    }

    return 0;
}



