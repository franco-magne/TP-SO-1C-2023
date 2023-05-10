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