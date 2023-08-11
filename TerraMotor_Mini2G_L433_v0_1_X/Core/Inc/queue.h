/*
 * queue.h
 *
 *  Created on: Apr 19, 2021
 *      Author: admin
 */

#ifndef INC_QUEUE_H_
#define INC_QUEUE_H_

#define MAX_QUEUE_SIZE         (10)

typedef struct
{
	uint32_t u32MaxStackDepth;
	int32_t u32Stacktop;
}strctPayloadStack;

typedef struct{
    int head;
    int tail;
    char* data[MAX_QUEUE_SIZE];
}strctQUEUE;

/* Queue Function Prototypes */
void initQueue(strctQUEUE *queue);
void enqueue(strctQUEUE * queue,char * data);
void dequeue(strctQUEUE * queue);

uint32_t isQueueEmpty(strctQUEUE * queue);
uint32_t isQueueFull(strctQUEUE * queue);

#endif /* INC_QUEUE_H_ */
