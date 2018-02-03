#ifndef __PROBLEMRND_H__
#define __PROBLEMRND_H__

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "RND_Mono.h"

using namespace std;

class RND_Mono;

// Clase para especificar las coordenadas de las estaciones base
class Position {
	public:
		Position();
		Position(unsigned int x, unsigned int y);
		inline unsigned int getX() { return x; }
		inline unsigned int getY() { return y; }
	private:
		unsigned int x;
		unsigned int y;
};


// De momento se supone que las antenas son siempre omnidireccionales y que la cobertura es circular
// Si se introdujeran nuevos tipos de antenas habria que incluir nuevas formas de calculo del cubrimiento
class ProblemRND {
	public:
		// Constructor para un problema con bs omni-direccionales (covertura circular)
		ProblemRND(const string &fich_dim, const string &fich_inst, const int radio);
		ProblemRND(const string &fich_inst);
		
		void readInstanceFile(const string &fichInstance);
		void readDimFile(const string &fich_dim);
		void readOldInstanceFile(const string &fich_inst);
		
		// Inicializacion de los ratios maximos de cubrimiento
		void calculateMaxCoverages();
		
		// Calculo del numero de puntos cubiertos por las antenas que estan a "1" en bs
		unsigned int calculateCoverage(RND_Mono *ind, unsigned int &usedBs, vector< vector<int> > *grid);

		// Inicializacion de la matriz del conjunto de puntos que puede cubrir cada antena 
		void calculateBsCoveredPoints();

		void calculateAntenas(RND_Mono *ind, vector< vector<int> > *posiciones);
		
		// Getters	
		inline unsigned int getBsLocationsSize()                 { return bsLocationsSize; }
		//NUEVO
		inline unsigned int getbsLocationsX(unsigned int RegionId) {return bsLocations[RegionId].getX();}
		inline unsigned int getbsLocationsY(unsigned int RegionId) {return bsLocations[RegionId].getY();}
		
		
		inline unsigned int getGridSizeX()                       { return gridSizeX; }
		inline unsigned int getGridSizeY()                       { return gridSizeY; }
		inline Position getCoveredPoint(unsigned int bsId, unsigned int point)
                                                          		{ return bsCoveredPoints[bsId][point]; }
		inline unsigned int getCoveredPoints(unsigned int bsId)  { return bsCoveredPoints[bsId].size(); }
	
	private:
		vector<Position> bsLocations;                       // Posibles ubicaciones para las BS
		unsigned int bsLocationsSize;                       // Numero maximo de posibles posiciones para las BS
		unsigned int bsCoverageRadius;                      // Radio de cubrimiento de la BS circular
		unsigned int gridSizeX, gridSizeY;                  // Dimensiones de la grid del problema
		double maxBsCoverageRate, maxCoverageRate;          // Ratios maximos de cubrimiento
		vector< vector <Position> > bsCoveredPoints;        // Conjunto de puntos cubierto por cada antena

};

#endif
