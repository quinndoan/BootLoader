#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define BUFFER_SIZE 128
#define QUEUE_SIZE 10

typedef struct {
    char data[BUFFER_SIZE];
    uint16_t length;
} QueueItem;

typedef struct {
    QueueItem items[QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
} Queue;

void Queue_Enqueue(Queue *q, char *data, uint16_t length) {
    if (q->count < QUEUE_SIZE) {
        q->items[q->head].length = length;
        memcpy(q->items[q->head].data, data, length);
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

// Mock UART_TransmitString function
void UART_TransmitString(uint8_t *buffer, int length) {
    // Just print the buffer to stdout for testing
    printf("%.*s", length, buffer);
}

// Mock parse_srec function
struct Srec {
    uint8_t recordType;
    uint32_t ByteCount;
    char *Address;
    char *Data;
    uint8_t checkSum;
};

struct Srec* parse_srec(const char *line) {
    struct Srec *srec = (struct Srec *)malloc(sizeof(struct Srec));
    if (srec == NULL) {
        return NULL;
    }

    // Mock parsing logic
    if (line[0] == 'S') {
        srec->recordType = 1;
        srec->ByteCount = 16;
        srec->Address = strdup("1234");
        srec->Data = strdup("DEADBEEF");
        srec->checkSum = 0x5A; // Mock checksum
    } else {
        free(srec);
        return NULL;
    }

    return srec;
}

void SendProcessedData(struct Srec *srecLine) {
    char buffer[BUFFER_SIZE];
    int length;

    // Format the SREC data into a readable string
    length = snprintf(buffer, BUFFER_SIZE,
                      "Record Type: S%d\n"
                      "Byte Count: %lu\n"
                      "Address: %s\n"
                      "Data: %s\n"
                      "Checksum: %02X\n\n",
                      srecLine->recordType,
                      srecLine->ByteCount,
                      srecLine->Address,
                      srecLine->Data,
                      srecLine->checkSum);

    // Transmit the formatted string to the PC
    UART_TransmitString((uint8_t *)buffer, length);
}

void Process_SREC(Queue *q) {
    QueueItem item;
    while (Queue_Dequeue(q, &item)) {
        struct Srec *parsedSrec = parse_srec(item.data);
        if (parsedSrec != NULL) {
            SendProcessedData(parsedSrec);
            free(parsedSrec->Address);
            free(parsedSrec->Data);
            free(parsedSrec);
        } else {
            uint8_t Message1[] = "SREC Error\r\n";
            UART_TransmitString(Message1, sizeof(Message1) - 1);
        }
    }
}

void test_Process_SREC() {
    Queue srecQueue = {0};

    // Mock SREC lines
    const char *testLines[] = {
        "S113280000600020D528000043290000F92D0000B5",
        "S11328404F290000572900005F2900006729000074",
        "InvalidLine"
    };

    // Enqueue test lines
    int i;
    for (i = 0; i < 3; ++i) {
        Queue_Enqueue(&srecQueue, (char *)testLines[i], strlen(testLines[i]));
    }

    // Process the queue
    Process_SREC(&srecQueue);
}

int main() {
    test_Process_SREC();
    return 0;
}

