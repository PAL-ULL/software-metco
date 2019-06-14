/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 *
 * DESCRIPTION
 *   Plugin base class to represent score algorithms which are executed by the slave islands 
 *********************************************************************************************/


#ifndef LOCAL_SCORE_ALGORITHM_H
#define LOCAL_SCORE_ALGORITHM_H

#include "ScoreAlgorithm.h"
#include "MOFront.h"

class LocalScoreAlgorithm : public ScoreAlgorithm {
	public:
		LocalScoreAlgorithm();
		virtual ~LocalScoreAlgorithm() {}
		virtual long double score(MOFront *) = 0;//Executed in slaves
		virtual void updateScore(const long double breakFrom, const long double arriveTo, int alg, int conf, int islandId) = 0; //Executed in Coordinator 
		virtual long double estimateScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue) = 0; // Executed in Coordinator
};

#endif
