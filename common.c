/*************************************************************************
	> File Name: common.c
	> Author: 
	> Mail: 
	> Created Time: Mon 09 Jan 2017 11:44:19 AM EST
 ************************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <regex.h>
#include <netinet/in.h>
#include "common.h"
int client_socket (const char *host, int port)
{
	if (port <= 0)
		goto ERROR;
	int sockfd, n;
	struct sockaddr_in servaddr;
	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf ("sokcet:%s\n", strerror (errno));
		goto ERROR;
	}
	memset (&servaddr, 0, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (port);
	if (inet_pton (AF_INET, host, &servaddr.sin_addr) <= 0)
	{
		printf ("inet_pton:%s\n", strerror (errno));
		goto ERROR;
	}
	if (connect (sockfd, (struct sockaddr *) &servaddr, sizeof (servaddr)) < 0)
	{
		printf ("connect:%s\n", strerror (errno));
		goto ERROR;
	}
	return sockfd;
  ERROR:
	if (sockfd != -1)
	{
		close (sockfd);
	}
	return -1;
}

bool is_digit (char *s)
{
	if (s == NULL)
		return false;
	uint32_t len = strlen (s);
	uint32_t i = 0;
	for (; i < len; i++)
	{
		if (isdigit (s[i]) == 0)
		{
			return false;
		}
	}
	return true;
}

bool is_ip (const char *ip)
{
	int cflags = REG_EXTENDED;
	regmatch_t pmatch[1];
	const size_t nmatch = 1;
	regex_t reg;
	const char *pattern = "^[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+$";
	regcomp (&reg, pattern, cflags);
	bool ret = true;
	if (regexec (&reg, ip, nmatch, pmatch, 0) == REG_NOMATCH)
	{
		ret = false;
	}
	regfree (&reg);
	return ret;
}

bool send_message (int sockfd, void *s_msg, int s_msg_len)
{
	if (sockfd == -1 || s_msg_len != send (sockfd, s_msg, s_msg_len, 0))
	{
		return false;
	}
	return true;
}
