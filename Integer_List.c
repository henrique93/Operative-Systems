/*
 * intList.c - implementation of the integer list functions
 */


#include <stdlib.h>
#include <stdio.h>
#include "Integer_List.h"
#include <signal.h>
 



terminalLst* new_terminal_lst()
{
   terminalLst *list;
   list = (terminalLst*) malloc(sizeof(terminalLst));
   list->first = NULL;
   return list;
}

void insert_terminal(terminalLst *list, int pid)
{
	terminalI_t *item;

	item = (terminalI_t *) malloc (sizeof(terminalI_t));
	item->pid = pid;
	item->next = list->first;
	list->first = item;
}

int remove_terminal(terminalLst *list)
{
  int pid=0;
  terminalI_t *aux = NULL;
  terminalI_t *ap= NULL;
  if(list->first != NULL){
    aux = list->first->next;
    ap = list->first;
    list->first = aux;
    pid = ap->pid;
    ap->next=NULL;
    free(ap);
    kill(pid, SIGINT);
    return pid;
  }
  else 
    free(list);
    return -1;
}

