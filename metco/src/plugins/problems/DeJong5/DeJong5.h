/***********************************************************************
* Autor: Carlos Segura González
* Fecha: 09/01/13 
* Descripcion: implementación de la función número 5 de DeJong
************************************************************************/

#include <Individual.h>

class DeJong5 : public Individual {
	public: 
		bool init(const vector<string> &params);
		double getMaximum(const int i) const { return 65536; }
		double getMinimum(const int i) const { return -65536; }
		unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }
		void evaluate();
	private:
		Individual* clone(void) const;
};
