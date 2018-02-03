#include <iostream>
#include <stdlib.h>
#include <iomanip> 

using namespace std;

int main(int argc, char *argv[]){
	if (argc != 5){
		cerr << "Uso: " << argv[0] << " semilla cantidad_numeros " << " minValue maxValue" << endl;
		exit(-1);
	}

	cout << setprecision(20);
	unsigned int semilla = atoi(argv[1]);
	srandom(semilla);
	int cantidad = atoi(argv[2]);
	double minValue = atof(argv[3]);
	double maxValue = atof(argv[4]);
	for (int i = 0; i < cantidad; i++){
		double value = (((double)rand()) / RAND_MAX * (maxValue - minValue)) + minValue;
		cout << value << ", ";
	}
	cout << endl;
}
