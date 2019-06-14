#include "DeJong5.h"
#include "utils.h"
#include "Definitions.h"

#include <iostream>
#include <stdlib.h>
#include <math.h>

using namespace std;

int dataVector[2][25] = { { -32, -16,  0,   16,  32, -32, -16,  0,   16,  32, -32, -16, 0, 16, 32, -32, -16, 0,  16, 32, -32, -16, 0,  16, 32},
													{ -32, -32, -32, -32, -32, -16, -16, -16, -16, -16,  0,   0,  0, 0,  0,   16,  16, 16, 16, 16,  32,  32, 32, 32, 32} }; 

bool DeJong5::init(const vector<string> &parameters){
	if (parameters.size() != 0){
		cerr << "Error, no se esperaban parametros en el individuo" << endl;
		exit(-1);
	}
	setNumberOfVar(2);
	setNumberOfObj(1);
	return true;
}

void DeJong5::evaluate(){
	long double externalSum = 0;
	for (int i = 0; i < 25; i++){
		long double internalSum = 0;
		for (int j = 0; j < 2; j++){
			internalSum += pow((getVar(j) - dataVector[j][i]), 6);
		}
		externalSum += 1.0 / ((i+1) + internalSum);
	}
	setObj(0, (1.0 / ( ( 1.0 / 500.0) + externalSum ) ) );
}

Individual* DeJong5::clone() const{
	return new DeJong5();
}
