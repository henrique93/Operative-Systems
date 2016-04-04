#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include "commandlinereader.h"
#include "list.h"
#include "list_terminal.h"
#include "Integer_List.h"

/* Pipe file */
#define PIPE "/tmp/par-shell-in"

/* Pipe file */
#define PIPE_S "/tmp/par-shell-stats"


/*////////////////////////////////////////
///////////////// COMMANDS ///////////////
////////////////////////////////////////*/
/* Wait for every child to finish and
 shut down everything in order, terminating
 the program */
#define EXIT "exit"

 /* Wait for every child to finish and
 shut down everything in order, prints the
 file log.txt, terminates every
 par-shell-terminal and terminates the
 program */
#define EXITGLOBAL "exit-global"

/* Print program status, prints the number
 of children currently executing and the
 program's total execution time*/
#define stats "stats"

#define starter "start"



/*////////////////////////////////////////
////////////// ERROR MESSAGES ////////////
////////////////////////////////////////*/
/* Fork errors */
#define error_fork "ERRO: falha ao criar um filho"

/* Mutex errors */
#define error_mutex "ERRO: falha ao criar o mutex"
#define error_mutex_lock "ERRO: falha ao bloquear o mutex"
#define error_mutex_unlock "ERRO: falha ao desbloquear o mutex"

/* Exec errors */
#define error_execv "ERRO: Ocorreu um erro ao executar o programa"

/* Thread errors */
#define error_thread_create "ERRO: falha ao criar a thread"
#define error_thread_join "ERRO: falha ao terminar a thread 'waitChild'"

/* File errors */
#define error_fopen "ERRO: falha ao abrir o ficheiro 'log.txt'"
#define error_fclose_log "ERRO: falha ao fechar o ficheiro 'log.txt'"

/* Condition errors */
#define error_cond_init_execute "ERRO: falha ao iniciar a condicao 'can_execute'"
#define error_cond_init_wait "ERRO: falha ao iniciar a condicao 'can_wait'"
#define error_cond_execute "ERRO: falha ao sinalizar a condicao 'can_execute'"
#define error_cond_wait "ERRO: falha ao sinalizar a condicao 'can_wait'"
#define error_cond_destroy_execute "ERRO: falha ao destruir a condicao 'can_execute'"
#define error_cond_destroy_wait "ERRO: falha ao destruir a condicao 'can_wait'"

/* Pipe errors */
#define error_pipe "ERRO: falha ao criar o pipe"
#define error_pipe_open "ERRO: falha ao abrir o pipe"
#define error_mkfifo "ERRO: falha ao criar o FIFO"

/* Signal errors */
#define error_signal "ERRO: falha ao associar a rotina ao signal SIGINT"


/*////////////////////////////////////////
///////////// GLOBAL VARIABLES ///////////
////////////////////////////////////////*/
list_t* l;

/* Terminal process id's list */
list_terminal_t* l_terminal;


int numChildren = 0;
int isExit = false;
int timeTotal = 0;

/* Maximum number of children running at
 the same time */
int MAXPAR = 2;

int flag_signal = false;

pthread_mutex_t mutex;
pthread_cond_t can_wait, can_execute;


/*///////////////////////////////////////
///////////// THREAD FUNCTION ///////////
///////////////////////////////////////*/
void *waitThread() {
	int childID, status, iteration = 0;
	char linha[256];
	FILE *f;
	f =  fopen("log.txt", "a+");
	if (f == NULL) {
		perror(error_fopen);
	}
	while (fgets(linha, 256, f) != NULL) {
	    iteration++;
	}
    if (pthread_mutex_lock(&mutex) != 0) {
        perror(error_mutex_lock);
    }
	sscanf(linha, "total execution time: %d", &timeTotal);
    if (pthread_mutex_unlock(&mutex) != 0) {
        perror(error_mutex_unlock);
    }
	if (iteration > 0) {
	    iteration = iteration / 3;
	}
	while (1){
		if (pthread_mutex_lock(&mutex) != 0) {
			perror(error_mutex_lock);
		}
		while((numChildren == 0) && (isExit == false)) {
			pthread_cond_wait(&can_wait, &mutex);
		}
		if (pthread_mutex_unlock(&mutex) != 0) {
			perror(error_mutex_unlock);
		}
		if (numChildren > 0) {
			childID = wait(&status);
			if (pthread_mutex_lock(&mutex) != 0) {
				perror(error_mutex_lock);
			}
			timeTotal += update_terminated_process(l, childID, status, time(NULL));
			file_print(f, l, iteration, childID, timeTotal);
			iteration++;
			numChildren--;
			if (pthread_cond_signal(&can_execute) != 0) {
				perror(error_cond_execute);
			}
			if (pthread_mutex_unlock(&mutex) != 0) {
				perror(error_mutex_unlock);
			}
		}
		else if (isExit == true) {
			if (fclose(f) != 0) {
				perror(error_fclose_log);
			}
			pthread_exit(NULL);
		}
	}
}

/*////////////////////////////////////////////
////////////// SIGINT FUNCTION ///////////////
////////////////////////////////////////////*/
void CTRLC(int s) {
    if (s == SIGINT) {
    	close(0);
        flag_signal = true;
    }
    /*lst_terminal_kill(l_terminal);*/
}


/*////////////////////////////////////////////
/////////////// MAIN FUNCTION ////////////////
////////////////////////////////////////////*/
int main() {
	char* arg[7];
	char buffer[100], filename[100], fnamePipeStats[100];
	int vectorSize = 7, pid, numargs, fd, fDescPIPE, fDescPIPE_S;
	pthread_t waitChild;
	pid_t pidChild;
	terminalLst* terminals = new_terminal_lst();
	l = lst_new();
	if (pthread_mutex_lock(&mutex) != 0) {
		perror(error_mutex_lock);
	}
	if (pthread_mutex_unlock(&mutex) != 0) {
		perror(error_mutex_unlock);
	}
    
	if (pthread_mutex_init(&mutex, NULL) != 0){
		perror(error_mutex);
	}
	if (pthread_cond_init(&can_wait, NULL) != 0) {
		perror(error_cond_init_wait);
	}
	if (pthread_cond_init(&can_execute, NULL) != 0) {
		perror(error_cond_init_execute);
	}
	if (pthread_create(&waitChild, NULL, waitThread, NULL) != 0) {
		perror(error_thread_create);
		exit(EXIT_FAILURE);
	}
    if (signal(SIGINT, CTRLC) == SIG_ERR) {
        perror(error_signal);
    }
	unlink(PIPE); /*-------------------------------------------------*/

	if (mkfifo(PIPE, 0777) != 0) {
		perror(error_mkfifo);
		exit(EXIT_FAILURE);
	}
	if ((fDescPIPE = open(PIPE, O_RDONLY)) == -1) {
		perror(error_pipe_open);
		exit(EXIT_FAILURE);
	}

	close(0);
	dup2(fDescPIPE, 0);

	while (1) {

		numargs = readLineArguments(arg, vectorSize, buffer, 100);

		if(flag_signal)
			break;

		if (numargs <= 1) {
			continue;
		}
		
		if (pthread_mutex_lock(&mutex) != 0) {
			perror(error_mutex_lock);
		}
		while (numChildren == MAXPAR) {
            if (pthread_cond_wait(&can_execute, &mutex) != 0) {
                perror(error_cond_wait);
            }
		}
		if (pthread_mutex_unlock(&mutex) != 0) {
			perror(error_mutex_unlock);
		}
		else if (strcmp(arg[0], EXIT) != 0) {
			if (strcmp(arg[0], starter) == 0) {
				int pid_terminal = atoi(arg[1]);
				insert_terminal(terminals, pid_terminal);
				continue;
			}
			if (strcmp(arg[0], stats) == 0) { /*tratar o PID e meter na lista*/
				sprintf(fnamePipeStats, "%s-%s", PIPE_S, arg[1]);
                
                while((fDescPIPE_S = open(fnamePipeStats, O_WRONLY)) < 0);
                if(fDescPIPE < 0) {
					perror(error_pipe_open);
					exit(EXIT_FAILURE);
				}
				if (pthread_mutex_lock(&mutex) != 0) {
                    perror(error_mutex_lock);
                }

				sprintf (buffer, "Number of active children: %d\nTotal execution time to the moment: %d", numChildren, timeTotal);
				write(fDescPIPE_S, buffer, strlen(buffer));
                if (pthread_mutex_unlock(&mutex) != 0) {
                    perror(error_mutex_unlock);
                }

				close(fDescPIPE_S);
				continue;
			}

            if (strcmp(arg[0], EXITGLOBAL) == 0) { /* SONFOILULDNILDEFNC*/
                printf("1\n");
                CTRLC(SIGINT);
            }
			else {
				pid = fork();
				if (pid < 0) {
					perror(error_fork);
				}
				if (pthread_mutex_lock(&mutex) != 0) {
					perror(error_mutex_lock);
				}
				insert_new_process(l, pid, time(NULL));
				numChildren++;
				if (pthread_cond_signal(&can_wait) != 0) {
					perror(error_cond_wait);
				}
				if (pthread_mutex_unlock(&mutex) != 0) {
					perror(error_mutex_unlock);
				}
				if (pid == 0) {
					pidChild = getpid();
					sprintf(filename, "par-shell-%d.txt", pidChild);
					fd = open(filename,  O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
					close(1);
					dup(fd);
					close(fd);
					if (execv(arg[0], arg) < 0) {
						perror(error_execv);
						exit(EXIT_FAILURE);
					}
				}
				else {
					sprintf(filename, "par-shell-%d.txt", pid);
				}
			}
        }
        else {
            break;
        }
	}
	isExit = true;
    if (pthread_cond_signal(&can_wait) != 0) {
        perror(error_cond_wait);
    }
    if (pthread_mutex_unlock(&mutex) != 0) {
        perror(error_mutex_unlock);
    }
    if (pthread_join(waitChild, NULL) != 0) {
        perror(error_thread_join);
    }
    lst_print(l);
    lst_destroy(l);
    while(remove_terminal(terminals)!= -1){

    }
    if (pthread_cond_destroy(&can_wait) != 0) {
        perror(error_cond_destroy_wait);
    }
    if (pthread_cond_destroy(&can_execute) != 0) {
        perror(error_cond_destroy_execute);
    }
    exit(EXIT_SUCCESS);
	return 0;
}
