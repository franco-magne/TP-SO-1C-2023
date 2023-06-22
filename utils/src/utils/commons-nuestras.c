#include "commons-nuestras.h"

FILE* abrir_archivo(const char* pathArchivo, const char* mode, t_log* moduloLogger)
{
    FILE* tempFilePointer = fopen(pathArchivo, mode);

    if(tempFilePointer == NULL){

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

    for(int i = 0; vectorRecursos[i] != NULL; i++){

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
        if (actual == elemento) {
            return false;
        }
        actual = actual->next;
    }
    
    return (actual == elemento);
}