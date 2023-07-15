#include "commons-nuestras.h"

#define MAX_COMMAND_LENGTH 1000
#define MAX_IP_LENGTH 16


FILE* abrir_archivo(const char* pathArchivo, const char* mode, t_log* moduloLogger)
{
    FILE* tempFilePointer = fopen(pathArchivo, mode);

    if(tempFilePointer == NULL) {

        log_error(moduloLogger, "No se pudo abrir el archivo en el path %s especificado.", pathArchivo);
    }

    return tempFilePointer;
}

int config_init(void* moduleConfig, char* pathToConfig, t_log* moduleLogger,void (*config_initializer)(void* moduleConfig, t_config* tempConfig)) {

    t_config* tempConfig = config_create(pathToConfig);
    if (NULL == tempConfig) {
        log_error(moduleLogger, "Path del archivo de configuracion \"%s\" no encontrado", pathToConfig);
        return -1;
    }

    config_initializer(moduleConfig, tempConfig);
    log_info(moduleLogger, "Inicialización de campos de configuracion correcta");
    config_destroy(tempConfig);

    return 1;
}

void intervalo_de_pausa(uint32_t duracionEnMilisegundos) 
{
    const uint32_t SECS_MILISECS = 1000;
    const uint32_t MILISECS_NANOSECS = 1000000;
    struct timespec timeSpec;
    timeSpec.tv_sec = duracionEnMilisegundos / SECS_MILISECS;
    timeSpec.tv_nsec = (duracionEnMilisegundos % SECS_MILISECS) * MILISECS_NANOSECS;
    nanosleep(&timeSpec, &timeSpec);
}

int size_recurso_list(char** vectorRecursos)
{
    int tamanioLista = 0;

    for(int i = 0; vectorRecursos[i] != NULL; i++) {

        tamanioLista++;
    }

    return tamanioLista ;
}

bool contains(char** list, char* object) {

    if (list == NULL || object == NULL) {
        return false; // Manejar el caso de punteros nulos
    }
    
    for (int i = 0; i < size_recurso_list(list); i++) {
        if (list[i] != NULL && strcmp(list[i], object) == 0) {
            return true;
        }
    }
    
    return false;
}

int position_in_list(char** list, char* object) {

    for (int i = 0; i < size_recurso_list(list); i++) {
        if (strcmp(list[i], object) == 0) {
            return i;
        }
    }

    return -1;
}
int list_get_index(t_list* list, bool (*cutting_condition)(void*, void*), void* target) {
    
    for (int i = 0; i < list_size(list); i++) {
        void* temp = list_get(list, i);
        if (cutting_condition(temp, target)) {
            return i;
        }
    }

    return -1;
}

void set_timespec(struct timespec* timespec) 
{
    int retVal = clock_gettime(CLOCK_REALTIME, timespec);
    
    if (retVal == -1) {
        perror("clock_gettime");
        exit(-1);
    }
}

double obtener_diferencial_de_tiempo_en_milisegundos(struct timespec end, struct timespec start) 
{
    const uint32_t SECS_TO_MILISECS = 1000;
    const uint32_t NANOSECS_TO_MILISECS = 1000000;
    return (end.tv_sec - start.tv_sec) * SECS_TO_MILISECS + (end.tv_nsec - start.tv_nsec) / NANOSECS_TO_MILISECS;
}

bool es_el_ultimo_elemento(t_list* lista, t_link_element* elemento) {

    if (lista->head == NULL || elemento == NULL) {
        return false;
    }
    
    t_link_element* actual = lista->head;
    while (actual->next != NULL) {
        actual = actual->next;
    }
    
    return (actual == elemento);
}

t_segmento* segmento_create(uint32_t id_de_segmento, uint32_t tamanio_de_segmento) {

    t_segmento* this = malloc(sizeof(*this));
    this->pid = -1;
    this->id_de_segmento = id_de_segmento;
    this->base_del_segmento = -1;
    this->tamanio_de_segmento = tamanio_de_segmento;
    this->victima = true;
    
    return this;
}

t_segmento* buffer_unpack_segmento(t_buffer* buffer) {

    t_segmento* segmento = segmento_create(-1,-1);

    buffer_unpack(buffer, &(segmento->pid), sizeof(segmento->pid));
    buffer_unpack(buffer, &(segmento->id_de_segmento), sizeof(segmento->id_de_segmento));
    buffer_unpack(buffer, &(segmento->base_del_segmento), sizeof(segmento->base_del_segmento));
    buffer_unpack(buffer, &(segmento->tamanio_de_segmento), sizeof(segmento->tamanio_de_segmento));
    buffer_unpack(buffer, &(segmento->victima), sizeof(bool));

    return segmento;
}

t_list* buffer_unpack_segmento_list(t_buffer* buffer) {

    t_list* lista_segmentos = list_create();

    int cantidad_segmentos;
    buffer_unpack(buffer, &cantidad_segmentos, sizeof(cantidad_segmentos));

    for (int i = 0; i < cantidad_segmentos; i++) {
        t_segmento* segmento = buffer_unpack_segmento(buffer);
        list_add(lista_segmentos, segmento);
    }

    return lista_segmentos;
}

void buffer_pack_segmento(t_buffer* buffer, t_segmento* segmento) {

    buffer_pack(buffer, &(segmento->pid), sizeof(segmento->pid));
    buffer_pack(buffer, &(segmento->id_de_segmento), sizeof(segmento->id_de_segmento));
    buffer_pack(buffer, &(segmento->base_del_segmento), sizeof(segmento->base_del_segmento));
    buffer_pack(buffer, &(segmento->tamanio_de_segmento), sizeof(segmento->tamanio_de_segmento));
    buffer_pack(buffer, &(segmento->victima), sizeof(segmento->victima));
}

void buffer_pack_segmento_list(t_buffer* buffer, t_list* lista_segmentos) {

    int cantidad_segmentos = list_size(lista_segmentos);
    buffer_pack(buffer, &cantidad_segmentos, sizeof(cantidad_segmentos));

    for (int i = 0; i<list_size(lista_segmentos); i++) {
       t_segmento* segmento = list_get(lista_segmentos,i);
       buffer_pack_segmento(buffer,segmento);
    }
}

t_list* list_filter_ok(t_list* lista, bool (*condition)(void*, void*), void* argumento) {
    t_list* resultado = list_create();
 
    for (int i = 0; i < lista->elements_count; i++) {
        void* elemento = list_get(lista, i);
        if (condition(elemento, argumento)) {
            list_add(resultado, elemento);
        }
    }
 
    return resultado;
}


///////////////////////////////////////////// FUNCIONES DECORATIVAS ///////////////////////////////

void inicio_kernel() {

    printf("\033[1;31m   K   K   \033[1;32m  EEEEE   \033[1;33m  RRRRR   \033[1;34m  N     N   \033[1;35m  EEEEE   \033[1;36m  L       \033[0m\n");
    printf("\033[1;31m   K  K    \033[1;32m  E       \033[1;33m  R    R  \033[1;34m  NN    N   \033[1;35m  E       \033[1;36m  L       \033[0m\n");
    printf("\033[1;31m   KKK     \033[1;32m  EEEE    \033[1;33m  RRRRR   \033[1;34m  N N   N   \033[1;35m  EEEE    \033[1;36m  L       \033[0m\n");
    printf("\033[1;31m   K  K    \033[1;32m  E       \033[1;33m  R  R    \033[1;34m  N  N  N   \033[1;35m  E       \033[1;36m  L       \033[0m\n");
    printf("\033[1;31m   K   K   \033[1;32m  EEEEE   \033[1;33m  R   R   \033[1;34m  N   N N   \033[1;35m  EEEEE   \033[1;36m  LLLLL   \033[0m\n");
}


void inicio_cpu() {
    printf("\033[1;31m   CCCC   \033[1;32m  PPPPP   \033[1;33m  U   U   \033[0m\n");
    printf("\033[1;31m  C       \033[1;32m  P   PP  \033[1;33m  U   U   \033[0m\n");
    printf("\033[1;31m  C       \033[1;32m  PPPPP   \033[1;33m  U   U   \033[0m\n");
    printf("\033[1;31m  C       \033[1;32m  P       \033[1;33m  U   U   \033[0m\n");
    printf("\033[1;31m   CCCC   \033[1;32m  P       \033[1;33m  UUUUU   \033[0m\n");
}


void imprimir_consola() {
    printf("\033[1;31m  CCCCCC   \033[1;32m  OOOOO   \033[1;33m  N     N   \033[1;34m  SSSSS   \033[1;35m  OOOOO   \033[1;36m  L       \033[1;37m  AAAAA   \033[0m\n");
    printf("\033[1;31m C         \033[1;32m O     O  \033[1;33m  NN    N   \033[1;34m S        \033[1;35m O     O  \033[1;36m  L       \033[1;37m A     A  \033[0m\n");
    printf("\033[1;31m C         \033[1;32m O     O  \033[1;33m  N N   N   \033[1;34m  SSSSS   \033[1;35m O     O  \033[1;36m  L       \033[1;37m AAAAAAA  \033[0m\n");
    printf("\033[1;31m C         \033[1;32m O     O  \033[1;33m  N  N  N        \033[1;34m  S  \033[1;35m O     O  \033[1;36m  L       \033[1;37m A     A  \033[0m\n");
    printf("\033[1;31m  CCCCCC   \033[1;32m  OOOOO   \033[1;33m  N   N N   \033[1;34m  SSSSS   \033[1;35m  OOOOO   \033[1;36m  LLLLL   \033[1;37m A     A  \033[0m\n");
}


void imprimir_memoria() {
    printf("\033[1;31m   M   M   \033[1;32m  EEEEE   \033[1;33m  M   M   \033[1;34m  OOOOO   \033[1;35m  RRRRR   \033[1;36m  I  \033[37m  AAAAAAA   \033[0m\n");
    printf("\033[1;31m   MM MM   \033[1;32m  E       \033[1;33m  MM MM   \033[1;34m O     O  \033[1;35m  R    R  \033[1;36m  I  \033[37m  A     A  \033[0m\n");
    printf("\033[1;31m   M M M   \033[1;32m  EEEE    \033[1;33m  M M M   \033[1;34m O     O  \033[1;35m  RRRRR   \033[1;36m  I  \033[37m  AAAAAAA  \033[0m\n");
    printf("\033[1;31m   M   M   \033[1;32m  E       \033[1;33m  M   M   \033[1;34m O     O  \033[1;35m  R  R    \033[1;36m  I  \033[37m  A     A  \033[0m\n");
    printf("\033[1;31m   M   M   \033[1;32m  EEEEE   \033[1;33m  M   M   \033[1;34m  OOOOO   \033[1;35m  R   R   \033[1;36m  I  \033[37m  A     A  \033[0m\n");
}


void imprimir_file_system() {
    printf("         \033[1;31m   FFFFF   \033[1;32m  IIIII   \033[1;33m  L       \033[1;34m  EEEEE   \033[0m\n");
    printf("         \033[1;31m   F       \033[1;32m    I     \033[1;33m  L       \033[1;34m  E       \033[0m\n");
    printf("         \033[1;31m   FFFF    \033[1;32m    I     \033[1;33m  L       \033[1;34m  EEEE    \033[0m\n");
    printf("         \033[1;31m   F       \033[1;32m    I     \033[1;33m  L       \033[1;34m  E       \033[0m\n");
    printf("         \033[1;31m   F       \033[1;32m  IIIII   \033[1;33m  LLLLL   \033[1;34m  EEEEE   \033[0m\n");

    printf("\n");

    printf("\033[1;31m   SSSSS   \033[1;32m  Y   Y   \033[1;33m   SSSSS    \033[1;34m TTTTTT   \033[1;35m  EEEEE   \033[1;36m  M   M   \033[0m\n");
    printf(" \033[1;31m S       \033[1;32m    Y Y    \033[1;33m  S        \033[1;34m    TT     \033[1;35m  E       \033[1;36m  M M M   \033[0m\n");
    printf("\033[1;31m   SSSSS   \033[1;32m    Y     \033[1;33m   SSSSS   \033[1;34m    TT     \033[1;35m  EEEE    \033[1;36m  M   M   \033[0m\n");
    printf("\033[1;31m        S  \033[1;32m    Y     \033[1;33m        S  \033[1;34m    TT     \033[1;35m  E       \033[1;36m  M   M   \033[0m\n");
    printf("\033[1;31m  SSSSSS   \033[1;32m    Y     \033[1;33m  SSSSSS   \033[1;34m    TT     \033[1;35m  EEEEE   \033[1;36m  M   M   \033[0m\n");
}


char* getIPAddress() {

    struct ifaddrs* ifaddr;
    struct ifaddrs* ifa;
    char* ip = NULL;

    // Obtener la lista de interfaces de red
    if (getifaddrs(&ifaddr) == -1) {
        perror("Error al obtener la lista de interfaces de red");
        exit(EXIT_FAILURE);
    }

    // Recorrer la lista de interfaces
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        // Verificar si es una interfaz de red IPv4 y no loopback
        if (ifa->ifa_addr->sa_family == AF_INET &&
            !(ifa->ifa_flags & 0x8)) {
            // Obtener la dirección IP como una cadena
            struct sockaddr_in* addr = (struct sockaddr_in*)ifa->ifa_addr;
            ip = malloc(INET_ADDRSTRLEN);
            inet_ntop(AF_INET, &(addr->sin_addr), ip, INET_ADDRSTRLEN);

            // Buscar la cadena "inet " en la descripción
            char* description = ifa->ifa_data;
            if (description != NULL) {
                char* inetPtr = strstr(description, "inet ");
                if (inetPtr != NULL) {
                    // Obtener el valor de la dirección IP después de "inet "
                    char* inetValue = inetPtr + strlen("inet ");
                    strncpy(ip, inetValue, INET_ADDRSTRLEN);
                }
            }

            break;
        }
    }

    // Liberar la memoria de la lista de interfaces
    freeifaddrs(ifaddr);

    return ip;
}