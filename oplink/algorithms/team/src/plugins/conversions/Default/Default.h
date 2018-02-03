/*****************************************************************************************
 * AUTHORS
 *  - Gara Miranda Valladares
 *  - Carlos Segura González
 *
 * DATE
 *   February 2008
 *
 * DESCRIPTION
 *   Default Conversion method. It just clone individuals. It is valid when every 
 *   individual has the same representation.
 * ***************************************************************************************/


#include "Conversion.h"

class Default: public Conversion {
	public:
		bool init(const vector<string> &v);
		Individual* convert(const Individual *ind, const int confFrom, const int confTo);
};

