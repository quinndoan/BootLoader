
#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"

#define BUFFER_SIZE 256
#define QUEUE_SIZE 10

typedef struct {
    uint8_t data[BUFFER_SIZE];
    uint16_t length;
} QueueItem;

typedef struct {
    QueueItem items[QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
} Queue;

Queue srecQueue = {{0}, 0, 0, 0};

void Queue_Enqueue(Queue *q, char *data, uint16_t length);
bool Queue_Dequeue(Queue *q, QueueItem *item);

void test_queue();

int main(void) {
    // Call test function
    test_queue();

    return 0;
}

void Queue_Enqueue(Queue *q, char *data, uint16_t length) {
    if (q->count < QUEUE_SIZE) {
        q->items[q->head].length = length;
        uint16_t i=0;
        for (i = 0; i < length; i++) {
            q->items[q->head].data[i] = data[i];
        }
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

void test_queue() {
    const char *srecLines[] = {
        "S0100000626C696E6B4C65642E73726563EF\n",
        "S1130000285F245F2212226A000424290008237C2A\n",
        "S11300100002000800082629001853812341001813\n",
        "S11300200002000800082629001853812341001813\n",
        "S1070030000200080008262B\n",
    };
    const int numLines = sizeof(srecLines) / sizeof(srecLines[0]);

    printf("Enqueue SREC lines:\n");
    int i=0;
    for (i = 0; i < numLines; i++) {
        Queue_Enqueue(&srecQueue, (uint8_t *)srecLines[i], strlen(srecLines[i]));
        printf("%s", srecLines[i]);
    }

    printf("\nDequeuing SREC lines:\n");
    QueueItem item;
    while (Queue_Dequeue(&srecQueue, &item)) {
    	int j=0;
        for (j = 0; j < item.length; j++) {
            printf("%c", item.data[j]);
        }
    }
}
