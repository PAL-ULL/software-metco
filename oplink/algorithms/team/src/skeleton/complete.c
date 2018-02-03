#include "TIPO_NUEVO.h"

extern "C" {
	TIPO_DEVUELTO *maker(){
		return new TIPO_NUEVO();
	}
}
