#!/bin/bash

# Script para clonar repositorio so-commons-library y entorno vms

# Clonar repositorio so-commons de github
echo "Clonando so-commons-library..."
cd .. && git clone https://github.com/sisoputnfrba/so-commons-library.git
echo "Repositorio so-commons-library clonado."

# Clonar repositorio entorno-vms de github
echo "Clonando entorno-vms..."
git clone https://github.com/sisoputnfrba/entorno-vms.git
echo "Repositorio entorno-vms clonado."