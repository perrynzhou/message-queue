all:
	rm -rf queue
	rm -rf remote_server
	gcc -g -DQUEUE_TEST common.h common.c ini.h ini.c  dnode.h  dnode.c g_log.h  g_log.c log.h  queue.h  queue.c -o queue -lpthread
	gcc -g common.h common.c ini.h ini.c g_log.h g_log.c log.h poll.h poll.c remote_server.c  -o remote_server
clear:
	rm -rf queue
