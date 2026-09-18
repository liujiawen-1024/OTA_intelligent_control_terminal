#ifndef __FIFO_H
#define __FIFO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define NSize 256

typedef struct
{
    uint8_t data[NSize]; // 用数组作为队列的储存空间
    int32_t front;       // 指示队头位置
    int32_t rear;        // 指示队尾位置
} sequeue_t;
extern sequeue_t QUART3;

int8_t Queue_Init(sequeue_t *q);
uint16_t Queue_Size(sequeue_t *q);
int8_t Queue_IsEmpty(sequeue_t *q);
int8_t Enqueue(sequeue_t *q, uint8_t value);
int8_t Dequeue(sequeue_t *q, uint8_t *pvalue);
int8_t Enqueue_Bytes(sequeue_t *q, uint8_t *pvalue, uint32_t Size);
#endif
