/*************************************************************************
	> File Name: remote_server.c
	> Author: perrynzhou
	> Mail: 715169549@qq.com
	> Created Time: Tue 27 Dec 2016 04:26:00 PM HKT
 ************************************************************************/

#include <stdio.h>
#include <signal.h>
#include "poll.h"
#include "log.h"
#include "common.h"
#include "ini.h"
#define BUF_MAX_SIZE 2048
#define MAX_LISTEN_SIZE 1024
#define SRV_MAX_SIZE 64
static void read_cb (poll_event * p_evt, poll_event_element * node, struct epoll_event ev)
{
	log_info ("%s", "read_cb");
	char buf[BUF_MAX_SIZE] = { '\0' };
	int val = read (node->fd, buf, BUF_MAX_SIZE);
	if (val > 0)
	{
		log_info ("received data -> %s %t", buf, clock ());
		const char *flags = "commit ok";
		uint32_t len = strlen (flags);
		int w_len = write (node->fd, flags, len);
		if (w_len != len)
		{
			log_error ("write data to client %d failed", node->fd);
		}
	}
}

static void close_cb (poll_event * p_evt, poll_event_element * node, struct epoll_event ev)
{
	log_info ("%s", "in close_cb");
	poll_event_remove (p_evt, node->fd);
}

static void accept_cb (poll_event * p_evt, poll_event_element * node, struct epoll_event ev)
{
	log_info ("%s", "in accept_cb");
	struct sockaddr_in cli_addr;
	socklen_t cli_len = sizeof (cli_addr);
	int listenfd = accept (node->fd, (struct sockaddr *) &cli_addr, &cli_len);
	uint32_t flags = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
	poll_event_element *p;
	poll_event_add (p_evt, listenfd, flags, &p);
	p->read_callback = read_cb;
	p->close_callback = close_cb;
}

static int timeout_cb (poll_event * p_evt)
{
	if (p_evt->data != NULL)
	{
		log_info ("in timeout_cb");
		p_evt->data = g_calloc (sizeof (int));
	}
	else
	{
		int *val = (int *) p_evt->data;
		*val++;
		//log_info("timeout times %d",*val);
	}
	return 0;
}

static void usage (const char *name)
{
	fprintf (stdout, "usage:%s  conf_file\n", name);
	exit (0);
}

int main (int argc, char *argv[])
{
	if (argc != 2 || argv[1] == NULL)
	{
		usage (argv[1]);
	}
	log_init (NULL);
	ini *cfg = ini_create (argv[1]);
	if (cfg == NULL)
	{
		usage (argv[1]);
	}
	char *host = ini_val (cfg, "target", "host");
	char *port = ini_val (cfg, "target", "port");
	if (host == NULL || port == NULL || !is_ip (host) || !is_digit (port))
	{
		log_error ("%s", "host and port must be right");
		exit (0);
	}
	int sock = socket (AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in srv_addr;
	memset (&srv_addr, 0, sizeof (srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htons (INADDR_ANY);
	srv_addr.sin_port = htons (atoi (port));
	log_info ("bind socket %d", bind (sock, (struct sockaddr *) &srv_addr, sizeof (srv_addr)));
	log_info ("listen socket %d", listen (sock, MAX_LISTEN_SIZE));
	fcntl (sock, F_SETFL, O_NONBLOCK);

	log_info ("remote server host=%s,port=%s\n", host, port);
	//init poll
	poll_event *g_poll = poll_event_new (1000);

	g_poll->timeout_callback = timeout_cb;
	poll_event_element *p;
	poll_event_add (g_poll, sock, EPOLLIN, &p);
	p->accept_callback = accept_cb;
	p->read_callback = read_cb;
	p->close_callback = close_cb;
	p->cb_flags |= ACCEPT_CB;
	poll_event_loop (g_poll);
	return 0;
}
