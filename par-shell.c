#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include "commandlinereader.h"
#include "list.h"

/*////////////////////////////////////////
//////////////GLOBAL VARIABLES////////////
////////////////////////////////////////*/
list_t* l;
int numChildren = 0;
int isExit = false;
pthread_mutex_t mutex;


/*///////////////////////////////////////
//////////////THREAD FUNCTION////////////
///////////////////////////////////////*/
void *waitThread() {
	int childID, status;
	while(1){
		if (numChildren > 0) {
			childID = wait(&status);
			pthread_mutex_lock(&mutex);
			update_terminated_process(l, childID, status, time(NULL));
			numChildren--;
			pthread_mutex_unlock(&mutex);
		}
		else if (numChildren == 0) {
			if (isExit == true) {
			      return NULL;
			}
			sleep(1);
		}
	}
}


/*////////////////////////////////////////////
///////////////////MAIN///////////////////////
////////////////////////////////////////////*/
int main() {
	char* arg[7];
	char buffer[100];
	int vectorSize = 7, pid, numargs;
	pthread_t waitChild;
	l = lst_new();
	printf("Insert your commands:\n");
	if (pthread_create(&waitChild, NULL, waitThread, NULL) != 0) {
		perror("Erro ao criar a thread\n");
		exit(EXIT_FAILURE);
	}
	while (1) {
        	numargs = readLineArguments(arg, vectorSize, buffer, 100);
		if (numargs == 0) {continue;}
		else if (strcmp(arg[0], "exit") != 0) {
			pid = fork();
			pthread_mutex_lock(&mutex);
			insert_new_process(l, pid, time(NULL));
			numChildren++;
			pthread_mutex_unlock(&mutex);
	  		if (pid < 0) {
	  			perror("Ocorreu um erro ao criar um filho\n");
	  		}
	  		if (pid == 0) {
	      			if (execv(arg[0], arg) < 0) {
		  			perror("ERRO: Ocorreu um erro ao executar o programa\n");
		  			exit(EXIT_FAILURE);
	      			}
	  		}
		}
        	else {
			isExit = true;
			pthread_join(waitChild, NULL);
			lst_print(l);
			lst_destroy(l);
			exit(EXIT_SUCCESS);
		}
	}
	return 0;
}
