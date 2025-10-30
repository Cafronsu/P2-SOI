#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define COMMAND_LINE_SIZE 1024
#define COMMAND_LINE_ARGS_SIZE 64

/*
    Función para imprimir el prompt con el usuario y el directorio actual
    Parametros: ninguno
    Retorno: ninguno
*/
void imprimir_prompt(){
    char* user = getenv("USER");
    char* directorio = getcwd(NULL,0);
    printf("\033[1;31m%s\033[0m:\033[1;35m%s\033[0m$ ", user, directorio);
}

/*
    Función para manejar la salida del programa
    Parametros: key (int) - indica si la salida es por Ctrl+D (0) o por comando exit (1)
    Retorno: ninguno
*/
void internal_exit(int key){
    if(key == 0){
         
        printf("\nSaliendo con Ctrl+D");
        
        //Este trozo de código es para hacer una "animación" de puntos suspensivos
        for (int i=0; i<3; i++){
            fflush(stdout);
            printf(".");
            sleep(1);
        }

        fflush(stdout);
        sleep(1);
        printf("\n\033[1;0mTe echaremos de menos \U0001F972\n");  
    }else{
        printf("Saliendo con exit");

        for (int i=0; i<3; i++){
            fflush(stdout);
            printf(".");
            sleep(1);
        }

        fflush(stdout);
        sleep(1);
        printf("\n\033[1;0mTe echaremos de menos \U0001F972\n");
    }
    //Salimos del programa
    exit(0);
}

/*  
    Funcion para ejecutar el comando interno cd
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: por ahora nada.
*/

int internal_cd(char** args){
    printf("Ejecutando cd\n");
    return 1;
}

/*
    Funcion para ejecutar el comando interno export
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: por ahora nada.
*/
int internal_export(char** args){
    printf("Ejecutando export\n");
    return 1;
}

/*
    Funcion para ejecutar el comando interno source
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: por ahora nada.
*/
int internal_source(char** args){
    printf("Ejecutando source\n");
    return 1;
}

/*
    Funcion para ejecutar el comando interno jobs
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: por ahora nada.
*/
int internal_jobs(char** args){
    printf("Ejecutando jobs\n");
    return 1;
}

/*
    Funcion para ejecutar el comando interno fg
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: por ahora nada.
*/
int internal_fg(char** args){
    printf("Ejecutando fg\n");
    return 1;
}

/*
    Funcion para ejecutar el comando interno bg
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: por ahora nada.
*/
int internal_bg(char** args){
    printf("Ejecutando bg\n");
    return 1;
}


/*
    Funcion para comprobar si el comando es interno y ejecutarlo
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: 1 si el comando es interno y se ha ejecutado, 0 si no lo es.
*/
int check_internal(char** args){

    //Comprobamos que vale el primer argumento y, si es uno de los comandos internos, lo ejecutamos
        if(strcmp(args[0], "cd") == 0){
            internal_cd(args);
            return 1;
        }
        else if(strcmp(args[0], "export") == 0){
            internal_export(args);
            return 1;
        }
        else if(strcmp(args[0], "source") == 0){
            internal_source(args);
            return 1;
        }
        else if(strcmp(args[0], "jobs") == 0){
            internal_jobs(args);
            return 1;
        }
        else if(strcmp(args[0], "fg") == 0){
            internal_fg(args);
            return 1;
        }
        else if(strcmp(args[0], "bg") == 0){
            internal_bg(args);
            return 1;
        }
        else if(strcmp(args[0], "exit") == 0){
            internal_exit(1);
            return 1;
        }
        return 0;

}

/*
    Función para parsear la línea de comandos en argumentos
    Parametros: args (char**) - array de strings donde se guardarán los argumentos
                line (char*) - línea de comandos a parsear
    Retorno: número de argumentos parseados
*/
int parse_args(char **args, char*line){
        int i = 0;
        char* token = strtok(line, " ");

        do{
            if(token[0] == '#'){ // Si encontramos un comentario, lo ignoramos y no seguimos parseando
                token = NULL;
            }else{
                args[i] = token;
                i++;
                token = strtok(NULL, " ");
            }
        }
        while(token!= NULL);
        args[i] = NULL;
        return i;
}

/*
    Función para leer una línea de comandos
    Parametros: buffer (char*) - buffer donde se guardará la línea leída
    Retorno: línea leída (char*)
*/
char* read_line(char* buffer){

    //Imprimimos el prompt, hacemos un flush para que se vea inmediatamente
    imprimir_prompt();
    sleep(0.5);
    fflush(stdout);

    //Leemos la linea

    char* linea = fgets(buffer, COMMAND_LINE_SIZE, stdin);
    if(linea != NULL){
        size_t len = strlen(linea);
        if(len > 0){
            linea[len-1] = '\0'; //Eliminamos el salto de linea
        }
        return linea;
    }else{
        if(feof(stdin)){
            internal_exit(0);
        }
    }
        
}

/*
    Función para ejecutar una línea de comandos
    Parametros: line (char*) - línea de comandos a ejecutar
    Retorno: ninguno por ahora
*/
int execute_line(char* line){
    char* args[COMMAND_LINE_SIZE];
    int k = parse_args(args, line);
    if(k != 0){
        check_internal(args);
    }  
}


void main(){
    //Realizamos un bucle infinito para el shell
    char line[COMMAND_LINE_SIZE];
    while(1){
        if(read_line(line)){
            execute_line(line);
        }
    }
}

