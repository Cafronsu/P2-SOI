#include "minishell.h"

/*
    Función para imprimir el prompt con el usuario y el directorio actual
    Parametros: ninguno
    Retorno: ninguno
*/

void imprimir_prompt()
{
    char *user = getenv("USER");
    char *directorio = getcwd(NULL, 0);
    printf(NEGRITA AMARILLO"%s" RESET":" NEGRITA CIAN"%s"RESET"$ ", user, directorio);
}

/*
    Función para limpiar de comillas dobles, simples y \ la ruta introducida por parámetro
    Parametros: rutaSucia (char*) - ruta a limpiar y rutaLimpia (char*) - ruta limpia
    Retorno: ninguno
*/

void limpiarRuta(char* rutaSucia, char* rutaLimpia){

    //Recorremos la ruta sucia y miramos cada carácter:
    //Si es una \ seguida de un espacio, añadimos un espacio a la ruta limpia y saltamos el siguiente carácter
    //Si es una comilla simple o doble, la ignoramos
    //Si no, añadimos el carácter a la ruta limpia
    int j = 0;
    int i = 0;
    while(rutaSucia[j] != '\0'){
        if(rutaSucia[j] == '\\' && rutaSucia[j+1] == ' '){
            rutaLimpia[i] = ' ';
            i++;
            j+=2;
        }else if(rutaSucia[j] == '\"' || rutaSucia[j] == '\''){
            j++;
        }else{
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

    if (key == 0)
    {

        printf("\nSaliendo con Ctrl+D");

        // Este trozo de código es para hacer una "animación" de puntos suspensivos
        /*
        for (int i = 0; i < 3; i++)
        {
            fflush(stdout);
            printf(".");
            sleep(1);
        }
        */
        

        fflush(stdout);
        sleep(1);
        printf("\nTe echaremos de menos \U0001F972\n"RESET);
    }
    else
    {
        printf("Saliendo con exit");

        /*
        for (int i = 0; i < 3; i++)
        {
            fflush(stdout);
            printf(".");
            sleep(1);
        }
        */

        fflush(stdout);
        sleep(1);
        printf("\nTe echaremos de menos \U0001F972\n" RESET);
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
            }

        } // Si no, cambiamos al directorio indicado
        else
        {
            int c = chdir(args[1]);
            if (c != 0)
            {
                perror(ROJO NEGRITA"Error. cd: Error al cambiar de directorio");
            }
        }

        // Si hay varios argumentos, comprobamos si es uno de los formatos válidos
    }
    else
    {
        // Reservamos memoria para el string temporal donde construiremos la ruta con carácteres a eliminar como ", ' o '\' y otro para la ruta limpia

            char *concat = (char *)malloc(COMMAND_LINE_SIZE * sizeof(char));
            char *temp = (char *)malloc(COMMAND_LINE_SIZE * sizeof(char));
            if (temp == NULL|| concat == NULL)
            {
                perror(ROJO NEGRITA"Error. cd :error en malloc");
                return -1;
            }
            
        //Ahora, concatenaremos los argumentos en un solo string
            strcpy(concat, "");
            for (int i = 1; args[i] != NULL; i++)
            {
                //Lo concatenamos y añadimos un espacio si no es el último
                strcat(concat, args[i]);
                if (args[i + 1] != NULL)
                {
                    strcat(concat, " ");
                }
            }

        //Ahora, pasamos concat por la función limpiarRuta y luego se lo pasamos a chrdir
            limpiarRuta(concat, temp);
            int c = chdir(temp);
            if (c != 0)
            {
                perror(ROJO NEGRITA"Error. cd: Error al cambiar de directorio");
                return -1;
            }
        free(concat);
        free(temp);
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
        printf(ROJO NEGRITA"Error de sintaxis. export: se requiere un argumento (NOMBRE==VALOR)\n"RESET);
        return -1;
    }
    // Si hay más de uno, error
    if (args[2] != NULL)
    {
        printf(ROJO NEGRITA"Error de sintaxis. export: demasiados argumentos\n"RESET);
        return -1;
    }

    // Separamos el argumento en variable y valor
    char *var = strtok(args[1], "=");
    char *check = getenv(var);

    printf("Comprobación antes de export: %s\n", check);
    char *value = strtok(NULL, "=");

    printf("Variable: %s\n", var);
    printf("Valor: %s\n", value);

    // Comprobamos que variable y valor no sean nulos
    if (var == NULL)
    {
        printf(ROJO NEGRITA"Error de sintaxis. export: el nombre es nulo\n"RESET);
        return -1;
    }
    if (value == NULL)
    {
        printf(ROJO NEGRITA"Error de sintaxis. export: el valor es nulo\n"RESET);
        return -1;
    }

    // Establecemos la variable de entorno
    int k = setenv(var, value, 1);
    if (k != 0)
    {
        perror(ROJO NEGRITA"Error. export: error al establecer la variable de entorno");
        return -1;
    }

    check = getenv(var);
    printf("Comprobación después de export: %s\n", check);
    return 1;
}

/*
    Funcion para ejecutar el comando interno source
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: -1 si da error, 1 si se ejecuta correctamente.
*/

int internal_source(char **args)
{
    //Comprobamos que haya un argumento y, si no, indicamos la sintaxis correcta
    if(args[1]==NULL){
        printf(ROJO NEGRITA"Error de sintaxis. source: source<nombre_fichero>\n"RESET);
        return -1;
    }

    //Reservamos memoria para el string temporal donde construiremos la ruta con carácteres a eliminar como ", ' o '\', otro para la ruta limpia y otro para el buffer.
    char* concat = (char*)malloc(COMMAND_LINE_SIZE*sizeof(char));
    char* rutalimp = (char*)malloc(COMMAND_LINE_SIZE*sizeof(char));
    char* buffer = (char*)malloc(COMMAND_LINE_SIZE*sizeof(char));

    if(rutalimp==NULL || concat==NULL || buffer==NULL){
        perror(ROJO NEGRITA"Error. source: error en malloc");
        return -1;
    }

    //Ahora, concatenaremos los argumentos en un solo string
            strcpy(concat, "");
            for (int i = 1; args[i] != NULL; i++)
            {
                //Lo concatenamos y añadimos un espacio si no es el último
                strcat(concat, args[i]);
                if (args[i + 1] != NULL)
                {
                    strcat(concat, " ");
                }
            }

    limpiarRuta(concat, rutalimp);

    //Pasamos la ruta limpia a fopen para que abra el archivo
    FILE* exFopen = fopen(rutalimp,"r");
    if(exFopen==NULL){
        perror(ROJO NEGRITA"Error. source: ");
        return -1;
    }

    //Mientras no estemos al final del fichero, sustituimos el salto de linea y ejecutamos
    while(fgets(buffer,COMMAND_LINE_SIZE,exFopen)!=NULL){
        char* saltoLinea = strchr(buffer, '\n');
        fflush(exFopen);
        *saltoLinea = '\0';
        printf("\nComando: "SUBRAYADO"%s\n"RESET, buffer);
        fflush(stdout);
        execute_line(buffer);
    }

    //Cerramos el fichero y liberamos la memoria
    fclose(exFopen);
    free(concat);
    free(rutalimp);
    free(buffer);

    return 1;
}

/*
    Funcion para ejecutar el comando interno jobs
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: por ahora nada.
*/

int internal_jobs(char **args)
{
    printf("Ejecutando jobs\n");
    return 1;
}

/*
    Funcion para ejecutar el comando interno fg
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: por ahora nada.
*/

int internal_fg(char **args)
{
    printf("Ejecutando fg\n");
    return 1;
}

/*
    Funcion para ejecutar el comando interno bg
    Parametros: args (char**) - array de strings con los argumentos del comando
    Retorno: por ahora nada.
*/

int internal_bg(char **args)
{
    printf("Ejecutando bg\n");
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
        return 0;
    }
    
    do
    {
        if (token[0] == '#')
        { // Si encontramos un comentario, lo ignoramos y no seguimos parseando
            token = NULL;
        }
        else
        {
            args[i] = token;
            i++;
            token = strtok(NULL, " ");
        }
    } while (token != NULL);
    args[i] = NULL;
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
        size_t len = strlen(buffer);
        if (len > 0)
        {
            buffer[len - 1] = '\0'; // Eliminamos el salto de linea
        }
        return buffer;
    }
    else
    {
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
    //Copiamos la línea para tenerla sin modificar
    char *copyLine = (char *)malloc(COMMAND_LINE_SIZE * sizeof(char));
    if (copyLine == NULL)
    {
        perror(ROJO NEGRITA"Error. execute_line: error en malloc");
        return -1;
    }
    strcpy(copyLine, line);

    char *args[COMMAND_LINE_SIZE];
    int exParse = parse_args(args, line);

    //Si no hay argumentos, devolvemos 0
    if (exParse == 0)
    {
        return 0;
    }
        int exCheckInternal = check_internal(args);

        //Si no es interno, nos vamos a ejecutar el comando como externo
        if(exCheckInternal == 0){
            int exFork = fork();
            if(exFork < 0){
                perror(ROJO NEGRITA"Error. execute_line: error en fork");
                return -1;
            }
            
            //Si es el proceso hijo, ejecutamos el comando
            if (exFork == 0){
                printf("PID del hijo: %d\n", getpid());
                int exExecvp = execvp(args[0], args);
                if(exExecvp < 0){
                    printf("Código de error: %d\n", errno);
                    exit(-1);
                }
            }
            //Si es el proceso padre, esperamos a que termine el hijo
            else{
                jobs_list[0].pid = exFork;
                strcpy(jobs_list[0].comando, copyLine);
                jobs_list[0].estado = 'E';
                int status;

                printf("PID del padre: %d\n", getpid());

                waitpid(exFork, &status, 0);

                printf("El proceso hijo ha terminado con estado: %d\n", WEXITSTATUS(status)); 

                jobs_list[0].estado = 'N';
                jobs_list[0].pid = 0;
                for (int i = 0; i < COMMAND_LINE_SIZE; i++)
                {
                    jobs_list[0].comando[i] = '\0';
                }
            }
        }
    free(copyLine);
    return 1;
}

void main(int argc, char *argv[])
{
    char line[COMMAND_LINE_SIZE];

    //Inicializamos el primer proceso a 0.
    jobs_list[0].pid = 0;
    jobs_list[0].estado = 'N';
    for (int i = 0; i < COMMAND_LINE_SIZE; i++)
    {
        jobs_list[0].comando[i] = '\0';
    }

    //Cogemos el string para ejecutar el shell a partir de argv[0]
    strcpy(mi_shell, argv[0]);

    // Realizamos un bucle infinito para el shell
    while (1)
    {
        if (read_line(line))
        {
            execute_line(line);
        }
    }
}
