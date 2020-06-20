#ifndef _COLLECTIONS_QUEUE_H_
#define _COLLECTIONS_QUEUE_H_

#include <stdlib.h>

#include "collections/dllist.h"

typedef struct Queue {

    DoubleList *dlist;
    void (*destroy)(void *data);

} Queue;

#endif