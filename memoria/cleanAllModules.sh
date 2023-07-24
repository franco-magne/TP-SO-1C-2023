#!/bin/bash

# Ejecutar make clean en todos los módulos
make clean
cd ../cpu && make clean
cd ../fileSystem && make clean
cd ../kernel && make clean
cd ../consola && make clean
cd ../memoria
