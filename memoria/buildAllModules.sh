#!/bin/bash

# Script para compilar y crear directorios en diferentes módulos

# Compilar módulo CPU
echo "Compilando módulo CPU..."
cd ../cpu && make && mkdir -p logs
echo "Compilación del módulo CPU completada."

# Compilar módulo FileSystem
echo "Compilando módulo FileSystem..."
cd ../fileSystem && make && mkdir -p logs
echo "Compilación del módulo FileSystem completada."

# Compilar módulo Kernel
echo "Compilando módulo Kernel..."
cd ../kernel && make && mkdir -p logs
echo "Compilación del módulo Kernel completada."

# Compilar módulo Consola
echo "Compilando módulo Consola..."
cd ../consola && make && mkdir -p logs
echo "Compilación del módulo Consola completada."

# Compilar módulo Memoria
echo "Compilando módulo Memoria..."
cd ../memoria && make && mkdir -p logs
echo "Compilación del módulo Memoria completada."

echo "Proceso de compilación finalizado."
