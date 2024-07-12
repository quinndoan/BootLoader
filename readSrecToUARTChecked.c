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
    uint32_t i;
    for (i = 2; i < sizeForLine - 1; i += 2) {
        uint32_t convertTwoByte;
        StrtoHex(&line[i], 2, &convertTwoByte);

        checksum += convertTwoByte;
    }

    checksum = 0xFF - (checksum & 0xFF);

    return checksum;
}

SrecStatus_t ReadCheckLine(const char *line, struct Srec *line1) {
    if (line1 == NULL) {
        return BYTE_COUNT_ERROR;
    }

    uint8_t a1 = line[0];
    if (a1 != 'S') {
        return SYNTAX_ERROR;
    }

    uint8_t a2 = line[1];
    uint8_t a2Number = char_to_hex(a2);
    
    if (a2Number == HEX_INVALID) {
        return SYNTAX_ERROR;
    }

    if (0 <= a2Number && a2Number <= 9 && a2Number != 4) {
        if (a2Number == 1 || a2Number == 5 || a2Number == 9) {
            line1->sizeOfAddress = 4;
        } else if (a2Number == 2 || a2Number == 6 || a2Number == 8) {
            line1->sizeOfAddress = 6;
        } else {
            line1->sizeOfAddress = 8;
        }
    } else {
        return SYNTAX_ERROR;
    }

    char a3[3];
    memcpy(a3, &line[2], 2);
    a3[2] = '\0';
    uint32_t byteCount;
    SrecStatus_t byteCountStatus = StrtoHex(a3, 2, &byteCount);
    if (byteCountStatus != SREC_TRUE) {
        return byteCountStatus;
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
    SrecStatus_t addressStatus = StrtoHex(line1->Address, lengthAddress, &pAddress);
    if (addressStatus != SREC_TRUE) {
        free(line1->Address);
        return addressStatus;
    }

    uint32_t numberDataByte = (line1->ByteCount) - 1 - (line1->sizeOfAddress / 2);
    line1->sizeOfData = numberDataByte * 2;
    line1->Data = (char *)malloc(line1->sizeOfData + 1);
    if (line1->Data == NULL) {
        free(line1->Address);
        return BYTE_COUNT_ERROR;
    }
    memcpy(line1->Data, &line[4 + lengthAddress], line1->sizeOfData);
    line1->Data[line1->sizeOfData] = '\0';
    
    uint32_t j;
    for (j = 0; j < line1->sizeOfData; j++) {
        if (char_to_hex(line1->Data[j]) == HEX_INVALID) {
            free(line1->Address);
            free(line1->Data);
            return BYTE_COUNT_ERROR;
        }
    }

    char a6[3];
    strncpy(a6, &line[4 + lengthAddress + line1->sizeOfData], 2);
    a6[2] = '\0';
    uint32_t checkSum;
    SrecStatus_t checkSumStatus = StrtoHex(a6, 2, &checkSum);
    if (checkSumStatus != SREC_TRUE) {
        free(line1->Address);
        free(line1->Data);
        return checkSumStatus;
    }
    line1->checkSum = checkSum;

    uint32_t sizeForLine = line1->ByteCount * 2 + 2;
    char lineForCaculate[sizeForLine + 1];
    strncpy(lineForCaculate, line, sizeForLine);
    lineForCaculate[sizeForLine] = '\0';
    uint8_t checkSumCaculate = calculate_checksum(lineForCaculate, sizeForLine);
    
    if (checkSumCaculate != line1->checkSum) {
        free(line1->Address);
        free(line1->Data);
        return CHECK_SUM_ERROR;
    }

    return SREC_TRUE;
}

void send_uart(const char *message) {
    // Gi? l?p g?i thông báo qua UART
    printf("UART: %s\n", message);
}

struct Srec* parse_srec(const char *line) {
    struct Srec *record = (struct Srec *)malloc(sizeof(struct Srec));
    if (record == NULL) {
        send_uart("Memory allocation failed");
        return NULL;
    }

    SrecStatus_t status = ReadCheckLine(line, record);
    if (status != SREC_TRUE) {
        switch (status) {
            case CHECK_SUM_ERROR:
                send_uart("Checksum error");
                break;
            case SYNTAX_ERROR:
                send_uart("Syntax error");
                break;
            case BYTE_COUNT_ERROR:
                send_uart("Byte count error");
                break;
            default:
                send_uart("Unknown error");
                break;
        }
        free(record);
        return NULL;
    }

    return record;
}

int main() {
    const char *line = "S0100000626C696E6B4C65642E73726563EF";
    struct Srec *record = parse_srec(line);

    if (record != NULL) {
        printf("Record Type: S%c\n", line[1]);
        printf("Byte Count: %u\n", record->ByteCount);
        printf("Address: %s\n", record->Address);
        printf("Data: %s\n", record->Data);
        printf("Checksum: %u\n", record->checkSum);

        free(record->Address);
        free(record->Data);
        free(record);
    } else {
        printf("Failed to parse SREC line.\n");
    }

    return 0;
}

