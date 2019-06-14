/***********************************************************************************
 * AUTORES
 *   Carlos Segura Gonz�lez
 *
 * DESCRIPCION
 *   Algoritmo de puntuacion que implementa el algoritmo basado en detecci�n
 *   de "pares sin�rgicos".
 *   Al m�dulo se le debe pasar el n�mero de islas esclavas que intervienen y supone
 *   que el criterio de parada est� definido de forma que todos acaban a la vez, es decir,
 *   que todas las islas van a cambiar el algoritmo de forma simult�nea.
 *   
 *   El funcionamiento es el siguiente:
 *   - Cada heur�stica tiene una puntuaci�n que es la media ponderada de los incrementos
 *   que ha conseguido (se usan los �ltimos n valores)
 *   - Cada par de heur�sticas tiene una puntuaci�n que es la media ponderada de las
 *   mejoras que se han conseguido al utilizar las 2 heur�sticas de forma simult�nea
 *   - La Selecci�n se hace de la siguiente forma:
 *      * En un x% (par�metro) de las veces se utiliza la puntuaci�n independiente
 *      * En un y% (par�metro) de las veces se utiliza la puntuaci�n de pares
 *      * En el resto de las veces se utiliza una selecci�n aleatoria
 ***********************************************************************************/


#include <vector>
#include <map>
#include <list>
#include "LocalScoreAlgorithm.h"
#include "MOFront.h"

using namespace std;

class MonoSynergyPairs : public LocalScoreAlgorithm {
	public:
		MonoSynergyPairs() { optDirection = -1; }
		long double score(MOFront *);
		long double estimateScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue);
		void updateScore(const long double breakFrom, const long double arriveTo, int alg, int conf, int islandId); 
		bool init(const vector<string> &params) ;
		void printHistory();
		void begins(const int algId, const int confId, const int islandId); 
		void ends(const int algId, const int confId, const int islandId);
		int getDirection() const { 
			if (optDirection == -1) { 
				cout << "Error Interno. Direccion no fijada en modulo Score" << endl;
			}
			return optDirection; 
		}
	private:
		long double getScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue);
		void asynchronyDetection(int islandId);
		map< int, map <int, int > > executions;
		map<int, map <int, int > > configToExecuting;
		map< int, map < int, vector < long double > > > scores;
		//map< int, map < int, map < int, map < int, vector < pair < double, int > > > > > > pairScores;
		map< int, map < int, map < int, map < int, vector < long double > > > > > pairScores;
		list < pair < int, int > > assignedHeuristics;
		int slaveIslands;
		double p1Value, p2Value;
		int consideredValues, optDirection;
};


