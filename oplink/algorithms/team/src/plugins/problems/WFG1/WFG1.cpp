/***********************************************************************************
 * AUTORES
 *   - Eduardo Manuel Segredo Gonzalez
 * 
 * FECHA
 *   Julio de 2008
 *
 * *********************************************************************************/

#include <vector>
#include <math.h>

#include "WFG1.h"

// Constantes y variables estaticas
const double PI = 3.1415926535897932384626433832795;
int WFG1::k;

// Inicializacion del problema
bool WFG1::init (const vector<string> &params) {
	// Comprueba los parametros del problema
	if (params.size() != 1) {
		cout << "Error WFG1: Incorrect parameters (number_of_objectives)" << endl;
		return false;
	}

	// Inicializa los parametros del problema
	int nObj = atoi(params[0].c_str());

	if (nObj < 2) {
	  cout << "Error WFG1: Parameter number_of_objectives must be greater than 2" << endl;
		return false;
	}

	if (nObj >= 3)
    k = 2 * (nObj - 1);
	else
	  k = 4;

  setNumberOfVar(k + l);  // Parametros de posicion + Parametros de distancia
	setNumberOfObj(nObj);   // Numero de objetivos
	return true;
}

// Evaluacion de un individuo WFG1
void WFG1::evaluate (void) {

	// Inicializa el vector y
  vector<double> y;
	for (int i = 0; i < getNumberOfVar(); i++)
	  y.push_back(getVar(i));

	// Aplica las transiciones al vector y
	y = WFG1_t1(y, k);
	y = WFG1_t2(y, k);
	y = WFG1_t3(y);
	y = WFG1_t4(y, k, getNumberOfObj());

	// Calcula los valores de las funciones objetivo
	vector<double> result = WFG1_shape(y);

	// Fija dichos valores
	for (int i = 0; i < result.size(); i++)
	  setObj(i, result[i]);

}
				
// Clonacion de un individuo WFG1
Individual* WFG1::clone (void) const {
	return new WFG1();
}

/******************************************************************************
 * Funciones auxiliares al problema
 * ****************************************************************************/

// Escala un valor al intervalo [0, 1]
double WFG1::correct_to_01 (const double &a, const double &epsilon) {

  const double min = 0.0;
	const double max = 1.0;

	const double min_epsilon = min - epsilon;
	const double max_epsilon = max + epsilon;

  if ((a <= min) && (a >= min_epsilon)) {
	  return min;
 	}
  else if ((a >= max) && (a <= max_epsilon)) {
    return max;
  }
  else {
    return a;
	}
}

// Construye un vector con los elementos v[head], ..., v[tail - 1]
vector<double> WFG1::subvector (const vector<double> &v, const int head, const int tail) {

  vector< double > result;

	for(int i = head; i < tail; i++) {
    result.push_back(v[i]);
 	}

  return result;
}

vector<double> WFG1::calculate_x (const vector<double> &t_p, const vector<short> &A) {

  vector<double> result;

	for (int i = 0; i < t_p.size() - 1; i++) {
		const double tmp1 = max<double>(t_p.back(), A[i]);
	 	result.push_back(tmp1 * (t_p[i] - 0.5) + 0.5); 
	}
 
 	result.push_back(t_p.back());
	return result;
}

vector<double>  WFG1::calculate_f (const double &D, const vector<double> &x, const vector<double> &h,
                                   const vector<double> &S) {

  vector< double > result;

  for (int i = 0; i < h.size(); i++) {
    result.push_back(D * x.back() + S[i] * h[i]);
  }

  return result;
}

/******************************************************************************
 * Transiciones genericas
 * ****************************************************************************/

double WFG1::s_linear (const double &y, const double &A) {
  return correct_to_01(fabs(y - A) / fabs(floor(A - y) + A));
}

double WFG1::b_poly (const double &y, const double &alpha) {
  return correct_to_01(pow(y, alpha));
}

double WFG1::b_flat (const double &y, const double &A, const double &B, const double &C) {

  const double tmp1 = min(0.0, floor(y - B)) * A * (B - y) / B;
  const double tmp2 = min(0.0, floor(C - y)) * (1.0 - A) * (y - C) / (1.0 - C);
	return correct_to_01(A + tmp1 -tmp2);
}

double WFG1::r_sum (const vector<double> &y, const vector<double> &w) {

  double numerator   = 0.0;
  double denominator = 0.0;

  for(int i = 0; i < y.size(); i++) {
	  numerator   += w[i] * y[i];
	  denominator += w[i];
	}

	return correct_to_01(numerator / denominator);
}

/******************************************************************************
 * Superficies genericas
 * ****************************************************************************/

double WFG1::convex (const vector<double> &x, const int m) {

	const int M = x.size();
  double result = 1.0;

  for (int i = 1; i <= M - m; i++) {
	  result *= 1.0 - cos(x[i - 1] * PI / 2.0);
 	}

  if( m != 1 ) {
    result *= 1.0 - sin(x[M - m] * PI/ 2.0);
 	}

  return correct_to_01(result);
}

double WFG1::mixed (const vector<double> &x, const int A, const double &alpha) {
  const double tmp = 2.0 * A * PI;
  return correct_to_01(pow(1.0 - x[0] - cos(tmp * x[0] + PI / 2.0) / tmp, alpha));
}

/******************************************************************************
 * Funciones propias del problema WFG1
 * ****************************************************************************/

// Primera transicion para WFG1
vector<double> WFG1::WFG1_t1 (const vector<double> &y, const int k) {

  const int n = y.size();
  vector<double> t;

  for(int i = 0; i < k; i++) {
    t.push_back(y[i]);
 	}

  for(int i = k; i < n; i++) {
    t.push_back(s_linear(y[i], 0.35));
	}

  return t;

}

// Segunda transicion para WFG1
vector<double> WFG1::WFG1_t2 (const vector<double> &y, const int k) {

  const int n = y.size();
  vector<double> t;

  for(int i = 0; i < k; i++) {
    t.push_back(y[i]);
  }

  for(int i = k; i < n; i++) {
    t.push_back(b_flat(y[i], 0.8, 0.75, 0.85));
 	}

  return t;

}

// Tercera transicion para WFG1
vector<double> WFG1::WFG1_t3 (const vector<double> &y) {

  const int n = y.size();
  vector<double> t;

  for(int i = 0; i < n; i++) {
    t.push_back(b_poly(y[i], 0.02));
	}

  return t;

}

// Cuarta transicion para WFG1
vector<double> WFG1::WFG1_t4 (const vector<double> &y, const int k, const int M) {

  const int n = y.size();
  vector<double> w;

  for(int i = 1; i <= n; i++) {
    w.push_back(2.0 * i);
	}

	vector<double> t;

	for (int i = 1; i <= M - 1; i++) {
    const int head = (i - 1) * k / (M - 1);
		const int tail = i * k / (M - 1);
		const vector<double> &y_sub = subvector(y, head, tail);
		const vector<double> &w_sub = subvector(w, head, tail); 
    t.push_back(r_sum(y_sub, w_sub));
 	}
 
 	const vector<double> &y_sub = subvector(y, k, n);
	const vector<double> &w_sub = subvector(w, k, n);
	t.push_back(r_sum(y_sub, w_sub));

  return t;

}

// Superficie para WFG1
vector<double> WFG1::WFG1_shape (const vector<double> &t_p) {

  const int M = t_p.size();
  const vector<short> &A = WFG_create_A(M, false);
  const vector<double> &x = calculate_x(t_p, A);
  vector<double> h;

  for(int m = 1; m <= M - 1; m++) {
    h.push_back(convex(x, m));
 	}

	h.push_back(mixed(x, 5, 1.0)); 
												  
	return WFG_calculate_f(x, h);

}

/******************************************************************************
 * Funciones propias de los problemas WFG
 * ****************************************************************************/

// Construye un vector de longitud M - 1 con los valores "1,0,0,..." si
// "degenerate" es verdadero. En otro caso, lo construye con los valores
// "1,1,1,..."
vector<short> WFG1::WFG_create_A (const int M, const bool degenerate) {
    
  if (degenerate) {
    vector<short> A(M - 1, 0);
    A[0] = 1;
    return A;
  }
  else {
    return vector<short>(M - 1, 1);
  }
}

// Calcula los valores de fitness escalados para un problema WFG 
vector<double> WFG1::WFG_calculate_f (const vector< double >& x, const vector< double >& h) {
  
  const int M = h.size();
	vector<double> S; 
	
	for(int m = 1; m <= M; m++) {
    S.push_back(m * 2.0);
 	}

	return calculate_f(1.0, x, h, S);
}
