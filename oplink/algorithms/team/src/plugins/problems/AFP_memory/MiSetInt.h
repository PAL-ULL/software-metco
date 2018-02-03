#ifndef MISET_INT_H
#define MISET_INT_H

#include <vector>
//Manejo de sets de enteros, con insercion, recorrido y borrado (O(1))
//Solo puede ser usado para sets de enteros, en los que se conozca
//el rango de valores que puede contener
//Usa mucha mas memoria que los sets de la stl
//La creacion es más lenta que los sets de la stl
//No se pueden hacer uniones en O(n) porque no se mantienen los valores ordenados
//Limitado a 2^(sizeof short) elementos

using namespace std;

class MiSetInt{
	public:
		MiSetInt(unsigned short maxValue){
			pos.insert(pos.begin(), maxValue+1, -1);
		}
		
		inline void insert(unsigned short value){
			if (pos[value] != -1)
				return;
			pos[value] = values.size();
			values.push_back(value);
		}
		
		inline void remove(unsigned short value){
			if (pos[value] == -1)
				return;
			values[pos[value]] = values[values.size()-1];
			pos[values[values.size()-1]] = pos[value];
			pos[value] = -1;
			values.pop_back();
		}

		inline unsigned short size() const { return values.size(); }
		inline unsigned short get(unsigned short index) const { return values[index]; }
	private:
		vector<unsigned short> values;
		vector<short> pos;
};

#endif
