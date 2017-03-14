/*************************************************************************
	> File Name: dnode.c
	> Author: perrynzhou
	> Mail: 715169549@qq.com
	> Created Time: Mon 09 Jan 2017 05:21:57 PM HKT
 ************************************************************************/

#include <stdlib.h>
#include "common.h"
#include "dnode.h"
inline dnode *dnode_create (void *data, int32_t data_len)
{
	if (data == NULL || data_len <= 0)
	{
		return NULL;
	}
	dnode *res = g_malloc (sizeof (*res));
	if (NULL == res)
	{
		return NULL;
	}
	res->data = data;
	res->len = data_len;
	return res;
}

bool dnode_init (dnode * d, void *data, int32_t data_len)
{
	if (NULL != d)
	{
		d->data = data;
		d->len = data_len;
		return true;
	}
	return false;
}

void dnode_destroy (dnode * d, void (*callback) (void *))
{
	if (NULL != d)
	{
		if (callback != NULL)
		{
			(*callback) (d->data);
		}
		g_free (d);
		d = NULL;
	}
}
