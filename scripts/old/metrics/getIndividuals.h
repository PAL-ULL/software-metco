#ifndef GET_INDIVIDUALS_H
#define GET_INVIDIDUALS_H

#include "parseSeqDefinitions.h"
#include "parseParDefinitions.h"
#include "Individual.h"

void getIndividualParallelExperiment(string baseDirectory);
void getIndividualSequentialExperiment(string baseDirectory);
extern Individual *ind;

#endif
