#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>

#define HEX_INVALID 255

struct Srec{
    uint8_t recordType;
    uint8_t sizeOfAddress;
    uint32_t ByteCount;
    uint32_t sizeOfData;
    uint32_t checkSum;
    char *Address;
    char *Data;
    uint8_t checksum;
} ReadSrec;

uint8_t char_to_hex(uint8_t c) {
    uint8_t result;

    if ('0' <= c && c <= '9') {
        result = c - '0';
    } else if ('A' <= c && c <= 'F') {
        result = c - 'A' + 10;
    } else {
        result = HEX_INVALID;
    }

    return result;
}

uint32_t StrtoHex(const char *str, uint8_t len, uint32_t *pDecimal) {
    *pDecimal = 0;
    uint32_t idx;

    for (idx = 0; idx < len; idx++) {
        *pDecimal += (char_to_hex(str[idx])) * pow(16, len - 1 - idx);
    }

    return *pDecimal;
}

struct Srec* ReadCheckLine(const char *line) {
    struct Srec *line1 = (struct Srec *)malloc(sizeof(struct Srec));
    if (line1 == NULL) {
        printf("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    uint8_t a1 = line[0];    // a1 supposed to be 'S'

    uint8_t a2 = line[1];
    uint8_t a2Number = char_to_hex(a2);
    
    if (a2Number == 1 || a2Number == 5 || a2Number == 9) {
       line1->sizeOfAddress = 4;  // read 4 characters for Address
    } else if (a2Number == 2 || a2Number == 6 || a2Number == 8) {
        line1->sizeOfAddress = 6;  // read 6 characters for Address
    } else {
        line1->sizeOfAddress = 8;  // read 8 characters for Address
    }

    // Handle ByteCount
    char a3[3];
    strncpy(a3, &line[2], 2);
    a3[2] = '\0';
    uint32_t byteCount;
    StrtoHex(a3, 2, &byteCount);
    line1->ByteCount = byteCount;

    // Handle Address
    uint8_t lengthAddress = line1->sizeOfAddress;
    line1->Address = (char *)malloc(lengthAddress + 1);
    if (line1->Address == NULL) {
        printf("Memory allocation failed");
        free(line1);
        exit(EXIT_FAILURE);
    }
    strncpy(line1->Address, &line[4], lengthAddress);
    line1->Address[lengthAddress] = '\0';
    
    // Handle Data
    uint32_t numberDataByte = (line1->ByteCount) - 1 - (line1->sizeOfAddress / 2);
    line1->sizeOfData = numberDataByte * 2; // number of characters for Data
    line1->Data = (char *)malloc(line1->sizeOfData + 1);
    if (line1->Data == NULL) {
        printf("Memory allocation failed");
        free(line1->Address);
        free(line1);
        exit(EXIT_FAILURE);
    }
    strncpy(line1->Data, &line[4 + lengthAddress], line1->sizeOfData);
    line1->Data[line1->sizeOfData] = '\0';

    // Handle CheckSum
    char a6[3];
    strncpy(a6, &line[4 + lengthAddress + line1->sizeOfData], 2);
    a6[2] = '\0';
    uint32_t checkSum;
    StrtoHex(a6, 2, &checkSum);
    line1->checkSum = checkSum;

    return line1;
}

int main() {
    const char *line = "S1137A10A54B1A3F0000097BFF3E5F5C3F7B5D6F81";
    struct Srec *record = ReadCheckLine(line);

    printf("Record Type: S%c\n", line[1]);
    printf("Byte Count: %u\n", record->ByteCount);
    printf("Address: %s\n", record->Address);
    printf("Data: %s\n", record->Data);
    printf("Checksum: %u\n", record->checkSum);

    free(record->Address);
    free(record->Data);
    free(record);

    return 0;
}

