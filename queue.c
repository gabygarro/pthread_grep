#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "queue.h"

void queue_init() {
   q.front = 0;
   q.rear = -1;
   q.itemCount = 0;
}

char * queue_peek() {
   return q.queue_array[q.front];
}

bool queue_is_void() {
   return q.itemCount == 0;
}

bool queue_is_full() {
   return q.itemCount == MAX;
}

int queue_size() {
   return q.itemCount;
}  

void queue_push(char * data) {
   if (!queue_is_full()) {
      if (q.rear == MAX-1) {
         q.rear = -1;            
      }
      //q.queue_array[++q.rear] = data;
      q.queue_array[++q.rear] = strdup(data);
      //printf("%s\n", q.queue_array[q.rear]);
      q.itemCount++;
   }
}

char * queue_pop() {
   char * data;
   data = strdup(q.queue_array[q.front++]);
   if (q.front == MAX) {
      q.front = 0;
   }
   q.itemCount--;
   //printf("%s\n", data);
   return data;  
}