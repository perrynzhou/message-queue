/*************************************************************************
	> File Name: queue.h
	> Author: perrynzhou
	> Mail: 715169549@qq.com
	> Created Time: Mon 09 Jan 2017 04:52:24 PM HKT
 ************************************************************************/

#ifndef _QUEUE_H
#define _QUEUE_H
#include <stdint.h>
#include <stdbool.h>
#include "dnode.h"

typedef struct _queue_s queue;

/*---public method-----*/
queue *queue_create (const char *path, const char *name, int32_t maxsize);
bool queue_push (queue * q, void *data, int32_t len);
dnode *queue_pop (queue * q, bool (*callback) (int, void *, int32_t));
int32_t queue_cursize (queue * q);
int32_t queue_maxsize (queue * q);
int32_t queue_socket (queue * q);
bool queue_destroy (queue * q);
#endif
