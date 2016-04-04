/*
 * list_terminal.c - implementation of the integer list functions 
 */


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "list_terminal.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>


list_terminal_t* lst_terminal_new()
{
    list_terminal_t *list;
    list = (list_terminal_t*) malloc(sizeof(list_terminal_t));
    list->first = NULL;
    return list;
}


void lst_terminal_destroy(list_terminal_t *list)
{
	struct lst_terminal_iitem *item, *nextitem;

	item = list->first;
	while (item != NULL){
		nextitem = item->next;
		free(item);
		item = nextitem;
	}
	free(list);
}


void insert_terminal_new_process(list_terminal_t *list, int pid)
{
	lst_terminal_iitem_t *item;

	item = (lst_terminal_iitem_t *) malloc (sizeof(lst_terminal_iitem_t));
	item->pid = pid;
	item->next = list->first;
	list->first = item;
}

void lst_terminal_kill(list_terminal_t *list) {
	lst_terminal_iitem_t *item;
	int pid;
	while (item != NULL) {
		pid = item->pid;
		kill(pid, SIGINT);
        item = item->next;
    }
}

