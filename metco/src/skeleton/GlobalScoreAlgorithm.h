/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 *
 * DESCRIPTION
 *   Plugin base class to represent score algorithms which are executed by the master island 
 *********************************************************************************************/


#ifndef GLOBAL_SCORE_ALGORITHM_H
#define GLOBAL_SCORE_ALGORITHM_H

#include "ScoreAlgorithm.h"
#include "MOFront.h"

class GlobalScoreAlgorithm : public ScoreAlgorithm {
	public:
		virtual ~GlobalScoreAlgorithm() {}
		GlobalScoreAlgorithm();
		virtual long double estimateScore(const int alg, const int conf, const double rndValue) = 0;
		virtual void updateScore(const int alg, const int conf, const MOFront *globalSolution, const int inserted) = 0;
		virtual long double score(const MOFront *) = 0;//Para el criterio de parada
};

#endif
