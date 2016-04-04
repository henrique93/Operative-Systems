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
#include <sys/types.h>
#include <sys/stat.h>

/* Starting message */
#define start "Insert your commands:\n"

/* Pipe file */
#define PIPE_S "/tmp/par-shell-stats"

/*////////////////////////////////////////
///////////////// COMMANDS ///////////////
////////////////////////////////////////*/
/* Wait for every child to finish and
 shut down everything in order, terminating
 the program */
#define EXIT "exit\n"

/* Wait for every child to finish and
 shut down everything in order, prints the
 file log.txt, terminates every
 par-shell-terminal and terminates the
 program */
#define exitGlobal "exit-global\n"

/* Print program status, prints the number
 of children currently executing and the
 program's total execution time*/
#define stats "stats\n"


/*////////////////////////////////////////
///////////////ERROR MESSAGES/////////////
////////////////////////////////////////*/
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

/* Pipe errors */
#define error_pipe "ERRO: falha ao criar o pipe"
#define error_pipe_open "ERRO: falha ao abrir o pipe"
#define error_mkfifo "ERRO: falha ao criar o FIFO"

/*////////////////////////////////////////////
///////////////////MAIN///////////////////////
////////////////////////////////////////////*/
int main(int argc, char *argv[])
{
	int fDescPipe, PipeS, TPID;
	char mess[100], pipeStats[200], fDescPipeStats[100], starter[20];
	/*maybe use while*/
	fDescPipe = open(argv[1], O_WRONLY);
	if (fDescPipe < 0) printf("Deu erro a abrir o pipe\n");

	

	printf(start);
	sprintf(starter, "start %d\n", getpid());
	write(fDescPipe, starter, strlen(starter));

	while(1){
		fgets(mess, 100, stdin);
		if(strcmp(mess, EXIT) == 0){
			exit(EXIT_SUCCESS);
		}
		if(strcmp(mess, stats) == 0){
			TPID = getpid();
			sprintf(pipeStats, "%s %d\n", "stats", TPID);
			write(fDescPipe, pipeStats, strlen(pipeStats));

			sprintf(fDescPipeStats, "%s-%d", PIPE_S, TPID);
			unlink(fDescPipeStats);
			if(mkfifo(fDescPipeStats, 0777) != 0){
                perror(error_mkfifo);
                exit(EXIT_FAILURE);
            }
			PipeS = open(fDescPipeStats, O_RDONLY);
			read(PipeS, mess, 100);
	    	printf("%s\n", mess);
			continue;
		}
		if(strcmp(mess, exitGlobal) == 0){
			sprintf(mess, "%s %d\n", "exit-global", TPID);
			write(fDescPipe, mess, strlen(mess));
		}
		else {
			printf(mess, "%s %d\n", mess, TPID);
			write(fDescPipe, mess, strlen(mess));
		}
		

	}

	return 0;
}





