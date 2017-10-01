/*
 * Proyecto I - Sistemas Operativos
 * Gabriela Garro Abdykerimov
 * 1 Octubre 2017
 *
 * Para compilar: gcc -pthread -o grep grep.c queue.c 
 *
 * Uso del programa:
 * ./grep [opciones] [expresión regular] [archivos|directorio]
 * Opciones:
 * * -l: Solo muestra el nombre del archivo
 * * -r: Busca recursivamente en el directorio
 * Expresión regular:
 * * Ver http://pubs.opengroup.org/onlinepubs/7908799/xbd/re.html
 *   para más información
 * Archivos / Directorio:
 * * Uno o más archivos/directorios, separado por espacios
 */

#include <stdlib.h> 
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>
#include <pthread.h> 
#include <regex.h>

#include "queue.h"

#define MAX_NUM_THREADS 8
#define MAX_NUM_FILES 10

// Variables globales
bool arg_l = false;
bool arg_r = false;
char * regular_exp;
char * files[MAX_NUM_FILES];
int num_files;

//Declaración de mutex
pthread_mutex_t mutex;

/* Imprimir un error y salir del programa */
void print_error(char * error) {
	printf("ERROR: %s\n", error);
	exit(-1);
}

/* Analizar los argumentos */
int get_args(int args, char* argv[]) {
	// Revisar cantidad min de argumentos
	if (args < 3) {
		print_error("Cantidad insuficiente de argumentos.");
	}
	printf("Argumentos:\n");
	int arg_num = 1; // Contador de argumentos
	// Recuperar banderas
	while (arg_num <= 2) {
		if (strcmp(argv[arg_num], "-r") == 0) {
			printf("\t-r: Buscar recursivamente.\n");
			arg_r = true;
			arg_num++;
		}
		else if (strcmp(argv[arg_num], "-l") == 0) {
			printf("\t-l: Imprimir solo nombre de archivo.\n");
			arg_l = true;
			arg_num++;
		}
		else {
			break;
		}
	}
	// A partir de aquí pueden haber errores de argumentos fuera de límites
	// Chequear si quedan mínimo dos argumentos más
	if (args < arg_num + 1) {
		print_error("Cantidad insuficiente de argumentos.");
	} 
	// Recuperar expresión regular
	regular_exp = argv[arg_num];
	printf("\tExpresión regular: %s\n", regular_exp);
	arg_num++;
	// Revisar si hay más archivos / directorios de los esperados
	if (args - arg_num - 1 > MAX_NUM_FILES) {
		printf("Se esperaba un máximo de %i archivos, se recibieron %i.\n", MAX_NUM_FILES, args - arg_num - 1);
		print_error("Excedida la cantidad máxima de archivos.");
	}
	// Recuperar archivos / directorios
	int i;
	printf("\tArchivos / Directorios: ");
	for (i = 0; arg_num < args; arg_num++, i++) {
		files[i] = argv[arg_num];
		printf("%s, ", files[i]);
	}
	num_files = i;
	printf("\n\n");
	return 0;
}

/* Obtiene como entrada un nombre de directorio / archivo
 * y añade los nombres de los archivos internos a una cola. 
 * Puede comportarse recursivamente si arg_r = true.
 */
void add_files(char * object, bool check_once) {
	DIR * dir;
	struct dirent * entry;
	if (!(dir = opendir(object)))
		return;
	// Mientras el objeto existe
	while((entry = readdir(dir)) != NULL && !check_once) {
		// Si es un directorio
		if (entry->d_type == DT_DIR) {
			char path[1024];
			// Ignorar estos dos
			if (strcmp(entry->d_name, ".") == 0||
				strcmp(entry->d_name, "..") == 0)
				continue;
			//Actualizar el nombre del path para buscar adentro
			snprintf(path, sizeof(path), "%s/%s", object, entry->d_name);
			//printf("[%s]\n", entry->d_name);
            if (arg_r) add_files(path, false);
            else add_files(path, true);
		}
		else {
			char path[1024];
			snprintf(path, sizeof(path), "%s/%s", object, entry->d_name);
			//printf("%s\n", path);
			char * full_path = path;
			//printf("%s\n", full_path);
			queue_push(full_path);
		}
	}
	closedir(dir);
}

/* Lo que ejecuta cada thread */
void * thread_exec(void * _thread_arg) {
	int * thread_arg = (int *) _thread_arg;
	int thread_num = (* thread_arg) - 1;
	//printf("Hola soy el thread no. %i\n", thread_num);

	char * file;
	while (1) {
		pthread_mutex_lock(&mutex);
		if (queue_is_void()) {
			pthread_mutex_unlock(&mutex);
			pthread_exit(NULL);
		}
		else {
			file = queue_pop();
		}
		pthread_mutex_unlock(&mutex);
		printf("%i: %s\n", thread_num, file);
	}
	pthread_exit(NULL);
}

/* Buscar las expresiones regulares en los archivos */
int search_regex() {

	return 0;
}

int main(int args, char* argv[]) {
	// Obtener valores de argumentos
	get_args(args, argv);
	// Crear cola de archivos
	queue_init();
	int i;
	for (i = 0; i < num_files; i++) {
		add_files(files[i], false);
	}
	// Crear threads
	pthread_t threads[MAX_NUM_THREADS];
	int thread_id;
	for (i = 0; i < MAX_NUM_THREADS; i++) {
		thread_id = pthread_create(&threads[i], NULL, thread_exec, &i);
		if (thread_id) {
			printf("ERROR: Return code from pthread_create() is %d.\n", thread_id);
			exit(-1);
		}
	}
	//Esperar a que terminen los threads
	for (i = 0; i < MAX_NUM_THREADS	; i++) {
		pthread_join(threads[i], NULL);
	}
	pthread_exit(NULL);
	return 0;
}