#include "fifo.h"
#include <stdlib.h>

// 循环队列关于满和空的理解
// 空 : 队头和队尾相等就是空
// 满 : 数组元素个数减一

/*
 * 为区别空队和满队，满队元素个数比数组元素个数少一个。
 * 满队 队尾+1 == 队头 ,数组空了一个元素, 因此满队比数组少一个元素
 */

sequeue_t QUART3; // 单片机中尽量不使用malloc函数,
int8_t Queue_Init(sequeue_t *q)
{
    q->front = q->rear = NSize - 1; // 指向数组的最后一个元素 , 原因入队队尾加, 出队队头加
    memset(q->data, 0, NSize);
    return 0;
}

uint16_t Queue_Size(sequeue_t *q)
{
    if (q->rear >= q->front)
    {
        return q->rear - q->front;
    }
    else
    {
        return NSize - q->front + q->rear;
    }
}

// 空队 队头等于队尾
int8_t Queue_IsEmpty(sequeue_t *q)
{
    return (q->front == q->rear);
}

int Queue_IsFull(sequeue_t *q)
{
    // 队尾 +1 == 队头 , 就是满队
    // 任何一个一个数对N求余 , 这个数的范围是  0 - N-1
    return ((q->rear + 1) % NSize == q->front);
}

// 入队  , 队尾加
int8_t Enqueue(sequeue_t *q, uint8_t value)
{
    if (Queue_IsFull(q))
    {
        printf("queue is full\n");
        return -1;
    }
    q->rear = (q->rear + 1) % NSize; // 数组下标的移动范围 0 到N-1
    q->data[q->rear] = value;
    return 0;
}

// 出队 , 队头加
int8_t Dequeue(sequeue_t *q, uint8_t *pvalue)
{
    if (Queue_IsEmpty(q))
    {
        // printf("empty\n");
        return -1;
    }
    q->front = (q->front + 1) % NSize; // 数组下标的移动范围 0 到N-1
    *pvalue = q->data[q->front];
    return 0;
}

int8_t Enqueue_Bytes(sequeue_t *q, uint8_t *pvalue, uint32_t Size)
{
    for (uint16_t i = 0; i < Size; i++, pvalue++)
    {
        if (Enqueue(q, *pvalue) < 0)
        {
            return -1;
        }
    }
    return 0;
}
