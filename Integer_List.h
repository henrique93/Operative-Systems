/*
 * intList.h - definitions and declarations of the integer list
 */

#ifndef INTLIST_H
#define INTLIST_H

#include <stdlib.h>
#include <stdio.h>



/* lst_iItem - each element of the list points to the next element */
typedef struct terminalI{
   int pid;
   struct terminalI *next;
} terminalI_t;

/* list_t */
typedef struct {
   terminalI_t *first;
} terminalLst;


/* lst_new - allocates memory for list_t and initializes it */
terminalLst* new_terminal_lst();

/* insert um pid na lista */
void insert_terminal(terminalLst *list, int pid);

/* remove pid da lista */
int remove_terminal(terminalLst *list);

#endif
