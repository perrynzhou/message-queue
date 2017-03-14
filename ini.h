/*************************************************************************
	> File Name: ini.h
	> Author: perrynzhou
	> Mail: 715169549@qq.com
	> Created Time: Mon 28 Nov 2016 12:49:08 PM HKT
 ************************************************************************/

#ifndef _INI_H
#define _INI_H
#include <stdint.h>
#include <stdbool.h>
typedef struct kv
{
	char *k;
	char *v;
	struct kv *next;
} kv;
typedef struct item_s
{
	char *section_name;
	kv *head;
	struct item_s *next;
} item;
typedef struct ini_s
{
	uint32_t size;
	item *head;
} ini;
ini *ini_create (const char *file);
void ini_destroy (ini * in);
bool ini_contain_section (ini * in, const char *sec_name);
char *ini_val (ini * in, const char *sec_name, const char *key);
#endif
