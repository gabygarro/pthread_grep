#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

/*
 * Implementación de una cola simple de char 
 */
#include <stdio.h>
#include <stdbool.h>

#define MAX 2048
 
struct queue
{
    char * queue_array[MAX];
    int front;
    int rear;
	int itemCount;
};
typedef struct queue QUEUE;
QUEUE q;
 
void queue_init();
void queue_push(char * data);
char *  queue_pop();
char * queue_peek();
void queue_display();
int queue_size();
bool queue_is_void();
bool queue_is_full();

#endif // QUEUE_H_INCLUDED