#!/bin/bash

# Ejecutar make de todos los módulos
make
cd ../cpu && make
cd ../fileSystem && make
cd ../kernel && make
cd ../consola && make
cd ../memoria
