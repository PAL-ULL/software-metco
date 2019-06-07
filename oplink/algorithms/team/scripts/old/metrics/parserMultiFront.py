#!/usr/bin/python
# -*- coding: iso-8859-1 -*-
#from __future__ import division
import os
import sys

# Fichero de entrada
# Contendr� el numero de individuos en el frente y una linea por individuo
# En cada linea tendr�: p1 p2 p3 p4 p5 o1 o2 o3
# El fichero de salida tendr� tan solo los objetivos
inputFile = open(sys.argv[1], "r")

# Fichero de salida
outputFile = open(sys.argv[2], "w")

# Tama�o de la cabecera del fichero de entrada
headSize = int (sys.argv[3]);

# N�mero de objetos
NParam = int (sys.argv[4]);
Nobj = int (sys.argv[5]);

# Nos saltamos la informacion de la cabecera
for i in range(headSize):
	inputFile.readline()

# N�mero de l�neas a leer = tama�o del frente de entrada
inputFileLineList = inputFile.readline().split()
nInd = int(inputFileLineList[3])

# Leer el frente de entrada
for i in range(nInd):
	inputFileLineList = inputFile.readline().split()
	for j in range(Nobj):
		outputFile.write(inputFileLineList[j + NParam] + "   ");
	outputFile.write("\n");

outputFile.close()
inputFile.close()
