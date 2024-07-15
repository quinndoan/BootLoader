#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>

#define HEX_INVALID 255

typedef enum {
    SREC_TRUE = 0x0,
    CHECK_SUM_ERROR = 0x1,
    SYNTAX_ERROR = 0x2,
    BYTE_COUNT_ERROR = 0x3
} SrecStatus_t;

struct Srec{
    uint8_t recordType;
    uint8_t sizeOfAddress;
    uint32_t ByteCount;
    uint32_t sizeOfData;
    uint32_t checkSum;
    char *Address;
    char *Data;
    uint8_t checksum;
    SrecStatus_t type;
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

SrecStatus_t StrtoHex(const char *str, uint8_t len, uint32_t *pDecimal) {
    SrecStatus_t result = SREC_TRUE;

    if (pDecimal == NULL) {
        return BYTE_COUNT_ERROR;
    }

    *pDecimal = 0;

    uint32_t idx;

    for (idx = 0; idx < len; idx++) {
        if (char_to_hex(str[idx]) == HEX_INVALID) {
            result = SYNTAX_ERROR;
            break;
        }
        *pDecimal += (char_to_hex(str[idx])) * pow(16, len - 1 - idx);
    }

    return result;
}

uint8_t calculate_checksum(const char *line, uint32_t sizeForLine) {
    uint8_t checksum = 0;
    uint32_t length = sizeForLine;
    
    uint32_t i;
    for (i = 2; i < length - 1; i += 2) {
        uint32_t convertTwoByte;
        StrtoHex(&line[i], 2, &convertTwoByte);

        // Add the value to checksum
        checksum += convertTwoByte;
      //  checksum += hexValue;
    }

    // Calculate the final checksum value
    checksum = 0xFF - (checksum & 0xFF);

    return checksum;
}


struct Srec* ReadCheckLine(const char *line) {
    struct Srec *line1 = (struct Srec *)malloc(sizeof(struct Srec));
    if (line1 == NULL) {
        printf("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    uint8_t a1 = line[0];
    if (a1 == 'S') {
        line1->type = SREC_TRUE;  // true
    } else {
        line1->type = SYNTAX_ERROR;
    }

    uint8_t a2 = line[1];
    uint8_t a2Number = char_to_hex(a2);
    
    if (a2Number == HEX_INVALID) {
        line1->type = SYNTAX_ERROR; // syntax error
    } else if (0 <= a2Number && a2Number <= 9 && a2Number != 4) {   // xoa phan hoac cua hexa
        line1->type = SREC_TRUE;  // true
        if (a2Number == 1 || a2Number == 5 || a2Number == 9) {
            line1->sizeOfAddress = 4;  // doc 4 ky tu cho Address
        } else if (a2Number == 2 || a2Number == 6 || a2Number == 8) {
            line1->sizeOfAddress = 6;  // doc 6 ky tu cho Address
        } else {
            line1->sizeOfAddress = 8;  // doc 8 ky tu cho Address
        }
    }

    // Handle ByteCount
    char a3[3];
    memcpy(a3, &line[2], 2);
    a3[2] = '\0';
    uint32_t byteCount;
	SrecStatus_t byteCountStatus = StrtoHex(a3, 2, &byteCount);
    if (byteCountStatus != SREC_TRUE) {
        line1->type = byteCountStatus;
    }
    line1->ByteCount = byteCount;


    // Handle Address
    uint8_t lengthAddress = line1->sizeOfAddress;
    line1->Address = (char *)malloc(lengthAddress + 1);
    if (line1->Address == NULL) {
        printf("Memory allocation failed");
        free(line1);
        exit(EXIT_FAILURE);
    }
    memcpy(line1->Address, &line[4], lengthAddress);
    line1->Address[lengthAddress] = '\0';
    
    // Check if the address field has the right hex type
    uint32_t pAddress;
    line1->type = StrtoHex(a4, lengthAddress, &pAddress);
    
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
    memcpy(line1->Data, &line[4 + lengthAddress], line1->sizeOfData);
    line1->Data[line1->sizeOfData] = '\0';
    
    // check the data length
    uint32_t i;
    for (i = 0; i < line1->sizeOfData; i++) {
        if (char_to_hex(a5[i]) == HEX_INVALID) {
            line1->type = BYTE_COUNT_ERROR;
        }
    }

    // Handle CheckSum
    char a6[3];
    strncpy(a6, &line[4 + lengthAddress - 1 + sizeForA5 - 1], 2);
    a6[2] = '\0';
    uint32_t checkSum;
    SrecStatus_t checkSumStatus = StrtoHex(a6, 2, &checkSum);
    line1->type = checkSumStatus;
    line1->checkSum = checkSum;

    if (line1->type == SREC_TRUE) {
        // check the value of CheckSum
        uint32_t sizeForLine = line1->ByteCount * 2 + 2;
        char lineForCaculate[sizeForLine + 1];
        strncpy(lineForCaculate, line, sizeForLine);
        lineForCaculate[sizeForLine] = '\0';
        uint8_t checkSumCaculate = calculate_checksum(lineForCaculate, sizeForLine);
    
       if (checkSumCaculate != line1->checkSum) {
            line1->type = CHECK_SUM_ERROR;
        }
    }

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

