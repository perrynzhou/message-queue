/*************************************************************************
	> File Name: common.h
	> Author: perrynzhou
	> Mail: 715169549@qq.com
	> Created Time: Mon 09 Jan 2017 04:33:27 PM HKT
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>
#include <syscall.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#define g_malloc(M) calloc(1,M)
#define g_calloc(X)  calloc(1,X)
#define g_free(O)  free(O)
int client_socket (const char *host, int port);
bool send_message (int fd, void *s_msg, int s_msg_len);
bool is_digit (char *s);
bool is_ip (const char *ip);
#endif
