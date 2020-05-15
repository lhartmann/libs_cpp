#include "pexec.h"
#include <unistd.h>
#include <cstdlib>

// Executa file com argumentos argv[] substituido as streams padrão.
// Se um ponteiro for nulo a stream não é afetada.
// Retorna o PID do processo filho se tiver sucesso.
// Retorna -1 se falhar ao criar os pipes.
// Retorna -2 se falhar ao executar o fork.

#define fail(n) { \
	if (nstdin ) { close(nstdin ); close(*stdin ); } \
	if (nstdout) { close(nstdout); close(*stdout); } \
	if (nstderr) { close(nstderr); close(*stderr); } \
	return -n; \
}

int pexec(const char *file, const char *const argv[], int *stdin, int *stdout, int *stderr) {
	int nstdin=0, nstdout=0, nstderr=0;
	if (stdin) {
		int fd[2];
		if (pipe(fd)) fail(1); // Erro ao criar pipe
		*stdin=fd[1];
		nstdin=fd[0];
	}
	if (stdout) {
		int fd[2];
		if (pipe(fd)) fail(1); // Erro ao criar pipe
		*stdout=fd[0];
		nstdout=fd[1];
	}
	if (stderr) {
		int fd[2];
		if (pipe(fd)) fail(1); // Erro ao criar pipe
		*stderr=fd[0];
		nstderr=fd[1];
	}
	
	int i=fork();
	if (i<0) fail(2); // Erro ao executar fork;
	if (!i) { // No processo filho
		if (stdin) {
			close(*stdin);
			dup2(nstdin, STDIN_FILENO);
//			close(nstdin);
		}
		if (stderr) {
			close(*stderr);
			dup2(nstderr, STDERR_FILENO);
//			close(nstderr);
		}
		if (stdout) {
			close(*stdout);
			dup2(nstdout, STDOUT_FILENO);
//			close(nstdout);
		}
		
		execvp(file,(char*const*)argv); // Não deve retornar
		// Se chegar nessa linha falhou
		std::exit(1);
	}
	if (stdin) close(nstdin);
	if (stderr) close(nstderr);
	if (stdout) close(nstdout);
	
	return i;
}
