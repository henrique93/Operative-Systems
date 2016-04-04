/*
 * list.h - definitions and declarations of the integer list 
 */


#include <stdlib.h>
#include <stdio.h>
#include <time.h>



/* lst_terminal_iitem - each element of the list points to the next element */
typedef struct lst_terminal_iitem {
    int pid;
    struct lst_terminal_iitem *next;
} lst_terminal_iitem_t;

/* list_terminal_t */
typedef struct {
   lst_terminal_iitem_t * first;
} list_terminal_t;



/* lst_terminal_new - allocates memory for list_t and initializes it */
list_terminal_t* lst_terminal_new();

/* lst_terminal_destroy - free memory of list_t and all its items */
void lst_terminal_destroy(list_terminal_t *);

/* insert_terminal_new_process - insert a new item with process id in list 'list' */
void insert_terminal_new_process(list_terminal_t *list, int pid);

/* list_terminal_kill - kills all processes in the list */
void lst_terminal_kill(list_terminal_t *list);

