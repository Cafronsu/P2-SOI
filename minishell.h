#define _POSIX_C_SOURCE 200112L

//Este archivo es el fichero de cabecera principal del proyecto minishell

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#define COMMAND_LINE_SIZE 1024
#define COMMAND_LINE_ARGS_SIZE 64

#define N_JOBS 64

//Son valores de color y formato para la impresión en la terminal
#define RESET        "\033[0m"
#define NEGRITA      "\033[1m"
#define SUBRAYADO    "\033[4m"
#define PARPADEO     "\033[5m"
#define ROJO         "\033[31m"
#define VERDE        "\033[32m"
#define AMARILLO     "\033[33m"
#define AZUL         "\033[34m"
#define ROSA         "\033[35m"
#define CIAN         "\033[36m"
#define BLANCO       "\033[37m"


struct info_job
{
    pid_t pid;
    char comando[COMMAND_LINE_SIZE];
    char estado; // 'E' para ejecutando, 'F' para finalizado, 'D' para detenido, 'N' para ninguno
};


static struct info_job jobs_list[N_JOBS];

static char mi_shell[COMMAND_LINE_SIZE]; 


//Esta parte es para que el compilador sepa que las funciones existan independientemente del orden en el que luego se desarrollen en el código
void imprimir_prompt();
void limpiarRuta(char* rutaSucia, char* rutaLimpia);
void internal_exit(int key);
int internal_cd(char** args);
int internal_export(char** args);
int internal_source(char** args);
int internal_jobs(char** args);
int internal_fg(char** args);
int internal_bg(char** args);
int check_internal(char** args);
int parse_args(char** args, char* line);
char* read_line(char* line);
int execute_line(char* line);