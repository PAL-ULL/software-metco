#ifndef __MPP_H__
#define __MPP_H__

#include <stdlib.h>
#include <unistd.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include "Individual.h"

#define CROSSOVER_UNIFORM 0
#define CROSSOVER_HUX 1

class Dish {
    friend class MPP;

   private:
    string name;
    int daysWHSelect;
    float price;
    float quantity;
    vector<int> foodGroups;
    vector<float> nutritionalFacts;
    vector<string> allergens;
    vector<string> incompatibilities;

   public:
    Dish();
    Dish(const string& name, const int days, const float price_food,
         const float quant, const vector<int>& foodG, const vector<float>& facts,
         const vector<string>& aller, const vector<string>& incm);
    Dish(const string& name, const int days, const float price_food,
         const float quant);
    virtual ~Dish();
    friend ostream& operator<<(ostream& os, const Dish& dish);
    // Getters and Setters
    string getName();
    void setName(const string& str);
    int getDaysWHSelect();
    void setDaysWHSelect(const int days);
    float getPrice();
    void setPrice(const float p);
    float getQuantity();
    void setQuantity(const float quat);
    int getFoodGroupAt(const int i);
    void setFoodGroupAt(const int i, const int group);
    float getNutritionalFactAt(const int i);
    void setNutritionalFactAt(const int i, const float fact);
    string getAllergenAt(const int i);
    void setAllergenAt(const int i, const string& allergen);
    string getIncompatibilityAt(const int i);
    void setIncompatibilityAt(const int i, const string& incomp);
    // Push_back methods
    void addAllergenBack(const string& allergen);
    void addFoodGroupBack(const int group);
    void addNutritionalFactBack(const float fact);
    void addIncompatibilityBack(const string& incomp);
    // Getters for vectors
    const vector<int>& getFoodGroups() const;
    const vector<float>& getNutritionalFacts() const;
    const vector<string>& getAllergens() const;
    const vector<string>& getIncompatibilites() const;
};

class MPP : public Individual {
   private:
    static int nDays;
    static vector<Dish> mainDishes;
    static vector<Dish> secondDishes;
    static vector<Dish> desserts;
    //Ingesta recomendada al dia, grados de penalizaciones, grupos alimenticios existentes, vector de compatibilidad de platos
    static vector<pair<double, double>> recommendations;
    static vector<double> penalties;
    static vector<pair<int, int>> foodGroups;
    static vector<vector<vector<double>>> dishesCompatibility;
    //Del plan alimenticio total: alergenos, incompatibilidades alimenticias, grupos alimenticios, informacion nutricional
    static vector<string> planAllergens;
    static vector<string> planIncompt;
    static vector<int> planFoodGroups;
    static vector<double> planNutritionalFacts;
    // Operadores geneticos
    void dependentCrossover(Individual* ind);
    void dependentMutation(double pm);
    void setRecommendations(void);
    void setPenalties(void);
    void setFoodGroups(void);
    //Lectura de ficheros de platos
    void setDishes(string& first, string& second, string& dessert);
    void setDishes(const char* c_filename, vector<Dish>& v_vecP);
    //Calculo del vector de compatibilidad de platos
    void computeCompatibilities(void);
    void setChoosenFoodG(vector<int> gal, vector<bool>& galE);
    double setPenalitiesToCompatibilities(const vector<int>& gal, vector<bool> galE);

   private:
    // Constants
    static const int N_OBJ;
    static const int PARAMS;
    static const int TYPES_OF_DISHES;
    static const int NUTRIENTS;
    static const int ALLERGENS;
    static const int INCOMPATIBILITIES;
    static const int FOOD_GROUPS;
    static const int PENALTIES;
    static const double P_OTHERS;
    static const double P_MEAT;
    static const double P_CEREALS;
    static const double P_FRUITS;
    static const double P_DAIRY_PRODUCTS;
    static const double P_VEGETABLES;
    static const double P_SEAFOOD;
    static const double P_PASTA;
    static const double P_FISH;
    static const double P_GREENNESS;
    static const double P_1D;
    static const double P_2D;
    static const double P_3D;
    static const double P_4D;
    static const double P_5D;
    static const double P_FIRST_DISHES;
    static const double P_SECOND_DISHES;
    static const double P_DESSERTS;
    static const int N_RECOMMENDATIONS;
    static const double MIN_RECOMMENDATIONS;
    static const double MAX_RECOMMENDATIONS;
    static const double R_ACFOL;
    static const double R_CAL;
    static const double R_EN;
    static const double R_FOS;
    static const double R_GRA;
    static const double R_HIE;
    static const double R_MAG;
    static const double R_POT;
    static const double R_PRO;
    static const double R_SEL;
    static const double R_SOD;
    static const double R_VA;
    static const double R_VB1;
    static const double R_VB2;
    static const double R_VB6;
    static const double R_VB12;
    static const double R_VC;
    static const double R_VD;
    static const double R_VE;
    static const double R_YOD;
    static const double R_ZIN;

   public:
    // Inicializacion
    bool init(const vector<string>& params);
    //Evaluacion de individuo
    void evaluate(void);
    //Clonacion de individuo
    Individual* clone(void) const;
    //Generacion aleatoria de individuo
    void restart(void);
    //Metodo de reparacion de individuo
    void repair(void);

    //Comprobar las restricciones del problema
    bool checkInfoN(const int i);
    bool checkInfoN2(void);
    //Metodos para el calculo del objetivo de grado de repeticion
    bool foodGroupsByIteration(vector<int> vec, int valor);
    int setValorPP(int id);
    int setValorSP(int id);
    int setValorP(int id);
    void sumValorPP(void);
    void sumValorSP(void);
    void sumValorP(void);
    void sumValorGA(void);
    void set_ultimos5GA(vector<vector<int>>& ultimos5GA, vector<int> vec);
    double set_ValorGAFirstAlternativa(vector<vector<int>>& ultimos5GA, vector<int> vec);
    unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }
    double inline getMinimum(const int i) const { return 1; }
    double inline getMaximum(const int i) const {
        if (i % 3 == 0)
            return mainDishes.size();
        else if (i % 3 == 1)
            return secondDishes.size();
        else if (i % 3 == 2)
            return desserts.size();
    }
    friend ostream& operator<<(ostream& os, const MPP& mpp);
};

#endif
