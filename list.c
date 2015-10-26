/*
 * list.c - implementation of the integer list functions 
 */


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "list.h"



list_t* lst_new()
{
   list_t *list;
   list = (list_t*) malloc(sizeof(list_t));
   list->first = NULL;
   return list;
}


void lst_destroy(list_t *list)
{
	struct lst_iitem *item, *nextitem;

	item = list->first;
	while (item != NULL){
		nextitem = item->next;
		free(item);
		item = nextitem;
	}
	free(list);
}


void insert_new_process(list_t *list, int pid, time_t starttime)
{
	lst_iitem_t *item;

	item = (lst_iitem_t *) malloc (sizeof(lst_iitem_t));
	item->pid = pid;
	item->status = 0;
	item->starttime = starttime;
	item->endtime = 0;
	item->next = list->first;
	list->first = item;
}


void update_terminated_process(list_t *list, int pid, int status, time_t endtime) {
  lst_iitem_t *item;
  item = list->first;
  while(item->pid != pid){
		item = item->next;
	}
  item->status = status;
  item->endtime = endtime;
}


void lst_print(list_t *list)
{
	lst_iitem_t *item;
	double timeSpent;
	printf("Process list with ID, time in seconds and exit status:\nID\tTime(s)\tStatus\n");
	item = list->first;
	while (item != NULL) {
		timeSpent = difftime(item->endtime, item->starttime);
		printf("%d\t%.0f\t%d\n", item->pid, timeSpent, WEXITSTATUS(item->status));
		item = item->next;
	}
	printf("-- end of list.\n");
}
