# pthread_grep
Buscador de expresiones regulares en archivos y directorios en C, implementado con pthreads.

Proyecto para Principios de Sistemas Operativos

Tecnológico de Costa Rica


**Para compilar:**
 * gcc -pthread -o grep grep.c 


**Uso del programa:**
 * ./grep [opciones] [expresión regular] [archivos|directorio]


**Opciones:**
 * -l: Solo muestra el nombre del archivo
 * -r: Busca recursivamente en el directorio


**Expresión regular:**
 * Ver http://pubs.opengroup.org/onlinepubs/7908799/xbd/re.html para más información


**Archivos / Directorio:**
 * Uno o más archivos, separado por espacios
 * Un directorio
