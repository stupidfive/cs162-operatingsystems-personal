#include "queue.h"
#include <stdlib.h>

queue_t *new_queue() {
  queue_t *q = malloc(sizeof(queue_t));
  q->head = q->tail = NULL;
  return q;
}

void enqueue (queue_t *q, void *val) {
  item_t *new_item = malloc(sizeof(item_t));
  new_item->val = val;
  new_item->next = q->head;
  new_item->prev = NULL;
  if (q->head) q->head->prev = new_item;
  else q->tail = new_item;
  q->head = new_item;
}

void *dequeue(queue_t *q) {
  item_t *pop = q->tail;
  void *res;
  if (!pop) return NULL;
  res = pop->val;
  q->tail = pop->prev;
  if (q->tail) q->tail->next = NULL;
  else q->head = NULL;
  free(pop);
  return res;
}

bool empty_queue(queue_t *q) {
  return (q->head == NULL);
}
