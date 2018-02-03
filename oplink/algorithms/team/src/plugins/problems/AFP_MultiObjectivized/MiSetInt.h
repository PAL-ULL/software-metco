#ifndef MISET_INT_H
#define MISET_INT_H

#include <vector>
//Manejo de sets de enteros, con insercion, recorrido y borrado (O(1))
//Solo puede ser usado para sets de enteros, en los que se conozca
//el rango de valores que puede contener
//Usa mucha mas memoria que los sets de la stl
//La creacion es más lenta que los sets de la stl
//No se pueden hacer uniones en O(n) porque no se mantienen los valores ordenados

using namespace std;

class MiSetInt{
	public:
		MiSetInt(int maxValue){
			pos.insert(pos.begin(), maxValue+1, -1);
		}
		
		inline void insert(int value){
			if (pos[value] != -1)
				return;
			pos[value] = values.size();
			values.push_back(value);
		}
		
		inline void remove(int value){
			if (pos[value] == -1)
				return;
			values[pos[value]] = values[values.size()-1];
			pos[values[values.size()-1]] = pos[value];
			pos[value] = -1;
			values.pop_back();
		}

		inline int size() const { return values.size(); }
		inline int get(int index) const { return values[index]; }
	private:
		vector<int> values;
		vector<int> pos;
};

#endif
