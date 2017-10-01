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
#include <sys/time.h>
#include <pthread.h> 

#include <dirent.h>
#include <regex.h>
#include "queue.h"

#define MAX_NUM_THREADS 16
#define MAX_NUM_FILES 16
#define MAX_MATCHES 1 // Máximo núm de coincidencias en un string

// Variables globales
bool arg_l = false;
bool arg_r = false;
char * arg_regular_exp;
char * files[MAX_NUM_FILES];
int num_arg_files = 0; // Número de archivos/dirs en argumentos
int num_file_objs = 0; // Total de archivos
int num_matches = 0; // Número de coincidencias

// Variables de regex
int regex_status;		// Status de la compilación
regex_t regex_comp; 	// Expresión compilada

// Declaración de mutex
pthread_mutex_t mutex;

/* ************************************************ */

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
	arg_regular_exp = argv[arg_num];
	printf("\tExpresión regular: %s\n", arg_regular_exp);
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
	num_arg_files = i;
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
			if (arg_r) add_files(path, false);
			else add_files(path, true);
		}
		else {
			char path[1024];
			snprintf(path, sizeof(path), "%s/%s", object, entry->d_name);
			char * full_path = path;
			queue_push(full_path);
		}
	}
	closedir(dir);
	// Recuperar cuántos archivos son
	num_file_objs = queue_size();
}

/* Lo que ejecuta cada thread */
void * thread_exec(void * _thread_arg) {
	int * thread_arg = (int *) _thread_arg;
	int thread_num = * thread_arg;

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

		// Analizar si el archivo contiene la expresión regular
		// Abrir el archivo
		FILE * file_obj;
		file_obj = fopen(file, "r");
		if (file_obj == NULL) {
			printf("%s\n", file);
			print_error("No se puede abrir archivo.");
		}
		/* Leer todo el archivo de un solo */
		/*fseek(file_obj, 0, SEEK_END);
		long fsize = ftell(file_obj);
		fseek(file_obj, 0, SEEK_SET);
		//Guardar los contenidos
		char * buffer = malloc(fsize + 1);
		fread(buffer, fsize, 1, file_obj);
		buffer[fsize] = 0;*/
		/* Leer línea por línea */
		size_t read, len = 0;
		char * buffer;
		int i = 1;
		while((read = getline(&buffer, &len, file_obj)) != -1) {
			// Comparar
			regmatch_t matches[MAX_MATCHES];
			if (regexec(&regex_comp, buffer, MAX_MATCHES, matches, 0) == 0) {
				if (arg_l)
					printf("%s\n", file);
				else
					printf("[thread %i] %s: %d, %d\n", thread_num, file, i, matches[0].rm_so + 1);
				pthread_mutex_lock(&mutex);
				num_matches++;
				pthread_mutex_unlock(&mutex);
			}
			i++;
		}
		fclose(file_obj);
		if (buffer) free(buffer);
	}
	pthread_exit(NULL);
}

int main(int args, char* argv[]) {
	// Iniciar reloj
	struct timeval stop, start;
	gettimeofday(&start, NULL);

	// Obtener valores de argumentos
	get_args(args, argv);
	
	// Crear cola de archivos
	queue_init();
	int i;
	for (i = 0; i < num_arg_files; i++) {
		add_files(files[i], false);
	}
	
	// Inicializar el regex
	regex_status = regcomp(&regex_comp, arg_regular_exp, REG_EXTENDED);
	if (regex_status != 0) 
		print_error("La expresión regular no se pudo compilar.");
	
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
	
	// Liberar el regex
	regfree(&regex_comp);

	// Mensaje de coincidencias
	printf("\n%i coincidencias\n", num_matches);

	// Mensaje de rendimiento
	gettimeofday(&stop, NULL);

	printf("%i archivos analizados en %lu microsegundos\n", num_file_objs, stop.tv_usec - start.tv_usec);
	
	pthread_exit(NULL);
	return 0;
}