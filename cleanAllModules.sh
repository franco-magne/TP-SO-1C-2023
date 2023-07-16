#!/bin/bash

# Realizar un make clean de todos modulos
clean_modulo() {
  local modulo="$1"
  cd "$modulo" && make clean
  cd ..
}

# Clean módulo CPU
clean_modulo "cpu"

# Clean módulo FileSystem
clean_modulo "fileSystem"

# Clean módulo Kernel
clean_modulo "kernel"

# Clean módulo Consola
clean_modulo "consola"

# Clean módulo Memoria
clean_modulo "memoria"

echo "Proceso de clean finalizado."