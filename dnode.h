/*************************************************************************
	> File Name: dnode.h
	> Author: perrynzhou
	> Mail: 715169549@qq.com
	> Created Time: Mon 09 Jan 2017 05:19:21 PM HKT
 ************************************************************************/

#ifndef _DNODE_H
#define _DNODE_H
#include <stdint.h>
#include <stdbool.h>
typedef struct _dnode_s
{
	void *data;
	int32_t len;
} dnode;
dnode *dnode_create (void *data, int32_t data_len);
bool dnode_init (dnode * d, void *data, int32_t data_len);
void dnode_destroy (dnode * d, void (*callback) (void *));
#endif
