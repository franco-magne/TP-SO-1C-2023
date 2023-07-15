#!/bin/bash

# Función para compilar un módulo y crear el directorio "logs"
compilar_modulo() {
  local modulo="$1"

  echo "Compilando módulo $modulo..."
  cd "$modulo" && make && mkdir -p logs
  cd ..
  echo "Compilación del módulo $modulo completada."
}

# Compilar módulo CPU
compilar_modulo "cpu"

# Compilar módulo FileSystem
compilar_modulo "fileSystem"

# Compilar módulo Kernel
compilar_modulo "kernel"

# Compilar módulo Consola
compilar_modulo "consola"

# Compilar módulo Memoria
compilar_modulo "memoria"

echo "Proceso de compilación finalizado."