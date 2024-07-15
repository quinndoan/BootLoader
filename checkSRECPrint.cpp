#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define HEX_INVALID 255

typedef enum {
    SREC_TRUE = 0x0,
    CHECK_SUM_ERROR = 0x1,
    SYNTAX_ERROR = 0x2,
    BYTE_COUNT_ERROR = 0x3
} SrecStatus_t;

typedef struct {
    uint8_t recordType;
    uint8_t sizeOfAddress;
    uint32_t ByteCount;
    uint32_t sizeOfData;
    uint32_t checkSum;
    char *Address;
    char *Data;
} Srec;

uint8_t char_to_hex(uint8_t c) {
    if ('0' <= c && c <= '9') {
        return c - '0';
    } else if ('A' <= c && c <= 'F') {
        return c - 'A' + 10;
    } else {
        return HEX_INVALID;
    }
}

SrecStatus_t StrtoHex(const char *str, uint8_t len, uint32_t *pDecimal) {
    if (pDecimal == NULL) {
        return BYTE_COUNT_ERROR;
    }

    *pDecimal = 0;
    for (uint32_t idx = 0; idx < len; idx++) {
        uint8_t hex = char_to_hex(str[idx]);
        if (hex == HEX_INVALID) {
            return SYNTAX_ERROR;
        }
        *pDecimal = (*pDecimal << 4) | hex;
    }

    return SREC_TRUE;
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

    uint8_t a2 = line[1];
    uint8_t a2Number = char_to_hex(a2);
    if (a2Number == HEX_INVALID) {
        return SYNTAX_ERROR;
    }

    switch (a2Number) {
        case 1:
        case 5:
        case 9:
            line1->sizeOfAddress = 4;
            break;
        case 2:
        case 6:
        case 8:
            line1->sizeOfAddress = 6;
            break;
        case 3:
        case 7:
            line1->sizeOfAddress = 8;
            break;
        default:
            return SYNTAX_ERROR;
    }
    line1->recordType = a2Number;

    uint32_t byteCount;
    SrecStatus_t byteCountStatus = StrtoHex(line + 2, 2, &byteCount);
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

    for (uint32_t j = 0; j < line1->sizeOfData; j++) {
        if (char_to_hex(line1->Data[j]) == HEX_INVALID) {
            free(line1->Address);
            free(line1->Data);
            return BYTE_COUNT_ERROR;
        }
    }

    uint32_t checkSum;
    SrecStatus_t checkSumStatus = StrtoHex(line + 4 + lengthAddress + line1->sizeOfData, 2, &checkSum);
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

Srec* parse_srec(const char *line) {
    Srec *srec = (Srec *)malloc(sizeof(Srec));
    if (srec == NULL) {
        return NULL;
    }

    SrecStatus_t status = ReadCheckLine(line, srec);
    if (status != SREC_TRUE) {
        free(srec);
        return NULL;
    }
    if ((srec->recordType != 1) & (srec->recordType != 2) & (srec->recordType != 3)){
        free(srec);
        return NULL;
    }

    return srec;
}

void print_srec(Srec *srec) {
    if (srec == NULL) {
        printf("NULL Srec structure.\n");
        return;
    }
    printf("Record Type: S%d\n", srec->recordType);
    printf("Byte Count: %u\n", srec->ByteCount);
    printf("Address: %s\n", srec->Address);
    printf("Data: %s\n", srec->Data);
    printf("Checksum: %u\n", srec->checkSum);
}

void read_and_print_srec_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline character if present
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        Srec *srec = parse_srec(line);
        if (srec != NULL) {
            print_srec(srec);
            free(srec->Address);
            free(srec->Data);
            free(srec);
        } else {
            printf("Invalid SREC line: %s\n", line);
        }
    }

    fclose(file);
}

int main() {
    const char *filename = "D:\\Bootloader\\BootLoader\\blinkLed.srec";
    read_and_print_srec_file(filename);
    return 0;
}

