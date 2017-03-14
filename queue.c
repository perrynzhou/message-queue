/*************************************************************************
	> File Name: queue.c
	> Author: perrynzhou
	> Mail: 715169549@qq.com
	> Created Time: Mon 09 Jan 2017 05:01:09 PM HKT
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "common.h"
#include "queue.h"
#include "log.h"
#include "ini.h"
struct _queue_s
{
	char *name;
	dnode *v;
	int32_t maxsize;			//how many dnode_s in queue
	int32_t cursize;			//current size of queue
	int32_t inpos;				//data position where should in insert into queue
	int32_t outpos;				//pop data positon
	int32_t fd;
	pthread_mutex_t lock;
	pthread_cond_t nonempty;	//is not empty flag
	pthread_cond_t nonfull;
};
static inline int32_t queue_incr (queue * q, int32_t pos)
{
	log_info ("%d", (pos + 1) % q->maxsize);
	return (pos + 1) % q->maxsize;
}

static inline bool queue_empty (queue * q)
{
	if (q->inpos == q->outpos)
	{
		log_info ("queue is empty now,queue cursize %d", q->cursize);
		return true;
	}
	log_info ("queue not empty now,queue cursize %d", q->cursize);
	return false;
}

static inline bool queue_full (queue * q)
{
	if (queue_incr (q, q->inpos) == q->outpos)
	{
		log_info ("queue is full now,queue cursize %d", q->cursize);
		return true;
	}
	log_info ("queue not full now,queue cursize %d", q->cursize);
	return false;
}

queue *queue_create (const char *path, const char *name, int32_t maxsize)
{
	ini *cfg = NULL;
	queue *q = NULL;
	char *qname = NULL;
	dnode *v = NULL;
	int fd = -1;
	if (path == NULL || (cfg = ini_create (path)) == NULL || name == NULL || maxsize <= 0)
	{
		goto _ERROR;
	}
	q = g_malloc (sizeof (*q));
	uint32_t len = strlen (name);
	qname = (char *) g_malloc (len + 1);
	v = g_malloc (sizeof (dnode) * maxsize);
	if (NULL == q || NULL == qname || NULL == v)
	{
		goto _ERROR;
	}
	q->maxsize = maxsize;
	q->cursize = q->inpos = q->outpos = 0;
	q->v = v;
	char *host = ini_val (cfg, "target", "host");
	char *port = ini_val (cfg, "target", "port");
	if (host == NULL || port == NULL || !is_ip (host) || !is_digit (port))
	{
		log_error ("host and port must be right");
		goto _ERROR;
	}
	q->fd = client_socket (host, atoi (port));
	if (q->fd == -1)
	{
		log_error ("can't init a socket fd");
		goto _ERROR;
	}
	pthread_mutex_init (&(q->lock), NULL);
	pthread_cond_init (&(q->nonempty), NULL);
	pthread_cond_init (&(q->nonfull), NULL);
	log_info ("queue maxsize %d,cursize %d", q->maxsize, q->cursize);
	if (NULL != cfg)
	{
		ini_destroy (cfg);
		log_info ("destroy ini configur");
	}
	log_info ("create queue %p,maxsize %d,remote server fd %d", q, q->maxsize, q->fd);
	return q;
  _ERROR:
	if (NULL != q)
	{
		g_free (q);
		q = NULL;
	}
	if (NULL != qname)
	{
		g_free (qname);
		qname = NULL;
	}
	if (NULL != v)
	{
		g_free (v);
		v = NULL;
	}
	if (cfg != NULL)
	{
		ini_destroy (cfg);
	}
	log_error ("%s", "create queue occur error");
	return NULL;
}

bool queue_destroy (queue * q)
{
	if (NULL != q)
	{
		g_free (q->name);
		g_free (q->v);
		q->name = NULL;
		q->v = NULL;
		pthread_mutex_destroy (&(q->lock));
		pthread_cond_destroy (&(q->nonempty));
		pthread_cond_destroy (&(q->nonfull));
		log_info ("%s", "destroy a queue ok");
		return true;
	}
	log_info ("%s", "no need to destroy queue");
	return false;
}

bool queue_push (queue * q, void *data, int32_t len)
{
	if (q == NULL)
	{
		return false;
	}
	pthread_mutex_lock (&(q->lock));
	while (queue_full (q))
	{
		log_info ("blocking data because queue is full");
		pthread_cond_wait (&(q->nonempty), &(q->lock));
	}
	if (!dnode_init (&(q->v[q->inpos]), data, len))
	{
		log_error ("%s", "dnode init failed");
		return false;
	}
	q->inpos = queue_incr (q, q->inpos);
	if (pthread_mutex_unlock (&(q->lock)) == -1 || pthread_cond_signal (&(q->nonempty)) == -1)
	{
		log_error ("%s", "lock or send signal failed");
		return false;
	}
	__sync_fetch_and_add (&(q->cursize), 1);
	log_info ("push data ok,cursize %d ", q->cursize);
	return true;
}

dnode *queue_pop (queue * q, bool (*callback) (int, void *, int32_t))
{
	pthread_mutex_lock (&(q->lock));
	while (queue_empty (q))
	{
		log_info ("blocking data because queue is empty");
		pthread_cond_wait (&(q->nonempty), &(q->lock));
	}
	dnode *rs = &q->v[q->outpos];
	if (callback != NULL)
	{
		bool ret = (*callback) (q->fd, rs->data, rs->len);
		log_info ("callback %p execute %d", callback, ret);
	}
	q->outpos = queue_incr (q, q->outpos);
	if (pthread_mutex_unlock (&(q->lock)) == -1 || pthread_cond_signal (&(q->nonfull)) == -1)
	{
		log_error ("%s", "lock or send signal failed");
		return NULL;
	}
	log_info ("%s", "send a queue not full signal");
	__sync_fetch_and_sub (&(q->cursize), 1);
	log_info ("pop data ok,cursize %d,dnode %p,data =%s,len=%d", q->cursize, rs, rs->data, rs->len);
	return rs;
}

#ifdef QUEUE_TEST
char *rand_string (int len)
{
	char *s = g_malloc (len + 1);
	int i = 0;
	while (i < len)
	{
		if (i % 3 == 0)
		{
			s[i++] = 'A' + (rand () % 26);
		}
		else if (i % 5 == 0)
		{
			s[i++] = 'a' + (rand () % 26);
		}
		else
		{
			s[i++] = 'A' + (rand () % 26);
		}
	}
	return s;
}

int main (int argc, char **argv)
{
	int size = 10, i = 0;
	queue *q = queue_create (argv[1], "test_queue", 10);
	if(q == NULL)
    {
        return 0;
    }
    log_init (NULL);
	char *data[10];
	for (i = 0; i < size - 1; i++)
	{
		data[i] = rand_string (10);
		log_info ("push data %d", queue_push (q, data[i], strlen (data[i])));
	}
	for (i = 0; i < size; i++)
	{
		queue_pop (q, &send_message);
		g_free (data[i]);
	}
	return 0;
}
#endif
