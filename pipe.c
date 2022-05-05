#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[])
{
	if (argc == 1) {
		// invalid number of arguments
		exit(EINVAL);
	}

	if (argc == 2) {
		// only one process
		execlp(argv[1], argv[1], NULL);
		exit(errno);
	}

	int pipes[2*(argc - 2)];
	pid_t pid;

	for (int i = 0; i < argc - 2; i++) {
		if (pipe(pipes + 2*i) == -1) {
			// failure to create pipes
			exit(errno);
		}
	}
	for (int i = 1; i < argc; i++) {
		pid = fork();
		if (pid == -1) {
			// bad fork
			exit(errno);
		}
		if (pid == 0) {
			int READ = -1;
			int WRITE = -1;
			if (i == 1) {
				// only open write pipe for first argument
				WRITE = 1;
			}
			else if (i == argc - 1) {
				// only open read pipe for last argument
				READ = 2*(argc - 2) - 2;
			}
			else {
				// otherwise open both
				WRITE = 2*i - 1;
				READ = 2*i - 4;
			}
			// set then close the pipes
			if (READ != -1) {
				dup2(pipes[READ], STDIN_FILENO);
			}
			if (WRITE != -1) {
				dup2(pipes[WRITE], STDOUT_FILENO);
			}
			for (int j = 0; j < 2*(argc - 2); j++) {
				if (j != READ && j != WRITE) {
					close(pipes[j]);
				}
			}
			// execute the ith process
			execlp(argv[i], argv[i], NULL);
			exit(errno);
		} 
	}
	int stat;
	for (int i = 1; i < argc; i++) {
		// clean up pipes by closing all of them
		for (int j = 0; j < 2*(argc - 2); j++) {
			close(pipes[j]);
		}
		// if no error status so far set stat to next exit status
		if (i == 1 || WEXITSTATUS(stat) == 0) {
			wait(&stat);
		}
		// error detected, wait normally
		else {
			wait(NULL);
		}
	}

	// exit if child returned an error status, otherwise return normally
	if (WEXITSTATUS(stat) != 0) exit(WEXITSTATUS(stat));

	return 0;
}
