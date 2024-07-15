#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define BUFFER_SIZE 256
#define QUEUE_SIZE 15

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

Queue srecQueue = {{0}, 0, 0, 0};

void Queue_Enqueue(Queue *q, const char *data, uint16_t length) {
    if (q->count < QUEUE_SIZE) {
        q->items[q->head].length = length;
        memcpy(q->items[q->head].data, data, length);
        q->items[q->head].data[length] = '\0'; // Null terminate for safety
        q->head = (q->head + 1) % QUEUE_SIZE;
        q->count++;
    } else {
        printf("Queue is full!\n");
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


void test_enqueue_and_split(FILE *fileptr) {
    char line[BUFFER_SIZE];

    // �?c t?ng d�ng t? t?p v� d?y v�o h�ng d?i
    while (fgets(line, sizeof(line), fileptr)) {
        size_t len = strlen(line);
        // Lo?i b? k� t? xu?ng d�ng n?u c�
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }
        Queue_Enqueue(&srecQueue, line, len);
    }

    // Hi?n th? c�c d�ng d� du?c d?y v�o h�ng d?i
    printf("Queue content:\n");
    QueueItem item;
    while (Queue_Dequeue(&srecQueue, &item)) {
        printf("Dequeued item: %s\n", item.data);
    }
}

int main(void) {
    // M? t?p d? d?c
    FILE *fileptr = fopen("D:\\Bootloader\\BootLoader\\blinkLed.srec", "r");
    if (fileptr == NULL) {
        printf("Cannot open file\n");
    }

    // G?i h�m d? d?c t? t?p v� d?y v�o h�ng d?i
    test_enqueue_and_split(fileptr);

    // ��ng t?p sau khi ho�n th�nh
    fclose(fileptr);

    return 0;
}


