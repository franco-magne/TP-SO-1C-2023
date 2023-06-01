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