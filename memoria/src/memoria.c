#include <../include/memoria.h>

t_log *memoriaLogger;
t_config *memoriaConfig;

int main() {

   memoriaLogger = log_create(MEMORIA_LOG_UBICACION,MEMORIA_PROCESS_NAME,true,LOG_LEVEL_INFO);
   memoriaConfig = config_create(MEMORIA_CONFIG_UBICACION);
   char *memoriaIP = config_get_string_value(memoriaConfig, "IP");
   char *memoriaPort = config_get_string_value(memoriaConfig,"PUERTO_MEMORIA");
   


   int serverMemoria = iniciar_servidor(memoriaIP, memoriaPort);
   log_info(memoriaLogger,"Servidor memoria listo para recibir al modulo\n");
   aceptar_conexiones_memoria(serverMemoria);
   

   

}  

void aceptar_conexiones_memoria(const int socketEscucha) 
{ 
    struct sockaddr cliente = {0};
    socklen_t len = sizeof(cliente);
    
    log_info(memoriaLogger, "A la escucha de nuevas conexiones en puerto %d", socketEscucha);
    
    for (int i =0 ;i<3 ;i++) { // PONEMOS 2 PORQUE PARA TESTEAR QUE SE CONECTE PRIMERO CPU Y DESPUES KERNEL
        
        const int clienteAceptado = accept(socketEscucha, &cliente, &len);
        
        if (clienteAceptado > -1) {
            
            log_info(memoriaLogger, "Se conecto un modulo");        
         } 
        else {

            log_error(memoriaLogger, "Error al aceptar conexión: %s", strerror(errno));
        }
    }
}
