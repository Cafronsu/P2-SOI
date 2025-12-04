#include "minishellnivel6.h"

/*
    Función para imprimir el prompt con el usuario y el directorio actual
    Parametros: ninguno
    Retorno: ninguno
*/
void imprimir_prompt()
{
    // Obtenemos el usuario y el directorio actual para imprimirlo
    char *user = getenv("USER");
    char *directorio = getcwd(NULL, 0);
    printf(NEGRITA AMARILLO "%s" RESET ":" NEGRITA CIAN "%s" RESET "$ ", user, directorio);
}

/*
    Función para limpiar de comillas dobles, simples y \ la ruta introducida por parámetro
    Parametros: rutaSucia (char*) - ruta a limpiar y rutaLimpia (char*) - ruta limpia
    Retorno: ninguno
*/
void limpiarRuta(char *rutaSucia, char *rutaLimpia)
{

    // Recorremos la ruta sucia y miramos cada carácter:
    // Si es una \ seguida de un espacio, añadimos un espacio a la ruta limpia y saltamos el siguiente carácter
    // Si es una comilla simple o doble, la ignoramos
    // Si no, añadimos el carácter a la ruta limpia
    int j = 0;
    int i = 0;
    while (rutaSucia[j] != '\0')
    {
        if (rutaSucia[j] == '\\' && rutaSucia[j + 1] == ' ')
        {
            rutaLimpia[i] = ' ';
            i++;
            j += 2;
        }
        else if (rutaSucia[j] == '\"' || rutaSucia[j] == '\'')
        {
            j++;
        }
        else
        {
            rutaLimpia[i] = rutaSucia[j];
            i++;
            j++;
        }
    }
    rutaLimpia[i] = '\0';
}

/*
    Función para manejar la salida del programa
    Parametros: key (int) - indica si la salida es por Ctrl+D (0) o por comando exit (1)
    Retorno: ninguno
*/
void internal_exit(int key)
{
    // Diferenciamos entre si la salida es por Ctrl+D o por comando exit porque con Ctrl+D queremos imprimir un salto de línea antes del mensaje de salida

    if (key == 0)
    {

        sprintf(mensaje,"\nSaliendo con Ctrl+D");
        write(2, mensaje, strlen(mensaje));
        fflush(stdout);
        sleep(1);
        sprintf(mensaje,"\nTe echaremos de menos \U0001F972\n" RESET);
        write(2, mensaje, strlen(mensaje));
        fflush(stdout);
    }
    else
    {
        sprintf(mensaje,"Saliendo con exit");
        write(2, mensaje, strlen(mensaje));
        fflush(stdout);
        sleep(1);
        sprintf(mensaje,"\nTe echaremos de menos \U0001F972\n" RESET);
        write(2, mensaje, strlen(mensaje));
        fflush(stdout);
    }
    // Salimos del programa
    exit(0);
}

/*
    Funcion para ejecutar el comando interno cd
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: -1 si da error, 1 si se ejecuta correctamente.
*/
int internal_cd(char **args)
{

    // Si no hay segundo argumento, vamos al directorio HOME
    if (args[1] == NULL)
    {
        char *home = getenv("HOME");
        if (home != NULL)
        {
            int h = chdir(home);
            if (h != 0)
            {
                perror(ROJO NEGRITA "Error. cd: Error al cambiar de directorio");
                return -1;
            }
            #if DEBUGN2
                sprintf("Directorio cambiado a %s\n", getcwd(NULL, 0));
                write(2,mensaje, strlen(mensaje));
                fflush(stdout);
            #endif
            return 1;
        }
        return -1;
    }
    // Si hay segundo argumento, comprobamos si solo hay uno o varios

    // Si hay solo uno

    if (args[2] == NULL)
    {

        // Si el argumento es "..", subimos un directorio
        if (strcmp(args[1], "..") == 0)
        {

            int c = chdir("..");
            if (c != 0)
            {
                perror(ROJO NEGRITA "Error. cd: Error al cambiar de directorio");
                return -1;
            }
            #if DEBUGN2
                sprintf("Directorio cambiado a %s\n", getcwd(NULL, 0));
                write(2,mensaje, strlen(mensaje));
                fflush(stdout);
            #endif
            return 1;
        } // Si no, cambiamos al directorio indicado
        else
        {
            int c = chdir(args[1]);
            if (c != 0)
            {
                perror(ROJO NEGRITA "Error. cd: Error al cambiar de directorio");
                return -1;
            }
            #if DEBUGN2
                sprintf("Directorio cambiado a %s\n", getcwd(NULL, 0));
                write(2,mensaje, strlen(mensaje));
                fflush(stdout);
            #endif
            return 1;
        }

    }        // Si hay varios argumentos, comprobamos que todos son parte de una ruta correcta
    else
    {
        // Reservamos memoria para el string temporal donde construiremos la ruta con carácteres a eliminar como ", ' o '\' y otro para la ruta limpia
        char *concat = (char *)malloc(COMMAND_LINE_SIZE * sizeof(char));
        char *temp = (char *)malloc(COMMAND_LINE_SIZE * sizeof(char));
        if (temp == NULL || concat == NULL)
        {
            perror(ROJO NEGRITA "Error. cd :error en malloc");
            return -1;
        }

        // Ahora, concatenaremos los argumentos en un solo string
        strcpy(concat, "");
        for (int i = 1; args[i] != NULL; i++)
        {
            // Lo concatenamos y añadimos un espacio si no es el último
            strcat(concat, args[i]);
            if (args[i + 1] != NULL)
            {
                strcat(concat, " ");
            }
        }

        // Ahora, pasamos concat por la función limpiarRuta y luego se lo pasamos a chrdir
        limpiarRuta(concat, temp);
        int c = chdir(temp);
        if (c != 0)
        {
            perror(ROJO NEGRITA "Error. cd: Error al cambiar de directorio");
            return -1;
        }
        free(concat);
        free(temp);
        #if DEBUGN2
                sprintf("Directorio cambiado a %s\n", getcwd(NULL, 0));
                write(2,mensaje, strlen(mensaje));
                fflush(stdout);
        #endif
        return 1;
    }
}

/*
    Funcion para ejecutar el comando interno export
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: -1 si da error, 1 si se ejecuta correctamente.
*/
int internal_export(char **args)
{
    // Comprobamos que haya un argumento
    if (args[1] == NULL)
    {
        sprintf(mensaje, ROJO NEGRITA "Error de sintaxis. export: se requiere un argumento (NOMBRE==VALOR)\n" RESET);
        write(2, mensaje, strlen(mensaje));
        fflush(stdout);
        return -1;
    }
    // Si hay más de uno, error
    if (args[2] != NULL)
    {
        sprintf(mensaje, ROJO NEGRITA "Error de sintaxis. export: demasiados argumentos\n" RESET);
        write(2, mensaje, strlen(mensaje));
        fflush(stdout);
        return -1;
    }

    // Separamos el argumento en variable y valor y comprobamos que no sean nulos

    char *var = strtok(args[1], "=");
    char *value = strtok(NULL, "=");

    #if DEBUGN2
        sprintf(mensaje, "Comprobación antes de export: %s\nValor: %s\nVariable: %s\n", getenv(var));
        write(2,mensaje, strlen(mensaje));
        fflush(stdout);
    #endif
    if (value == NULL)
    {
        sprintf(mensaje, ROJO NEGRITA "Error de sintaxis. export: el valor es nulo\n" RESET);
        write(2,mensaje, strlen(mensaje));
        fflush(stdout);
        return -1;
    }

    // Establecemos la variable de entorno
    int k = setenv(var, value, 1);
    if (k != 0)
    {
        perror(ROJO NEGRITA "Error. export: error al establecer la variable de entorno");
        return -1;
    }

    #if DEBUGN2
        sprintf(mensaje, "Valor: %s\n", value);
        write(2,mensaje, strlen(mensaje));
        fflush(stdout);
    #endif
    return 1;
}

/*
    Funcion para ejecutar el comando interno source
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: -1 si da error, 1 si se ejecuta correctamente.
*/
int internal_source(char **args)
{
    // Comprobamos que haya un argumento y, si no, indicamos la sintaxis correcta
    if (args[1] == NULL)
    {
        sprintf(mensaje, ROJO NEGRITA "Error de sintaxis. source: source <nombre_fichero>\n" RESET);
        write(2,mensaje, strlen(mensaje));
        fflush(stdout);
        return -1;
    }

    // Reservamos memoria para el string temporal donde construiremos la ruta con carácteres a eliminar como ", ' o '\', otro para la ruta limpia y otro para el buffer.
    char *concat = (char *)malloc(COMMAND_LINE_SIZE * sizeof(char));
    char *rutalimp = (char *)malloc(COMMAND_LINE_SIZE * sizeof(char));
    char *buffer = (char *)malloc(COMMAND_LINE_SIZE * sizeof(char));

    if (rutalimp == NULL || concat == NULL || buffer == NULL)
    {
        perror(ROJO NEGRITA "Error. source: error en malloc");
        return -1;
    }

    // Ahora, concatenaremos los argumentos en un solo string
    strcpy(concat, "");
    for (int i = 1; args[i] != NULL; i++)
    {
        // Lo concatenamos y añadimos un espacio si no es el último
        strcat(concat, args[i]);
        if (args[i + 1] != NULL)
        {
            strcat(concat, " ");
        }
    }

    limpiarRuta(concat, rutalimp);

    // Pasamos la ruta limpia a fopen para que abra el archivo
    FILE *exFopen = fopen(rutalimp, "r");
    if (exFopen == NULL)
    {
        perror(ROJO NEGRITA "Error. source: ");
        return -1;
    }

    // Mientras no estemos al final del fichero, sustituimos el salto de linea y ejecutamos
    while (fgets(buffer, COMMAND_LINE_SIZE, exFopen) != NULL)
    {
        char *saltoLinea = strchr(buffer, '\n');
        fflush(exFopen);
        *saltoLinea = '\0';
        sprintf(mensaje, "\nComando: " SUBRAYADO "%s\n" RESET, buffer);
        write(2,mensaje, strlen(mensaje));
        fflush(stdout);
        execute_line(buffer);
    }

    // Cerramos el fichero y liberamos la memoria
    fclose(exFopen);
    free(concat);
    free(rutalimp);
    free(buffer);

    return 1;
}

/*
    Funcion para ejecutar el comando interno jobs
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: 1 si se ejecuta correctamente.
*/
int internal_jobs(char **args)
{
    // Imprimimos la lista de trabajos del background
    for (int i = 1; i < n_jobs + 1; i++)
    {
        printf("[%d] PID: %d, comando: %s, estado: %c \n", i, jobs_list[i].pid, jobs_list[i].comando, jobs_list[i].estado);
    }
    return 1;
}

/*
    Funcion para ejecutar el comando interno fg
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: -1 si da error, 1 si se ejecuta correctamente.
*/
int internal_fg(char **args)
{
    // Comprobamos la sintaxis del comando
    if (args[1] == NULL)
    {
        sprintf(mensaje, ROJO NEGRITA "Error de sintaxis. fg: se requiere un argumento (fg valor)\n" RESET);
        write(2,mensaje, strlen(mensaje));
        fflush(stdout);
        return -1;
    }

    if (args[2] != NULL)
    {
        sprintf(mensaje, ROJO NEGRITA "Error de sintaxis. fg: demasiados argumentos\n" RESET);
        write(2,mensaje, strlen(mensaje));
        fflush(stdout);
        return -1;
    }

    //Obtenemos la posición del trabajo
    int pos;
    pos = atoi(args[1]);
    //Si no es una posición válida, error
    if (pos > n_jobs || pos <= 0)
    {
        sprintf(mensaje, ROJO NEGRITA "Error de sintaxis. fg: posición del trabajo no válida\n" RESET);
        write(2, mensaje, strlen(mensaje));
        fflush(stdout);
        return -1;
    }

    // Pasamos el trabajo a foreground
    jobs_list[0].pid = jobs_list[pos].pid;
    //De esta manera, todos los procesos pasarán a tener estado E, tuvieran D o no.
    jobs_list[0].estado = 'E';
    strcpy(jobs_list[0].comando, jobs_list[pos].comando);

    // Eliminamos el & del final del comando si lo tiene
    int len = strlen(jobs_list[0].comando);
    if (jobs_list[0].comando[len - 1] == '&')
    {
        jobs_list[0].comando[len - 1] = '\0';
    }
    
    // Lo eliminamos de la lista de trabajos
    jobs_list_remove(jobs_list[pos].pid);
    sprintf(mensaje, "PID %d, comando: %s\n", jobs_list[0].pid, jobs_list[0].comando);
    write(2, mensaje, strlen(mensaje));
    fflush(stdout);
    //Reanudamos el proceso
    kill(jobs_list[0].pid, SIGCONT);
    // Esperamos a que termine
    while (jobs_list[0].pid != 0)
    {
        pause();
    }
    return 1;
}

/*
    Funcion para ejecutar el comando interno bg
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: -1 si da error, 1 si se ejecuta correctamente.
*/
int internal_bg(char **args)
{
    // Comprobamos la sintaxis del comando
    if (args[1] == NULL)
    {
        sprintf(mensaje, ROJO NEGRITA "Error de sintaxis. bg: se requiere un argumento (bg valor)\n" RESET);
        write(2,mensaje, strlen(mensaje));
        fflush(stdout);
        return -1;
    }

    if (args[2] != NULL)
    {
        sprintf(mensaje, ROJO NEGRITA "Error de sintaxis. bg: demasiados argumentos\n" RESET);
        write(2,mensaje, strlen(mensaje));
        fflush(stdout);
        return -1;
    }
    //Obtenemos la posición del trabajo
    int pos;
    pos = atoi(args[1]);
    //Si no es una posición válida, error
    if (pos > n_jobs || pos <= 0)
    {
        sprintf(mensaje, ROJO NEGRITA "Error de sintaxis. bg: posición del trabajo no válida\n" RESET);
        write(2,mensaje, strlen(mensaje));
        fflush(stdout);
        return -1;
    }

    // Si está en ejecución, error
    if (jobs_list[pos].estado == 'E')
    {
        sprintf(mensaje, ROJO NEGRITA "Error de sintaxis. bg: el trabajo ya está en ejecución\n" RESET);
        write(2,mensaje, strlen(mensaje));
        fflush(stdout);
        return -1;
    }
    //Cambiamos el estado a ejecutándose
    jobs_list[pos].estado = 'E';
    sprintf(mensaje, "[%d] PID %d, comando: %s, estado: %c\n", pos, jobs_list[pos].pid, jobs_list[pos].comando, jobs_list[pos].estado);
    write(2, mensaje, strlen(mensaje));
    fflush(stdout);
    //Reanudamos el proceso 
    kill(jobs_list[pos].pid, SIGCONT);
    return 1;
}

/*
    Funcion para comprobar si el comando es interno y ejecutarlo
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: 1 si el comando es interno y se ha ejecutado, 0 si no lo es.
*/
int check_internal(char **args)
{

    // Comprobamos que vale el primer argumento y, si es uno de los comandos internos, lo ejecutamos
    if (strcmp(args[0], "cd") == 0)
    {
        internal_cd(args);
        return 1;
    }
    else if (strcmp(args[0], "export") == 0)
    {
        internal_export(args);
        return 1;
    }
    else if (strcmp(args[0], "source") == 0)
    {
        internal_source(args);
        return 1;
    }
    else if (strcmp(args[0], "jobs") == 0)
    {
        internal_jobs(args);
        return 1;
    }
    else if (strcmp(args[0], "fg") == 0)
    {
        internal_fg(args);
        return 1;
    }
    else if (strcmp(args[0], "bg") == 0)
    {
        internal_bg(args);
        return 1;
    }
    else if (strcmp(args[0], "exit") == 0)
    {
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
int parse_args(char **args, char *line)
{
    int i = 0;
    char *token = strtok(line, " ");

    // Si no hay tokens, devolvemos 0
    if (token == NULL)
    {
        args[0] = NULL;
        // printf("Token 0: NULL\n");
        return 0;
    }

    do
    {
        if (token[0] == '#')
        { // Si encontramos un comentario, lo ignoramos y no seguimos parseando
            token = NULL;
        }
        else
        //Si no es un comentario, lo guardamos en args y pasamos al siguiente token
        {
            args[i] = token;
            
            i++;
            token = strtok(NULL, " ");
        }
    } while (token != NULL);
    args[i] = NULL;
    #if DEBUGN1
        sprintf(mensaje, "Cadena de tokens: %s\nCantidad de tokens: %d\n", args);
        write(2,mensaje, strlen(mensaje));
        fflush(stdout);
    #endif
    return i;
}

/*
    Función para leer una línea de comandos
    Parametros: line (char*) - line donde se guardará la línea leída
    Retorno: línea leída (char*)
*/
char *read_line(char *line)
{

    // Imprimimos el prompt, hacemos un flush para que se vea inmediatamente
    imprimir_prompt();
    sleep(0.5);
    fflush(stdout);

    // Leemos la linea

    char *buffer = fgets(line, COMMAND_LINE_SIZE, stdin);
    if (buffer != NULL)
    {
        //Eliminamos el salto de linea
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
        {
            buffer[len - 1] = '\0'; // Eliminamos solo si es salto de línea
        }
        return buffer;
    }
    else
    {
        //Si es un Ctrl+D
        if (feof(stdin))
        {
            internal_exit(0);
        }
    }
}

/*
    Función para ejecutar una línea de comandos
    Parametros: line (char*) - línea de comandos a ejecutar
    Retorno: 0 si no hay argumentos, -1 si da error, 1 si se ejecuta correctamente.
*/
int execute_line(char *line)
{
    // Copiamos la línea para tenerla sin modificar
    char *copyLine = (char *)malloc(COMMAND_LINE_SIZE * sizeof(char));
    if (copyLine == NULL)
    {
        perror(ROJO NEGRITA "Error. execute_line: error en malloc");
        return -1;
    }
    strcpy(copyLine, line);

    char *args[COMMAND_LINE_SIZE];
    //Parseamos los argumentos
    int exParse = parse_args(args, line);
    // Si no hay argumentos, devolvemos 0
    if (exParse == 0)
    {
        return 0;
    }
    //Comprobamos si es un proceso en background
    int foreground = is_background(args);
    //Comprobamos si es un proceso interno
    int exCheckInternal = check_internal(args);

    // Si no es interno, nos vamos a ejecutar el comando como externo
    if (exCheckInternal == 0)
    {
        int exFork = fork();
        if (exFork < 0)
        {
            perror(ROJO NEGRITA "Error. execute_line: error en fork");
            return -1;
        }

        // Si es el proceso hijo, ejecutamos el comando y hacemos que omita las señales SIGINT y SIGTSTP
        if (exFork == 0)
        {

            signal(SIGINT, SIG_IGN);
            signal(SIGTSTP, SIG_IGN);

             #if DEBUGN3
                sprintf(mensaje, "PID del hijo: %d\nComando en ejecución: %s\n", getpid());
                write(2,mensaje, strlen(mensaje));
                fflush(stdout);
            #endif
            //Si hay redirección del output
            is_output_redirection(args);
            //Ejecutamos el comando
            int exExecvp = execvp(args[0], args);
            if (exExecvp < 0)
            {
                perror(NEGRITA ROJO "Error en execvp");
                printf(RESET);
                exit(-1);
            }
            dup2(STDOUT_FILENO, 1); // Restauramos la salida estándar
        }
        // Si es el padre
        else
        {
            // Si es foreground, esperamos a que termine
            if (foreground == 0)
            {
                #if DEBUGN5
                    sprintf(mensaje, "Ejecutando en foreground\n", getpid());
                    write(2,mensaje, strlen(mensaje));
                    fflush(stdout);
                #endif
                #if DEBUGN3
                    sprintf(mensaje, "PID del padre: %d\n", getpid());
                    write(2,mensaje, strlen(mensaje));
                    fflush(stdout);
                #endif
                jobs_list[0].pid = exFork;
                jobs_list[0].estado = 'E';
                strcpy(jobs_list[0].comando, copyLine);

                while (jobs_list[0].pid != 0)
                {
                    pause();
                }
            }
            // Si es background, añadimos el trabajo a la lista de trabajos
            else
            {
                #if DEBUGN5
                    sprintf(mensaje, "Ejecutando en background\n", getpid());
                    write(2,mensaje, strlen(mensaje));
                    fflush(stdout);
                #endif
                #if DEBUGN3
                    sprintf(mensaje, "PID del padre: %d\n", getpid());
                    write(2,mensaje, strlen(mensaje));
                    fflush(stdout);
                #endif
                jobs_list_add(exFork, copyLine, 'E');
            }
        }
    }
    free(copyLine);
    return 1;
}

/*
    Función para manejar la señal SIGCHLD y actualizar la lista de trabajos
    Parametros: signum (int) - número de señal
    Retorno: ninguno
*/
void reaper(int signum)
{
    signal(SIGCHLD, reaper);
    pid_t ended;
    int status;

    // Esperamos a que termine cualquier proceso hijo
    while ((ended = waitpid(-1, &status, WNOHANG)) > 0)
    {
        // Si el proceso que ha terminado es el de foreground
        if (ended == jobs_list[0].pid)
        { 

            if (WIFEXITED(status))
            {
                #if DEBUGN3||DEBUGN4
                sprintf(mensaje,
                        "Proceso en foreground (PID: %d, comando: %s, estado: %c) ha terminado con el estado: %d\n",
                        jobs_list[0].pid, jobs_list[0].comando, jobs_list[0].estado, WEXITSTATUS(status));
                write(2, mensaje, strlen(mensaje));
                fflush(stdout);
                #endif
            }
            else if (WIFSIGNALED(status) && DEBUGN4)
            {
                #if DEBUGN4||DEBUGN5||DEBUGN6
                sprintf(mensaje,
                        "\nProceso en foreground (PID: %d, comando: %s, estado: %c) ha recibido la señal: %d\n",
                        jobs_list[0].pid, jobs_list[0].comando, jobs_list[0].estado, WTERMSIG(status));
                write(2, mensaje, strlen(mensaje));
                fflush(stdout);
                #endif
            }

            // Limpiamos el foreground
            jobs_list[0].pid = 0;
            jobs_list[0].estado = 'N';
            for (int i = 0; i < COMMAND_LINE_SIZE; i++)
                jobs_list[0].comando[i] = '\0';
        }
        //Si se trata de un proceso en background
        else
        {
            //Lo buscamos en la lista de trabajos
            int index = jobs_list_find(ended);

            if (index != -1)
            {
                //Si ha acabado con normalidad
                if (WIFEXITED(status))
                {
                    jobs_list[index].estado = 'F';
                    sprintf(mensaje,
                            "\nProceso en background (PID: %d, comando: %s, estado: %c) ha terminado con el estado: %d\n",
                            jobs_list[index].pid, jobs_list[index].comando, jobs_list[index].estado, WEXITSTATUS(status));

                    write(1, mensaje, strlen(mensaje));
                    fflush(stdout);
                }
                //Si ha recibido una señal
                else if (WIFSIGNALED(status))
                {
                    sprintf(mensaje,
                            "\nProceso en background (PID: %d, comando: %s, estado: %c) ha recibido la señal: %d\n",
                            jobs_list[index].pid, jobs_list[index].comando, jobs_list[index].estado, WTERMSIG(status));

                    write(2, mensaje, strlen(mensaje));
                    fflush(stdout);
                }
                //Lo eliminamos de la lista de trabajos
                jobs_list_remove(ended);
            }
        }
    }
}

/*
    Función para manejar la señal SIGINT (Ctrl+C) y terminar el proceso en foreground
    Parametros: signum (int) - número de señal
    Retorno: ninguno
*/
void ctrlc(int signum)
{
    // Reasignamos el manejador de la señal SIGINT
    signal(SIGINT, ctrlc);

    // Si hay un proceso en foreground
    if (jobs_list[0].pid > 0)
    {
        // Si el proceso en foreground no es el shell, lo terminamos
        if (strcmp(jobs_list[0].comando, mi_shell) != 0)
        {
            kill(jobs_list[0].pid, SIGTERM);
        }
        // Si el proceso en foreground es el shell, no hacemos nada y mostramos un mensaje
        else
        {
            sprintf(mensaje, "\n" ROJO NEGRITA "No se puede terminar el shell con Ctrl+C" RESET);
            write(2, mensaje, strlen(mensaje));
            fflush(stdout);
        }
    }
    // Si no hay proceso en foreground, mostramos un mensaje de error
    else
    {
        sprintf(mensaje, "\n" ROJO NEGRITA "No hay ningún proceso en foreground para terminar\n" RESET);
        write(2, mensaje, strlen(mensaje));
        fflush(stdout);
    }
}

/*
    Función para manejar la señal SIGTSTP (Ctrl+Z) y detener el proceso en foreground
    Parametros: signum (int) - número de señal
    Retorno: ninguno
*/
void ctrlz(int signum)
{

    signal(SIGTSTP, ctrlz);
    // Mensaje para imprimir el proceso que ha sido detenido con write

    if (jobs_list[0].pid > 0)
    {
        // Si el proceso en foreground no es el shell, lo detenemos
        if (strcmp(jobs_list[0].comando, mi_shell) != 0)
        {
            kill(jobs_list[0].pid, SIGSTOP);
            write(1, "\n", 1);
            fflush(stdout);
            // Añadimos el proceso detenido a la lista de trabajos
            jobs_list_add(jobs_list[0].pid, jobs_list[0].comando, 'D');
            // Limpiamos el proceso en foreground
            jobs_list[0].pid = 0;
            jobs_list[0].estado = 'N';
            for (int i = 0; i < COMMAND_LINE_SIZE; i++)
            {
                jobs_list[0].comando[i] = '\0';
            }
            sprintf(mensaje, "Proceso detenido y pasado a background: PID %d, comando: %s\n", jobs_list[0].pid, jobs_list[0].comando);
            write(2, mensaje, strlen(mensaje));
            fflush(stdout);
        }
        // Si el proceso en foreground es el shell, no hacemos nada y mostramos un mensaje
        else
        {
            sprintf(mensaje, "\n" ROJO NEGRITA "No se puede detener el shell con Ctrl+Z\n" RESET);
            write(2, mensaje, strlen(mensaje));
            fflush(stdout);
        }
    }
    else
    {
        sprintf(mensaje, "\n" ROJO NEGRITA "No hay ningún proceso en foreground para detener\n" RESET);
        write(2, mensaje, strlen(mensaje));
        fflush(stdout);
    }
}

/*
    Función para comprobar si el comando se debe ejecutar en background
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: 1 si es en background, 0 si no lo es.
*/
int is_background(char **args)
{
    // Recorremos los argumentos hasta encontrar el último
    int i = 0;
    while (args[i] != NULL)
    {
        i++;
    }
    // Si el último es "&", lo eliminamos y devolvemos 1
    if (i > 0 && strcmp(args[i - 1], "&") == 0)
    {
        args[i - 1] = NULL;
        return 1;
    }
    return 0;
}

/*
    Función para añadir un trabajo a la lista de trabajos
    Parametros: pid (pid_t) - PID del proceso, comando (char*) - comando ejecutado, estado (char) - estado del trabajo
    Retorno: número de trabajos en la lista, -1 si no se puede añadir.
*/
int jobs_list_add(pid_t pid, char *comando, char estado)
{
    // Comprobamos que no se haya alcanzado el máximo de trabajos
    if (n_jobs + 1 >= N_JOBS)
    {
        sprintf(mensaje, ROJO NEGRITA "Error. jobs_list_add: número máximo de trabajos alcanzado\n" RESET);
        write(2, mensaje, strlen(mensaje));
        fflush(stdout);
        return -1;
    }
    n_jobs++;
    // Añadimos el trabajo a la lista
    jobs_list[n_jobs].pid = pid;
    strcpy(jobs_list[n_jobs].comando, comando);
    jobs_list[n_jobs].estado = estado;
    printf("[%d] PID %d, comando: %s, estado: %c\n", n_jobs, pid, comando, estado);
    fflsuh(stdout);
    return n_jobs;
}

/*
    Función para buscar un trabajo en la lista de trabajos por su PID
    Parametros: pid (pid_t) - PID del proceso a buscar
    Retorno: índice del trabajo en la lista, -1 si no se encuentra.
*/
int jobs_list_find(pid_t pid)
{

    // Recorremos la lista de trabajos buscando el PID
    for (int i = 0; i <= n_jobs; i++)
    {
        if (jobs_list[i].pid == pid)
        {
            return i;
        }
    }
    return -1;
}

/*
    Función para eliminar un trabajo de la lista de trabajos por su PID
    Parametros: pid (pid_t) - PID del proceso a eliminar
    Retorno: número de trabajos en la lista, -1 si no se encuentra.
*/
int jobs_list_remove(pid_t pid)
{
    // Buscamos el trabajo en la lista
    int index = jobs_list_find(pid);
    // Si no se encuentra, devolvemos -1
    if (index == -1)
    {
        return -1;
    }
    // Desplazamos los trabajos posteriores una posición hacia atrás
    for (int i = index; i < n_jobs; i++)
    {
        jobs_list[i] = jobs_list[i + 1];
    }
    //Como hemos quitado un trabajo, reducimos por uno el n_jobs
    n_jobs--;
    return n_jobs;
}

/*
    Función para comprobar si hay redirección de salida en los argumentos
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: 1 si hay redirección de salida, 0 si no la hay o -1 si ">" está en la primera posición.
*/
int is_output_redirection(char **args)
{
    int i = 0;
    //Mientras no lleguemos al final, vamos comprobando si hay un >
    while (args[i] != NULL)
    {
        //Si lo hay, y no está en la posición 0, redireccionamos
        if (strcmp(args[i], ">") == 0)
        {
            //Si está en la posición 0, enviamos un mensaje de error
            if (i == 0)
            {
                sprintf(mensaje, ROJO NEGRITA "Error. is_output_redirection: sintaxis incorrecta\n" RESET);
                write(2, mensaje, strlen(mensaje));
                fflush(stdout);
                return -1;
            }
            int j = i;
            // Desplazamos los argumentos posteriores una posición hacia atrás para eliminar el símbolo de red
            while (args[j] != NULL)
            {
                args[j] = args[j + 1];
                j++;
            }
            //Ahora, limpiamos la ruta que se encuentra en args[i] (recordemos que args[i]=> y args[i+1] = fichero, pero como hemos desplazado todo a la izquierda una posición, ahora args[i]=fichero)
            char *temp = args[i];
            char *limpia = malloc(COMMAND_LINE_SIZE * sizeof(char *));
            limpiarRuta(temp, limpia);

            // Abrimos el archivo de redirección
            int fd = open(limpia, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            if (fd < 0)
            {
                perror(ROJO NEGRITA "Error. is_output_redirection: error al abrir el archivo de redirección");
                return -1;
            }
            // Redirigimos la salida estándar al archivo
            if (dup2(fd, 1) < 0)
            {
                perror(ROJO NEGRITA "Error. is_output_redirection: error al redirigir la salida estándar");
                return -1;
            }
            close(fd);
            free(limpia);
            // Desplazamos los argumentos posteriores una posición hacia atrás para eliminar el nombre del archivo
            j = i;
            while (args[j] != NULL)
            {
                args[j] = args[j + 1];
                j++;
            }
        }
        else
        {
            i++;
        }
    }
    return 1;
}

/*
    Función principal del minishell
    Inicializa señales, el foreground, y ejecuta un bucle infinito de lectura y ejecución de comandos
    Parametros: argc, argv
    Retorno: ninguno
*/
void main(int argc, char *argv[])
{
    char line[COMMAND_LINE_SIZE];
    
    // Asignamos a las señales a sus manejadores correspondientes
    signal(SIGINT, ctrlc);
    signal(SIGCHLD, reaper);
    signal(SIGTSTP, ctrlz);

    // Inicializamos el primer proceso a 0.
    jobs_list[0].pid = 0;
    jobs_list[0].estado = 'N';
    for (int i = 0; i < COMMAND_LINE_SIZE; i++)
    {
        jobs_list[0].comando[i] = '\0';
    }

    // Cogemos el string para ejecutar el shell a partir de argv[0]
    strcpy(mi_shell, argv[0]);
    #if DEBUGN3
        sprintf("Nombre del programa shell : %s\n", mi_shell);
        write(2,mensaje, strlen(mensaje));
        fflush(stdout);
    #endif
    // Realizamos un bucle infinito para el shell
    while (1)
    {
        if (read_line(line))
        {
            execute_line(line);
        }
    }
}
