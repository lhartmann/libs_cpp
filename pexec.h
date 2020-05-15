// pexec.h

#ifndef PEXEC_H
#define PEXEC_H

// Executa file com argumentos argv[] substituindo as streams padrão.
// Se um ponteiro for nulo a stream não é afetada.
// Retorna o PID do processo filho se tiver sucesso.
// Retorna -1 se falhar ao criar os pipes.
// Retorna -2 se falhar ao executar o fork.

extern int pexec(const char *file, const char *const argv[], int *stdin, int *stdout, int *stderr);

#endif
