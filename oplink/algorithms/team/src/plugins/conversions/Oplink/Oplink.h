/*****************************************************************************************
 * AUTHORS
 *  - Gara Miranda Valladares
 *  - Carlos Segura González
 *
 * DATE
 *   February 2008
 *
 * DESCRIPTION
 * 	 Translation valid for the OPLINK project. It is valid for the algorithms: EE, NSGA2,
 * 	 SPEA, SPEA2, MOPSO, IBEA, IBEA_Adapt, MOCell and DE.
 * 	 EE is the only one algorithm with different representation, because it maintains a
 * 	 variance for each individual. When an individual is migrated to DE, an initial value
 * 	 is fixed to the variance. Such value is a parameter of the plugin.
 ****************************************************************************************/


#include "Conversion.h"

class Oplink: public Conversion {
	public:
		bool init(const vector<string> &v);
		Individual* convert(const Individual *ind, const int confFrom, const int confTo);
	private:
		double EEvarianza;
};

