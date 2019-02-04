#include "MPP.h"

// Static variables
int MPP::nDays;
vector<pair<double, double>> MPP::recommendations;
vector<double> MPP::penalties;
vector<Dish> MPP::mainDishes;
vector<Dish> MPP::secondDishes;
vector<Dish> MPP::desserts;
vector<pair<int, int>> MPP::foodGroups;
vector<vector<vector<double>>> MPP::dishesCompatibility;
vector<string> MPP::planAllergens;
vector<string> MPP::planIncompt;
vector<int> MPP::planFoodGroups;
vector<double> MPP::planNutritionalFacts;
// Constants
const int MPP::N_OBJ = 2;
const int MPP::PARAMS = 4;
const int MPP::TYPES_OF_DISHES = 3;
const int MPP::NUTRIENTS = 21;
const int MPP::ALLERGENS = 7;
const int MPP::INCOMPATIBILITIES = 5;
const int MPP::FOOD_GROUPS = 10;
const int MPP::PENALTIES = 18;
const double MPP::P_OTHERS = 0.1;
const double MPP::P_MEAT = 3;
const double MPP::P_CEREALS = 0.3;
const double MPP::P_FRUITS = 0.1;
const double MPP::P_DAIRY_PRODUCTS = 0.3;
const double MPP::P_VEGETABLES = 0.3;
const double MPP::P_SEAFOOD = 2;
const double MPP::P_PASTA = 1.5;
const double MPP::P_FISH = 0.5;
const double MPP::P_GREENNESS = 0.1;
const double MPP::P_1D = 3;
const double MPP::P_2D = 2.5;
const double MPP::P_3D = 1.8;
const double MPP::P_4D = 1;
const double MPP::P_5D = 0.2;
const double MPP::P_FIRST_DISHES = 8;
const double MPP::P_SECOND_DISHES = 10;
const double MPP::P_DESSERTS = 2;
const int MPP::N_RECOMMENDATIONS = 21;
const double MPP::MIN_RECOMMENDATIONS = 1.5;
const double MPP::MAX_RECOMMENDATIONS = 2;
const double MPP::R_ACFOL = 135;
const double MPP::R_CAL = 585;
const double MPP::R_EN = 1012;
const double MPP::R_FOS = 562.5;
const double MPP::R_GRA = 31.72;
const double MPP::R_HIE = 8.55;
const double MPP::R_MAG = 112.5;
const double MPP::R_POT = 2025;
const double MPP::R_PRO = 27.08;
const double MPP::R_SEL = 25.75;
const double MPP::R_SOD = 870;
const double MPP::R_VA = 450;
const double MPP::R_VB1 = 0.41;
const double MPP::R_VB2 = 2.28;
const double MPP::R_VB6 = 0.54;
const double MPP::R_VB12 = 2.28;
const double MPP::R_VC = 27;
const double MPP::R_VD = 4.65;
const double MPP::R_VE = 6.3;
const double MPP::R_YOD = 67.5;
const double MPP::R_ZIN = 6.75;

#define DEBUG

// Class Dish
Dish::Dish() {}

Dish::Dish(const string& name_p, const int days, const float price_food,
           const float quant, const vector<int>& foodG, const vector<float>& facts,
           const vector<string>& aller, const vector<string>& incm) : name(name_p),
                                                                      daysWHSelect(days),
                                                                      price(price_food),
                                                                      quantity(quant),
                                                                      foodGroups(foodG),
                                                                      nutritionalFacts(facts),
                                                                      allergens(aller),
                                                                      incompatibilities(incm) {
}

Dish::~Dish() {
    foodGroups.clear();
    nutritionalFacts.clear();
    allergens.clear();
    incompatibilities.clear();
}

ostream& operator<<(ostream& os, const Dish& dish) {
    os << "--- \nDish info" << endl;
    os << "Name: " << dish.name << endl;
    os << "Days without being selected: " << dish.daysWHSelect << endl;
    os << "Price: " << dish.price << endl;
    os << "Quantity: " << dish.quantity << endl;
    os << "Food Groups: ";
    string groups = "";
    for (const int& group : dish.foodGroups) {
        groups += group + " ";
    }
    os << groups << endl;
    os << "Nutritional facts: ";
    groups = "";
    for (const float& fact : dish.nutritionalFacts) {
        groups += to_string(fact) + " ";
    }
    os << groups << endl;
    os << "Allergens: ";
    groups = "";
    for (const string& allerg : dish.allergens) {
        groups += allerg + " ";
    }
    os << groups << endl;
    os << "Incompatibilities: ";
    groups = "";
    for (const string& incmp : dish.incompatibilities) {
        groups += incmp + " ";
    }
    os << groups << "\n---" << endl;
    return os;
}

string Dish::getName(void) {
    return name;
}

void Dish::setName(const string& str) {
    name = str;
}

int Dish::getDaysWHSelect(void) {
    return daysWHSelect;
}

void Dish::setDaysWHSelect(const int days) {
    daysWHSelect = days;
}

float Dish::getPrice(void) {
    return price;
}

void Dish::setPrice(const float p) {
    price = p;
}

float Dish::getQuantity(void) {
    return quantity;
}

void Dish::setQuantity(const float q) {
    quantity = q;
}

int Dish::getFoodGroupAt(const int i) {
    if (i >= foodGroups.size() || i < 0) {
        throw("Accessing foodGropus out of range. Index: " + i);
    }
    return foodGroups[i];
}

void Dish::setFoodGroupAt(const int i, const int group) {
    if (i >= foodGroups.size() || i < 0) {
        throw("Accessing foodGropus out of range. Index: " + i);
    }
    foodGroups[i] = group;
}

float Dish::getNutritionalFactAt(const int i) {
    if (i >= nutritionalFacts.size() || i < 0) {
        throw("Accessing nutritionalFacts out of range. Index: " + i);
    }
    return nutritionalFacts[i];
}

void Dish::setNutritionalFactAt(const int i, const float fact) {
    if (i >= nutritionalFacts.size() || i < 0) {
        throw("Accessing nutritionalFacts out of range. Index: " + i);
    }
    nutritionalFacts[i] = fact;
}

string Dish::getAllergenAt(const int i) {
    if (i >= allergens.size() || i < 0) {
        throw("Accessing allergens out of range. Index: " + i);
    }
    return allergens[i];
}

void Dish::setAllergenAt(const int i, const string& str) {
    if (i >= allergens.size() || i < 0) {
        throw("Accessing allergens out of range. Index: " + i);
    }
    allergens[i] = str;
}

string Dish::getIncompatibilityAt(const int i) {
    if (i >= incompatibilities.size() || i < 0) {
        throw("Accessing incompatibilities out of range. Index: " + i);
    }
    return incompatibilities[i];
}

void Dish::setIncompatibilityAt(const int i, const string& incmp) {
    if (i >= incompatibilities.size() || i < 0) {
        throw("Accessing incompatibilities out of range. Index: " + i);
    }
    incompatibilities[i] = incmp;
}

void Dish::addAllergenBack(const string& allergen) {
    allergens.push_back(allergen);
}

void Dish::addFoodGroupBack(const int group) {
    foodGroups.push_back(group);
}

void Dish::addNutritionalFactBack(const float fact) {
    nutritionalFacts.push_back(fact);
}

void Dish::addIncompatibilityBack(const string& incomp) {
    incompatibilities.push_back(incomp);
}

const vector<int>& Dish::getFoodGroups(void) const {
    return foodGroups;
}

const vector<float>& Dish::getNutritionalFacts(void) const {
    return nutritionalFacts;
}

const vector<string>& Dish::getAllergens(void) const {
    return allergens;
}

const vector<string>& Dish::getIncompatibilites(void) const {
    return incompatibilities;
}

// Class MPP

bool MPP::init(const vector<string>& params) {
    if (params.size() != PARAMS) {
        cout << "Error Menu Planning: params" << endl;
        cerr << "MPP <days> <first_dishes> <second_dishes> <desserts>" << endl;
        return false;
    }
#ifdef DEBUG
    cout << "\n--- Debugging Init ---" << endl;
#endif
    nDays = atoi((params[0]).c_str());
    string first = params[1];
    string second = params[2];
    string dessert = params[3];
    int nParam = nDays * TYPES_OF_DISHES;  //Multiplicar nDays por tipos de platos (3)
    setNumberOfVar(nParam);
    setNumberOfObj(N_OBJ);
    setPenalties();
    setFoodGroups();
    setRecommendations();
    setDishes(first, second, dessert);
    computeCompatibilities();
    planNutritionalFacts.assign(NUTRIENTS, 0);
    planAllergens.assign(ALLERGENS, "0");
    planIncompt.assign(INCOMPATIBILITIES, "0");
#ifdef DEBUG
    cout << "Days: " << nDays << endl;
    cout << "FDishes: " << first << endl;
    cout << "SDishes: " << second << endl;
    cout << "Desserts: " << dessert << endl;
    cout << "num_tipoPlato: " << TYPES_OF_DISHES << endl;
    cout << "NParams: " << nParam << endl;
    cout << "NObjs: " << N_OBJ << endl;
#endif
    return true;
}

void MPP::setDishes(string& first, string& second, string& dessert) {
#ifdef DEBUG
    cout << "\n --- Debugging setPlatos --- " << endl;
    cout << "Calling Primeros Platos" << endl;
#endif
    setDishes(first.c_str(), mainDishes);
#ifdef DEBUG
    cout << "Calling Segundos Platos" << endl;
#endif
    setDishes(second.c_str(), secondDishes);
#ifdef DEBUG
    cout << "Calling Postres" << endl;
#endif
    setDishes(dessert.c_str(), desserts);
}

void MPP::setPenalties(void) {
    penalties.reserve(PENALTIES);
    penalties.emplace_back(P_OTHERS);          //otros
    penalties.emplace_back(P_MEAT);            //carnes
    penalties.emplace_back(P_CEREALS);         //cereales
    penalties.emplace_back(P_FRUITS);          //frutas
    penalties.emplace_back(P_DAIRY_PRODUCTS);  //lacteos
    penalties.emplace_back(P_VEGETABLES);      //legumbres
    penalties.emplace_back(P_SEAFOOD);         //mariscos
    penalties.emplace_back(P_PASTA);           //pastas
    penalties.emplace_back(P_FISH);            //pescados
    penalties.emplace_back(P_GREENNESS);       //verduras
    penalties.emplace_back(P_1D);              // 1 dia
    penalties.emplace_back(P_2D);              //2 dias
    penalties.emplace_back(P_3D);              //3 dias
    penalties.emplace_back(P_4D);              //4 dias
    penalties.emplace_back(P_5D);              //5 dias
    penalties.emplace_back(P_FIRST_DISHES);    //primer platos
    penalties.emplace_back(P_SECOND_DISHES);   //segundo plato
    penalties.emplace_back(P_DESSERTS);        //postre
#ifdef DEBUG
    cout << "\n --- Debugging penalties  ---" << endl;
    for (const auto penalizacion : penalties) {
        cout << penalizacion << endl;
    }
#endif
}

void MPP::setFoodGroups(void) {
    pair<int, int> ga;
    ga.first = numeric_limits<int>::max();
    ga.second = numeric_limits<int>::max();
    foodGroups.assign(FOOD_GROUPS, ga);
#ifdef DEBUG
    cout << "\n --- Debugging foodGroups --- " << endl;
    cout << "Ga.first: " << ga.first << endl;
    cout << "Ga.second: " << ga.second << endl;
    cout << "num_gAl: " << FOOD_GROUPS << endl;
#endif
}

void MPP::setDishes(const char* c_filename,
                    vector<Dish>& v_vecP) {
    ifstream ifs;
    int i;
    int cont = 1;
#ifdef DEBUG
    cout << "\n --- Debugging set_VectoresPlatos ---" << endl;
    cout << "File: " << c_filename << endl;
#endif
    ifs.open(c_filename, ifstream::in);
    if (ifs.is_open()) {
        while (ifs.good()) {
            string str;
            Dish dish;
            getline(ifs, str, ',');
            dish.name = str;  //Nombre del plato
            getline(ifs, str, ',');
            dish.price = atof(str.c_str());  //Precio
            getline(ifs, str, ',');
            dish.quantity = atof(str.c_str());               //Cantidad
            dish.daysWHSelect = numeric_limits<int>::max();  //Dias desde que se repitio
            for (i = 0; i < ALLERGENS; i++) {                //Alergenos
                getline(ifs, str, ',');
                dish.allergens.push_back(str);
            }
            for (i = 0; i < INCOMPATIBILITIES; i++) {  //Incompatibilidades
                getline(ifs, str, ',');
                dish.incompatibilities.push_back(str);
            }
            for (i = 0; i < NUTRIENTS; i++) {  //Informacion nutricional
                getline(ifs, str, ',');
                dish.nutritionalFacts.push_back(atof(str.c_str()));
            }
            getline(ifs, str, ',');
            while (str != "*" && !ifs.eof()) {
                dish.foodGroups.push_back(atoi(str.c_str()));
                getline(ifs, str, ',');
                for (int x = 0; x < str.size(); x++)
                    if (str.at(x) == '\n')
                        str = "*";
            }
            v_vecP.push_back(dish);
        }
        ifs.close();
    } else {
        cout << "\n\nError. No se han podido leer los archivos de platos: ";
        cout << c_filename << endl;
        cin.get();
        exit(-1);
    }
#ifdef DEBUG
    cout << "v_vecP size: " << v_vecP.size() << endl;
#endif
}

void MPP::setRecommendations(void) {
    pair<double, double> ir;
    recommendations.reserve(N_RECOMMENDATIONS);
    ir.first = ir.second = R_ACFOL;
    recommendations.emplace_back(ir);  //acido folico
    ir.first = ir.second = R_CAL;
    recommendations.emplace_back(ir);  //calcio
    ir.first = ir.second = R_EN;
    recommendations.emplace_back(ir);  //energia
    ir.first = ir.second = R_FOS;
    recommendations.emplace_back(ir);  //fosforo
    ir.first = ir.second = R_GRA;
    recommendations.emplace_back(ir);  //grasa
    ir.first = ir.second = R_HIE;
    recommendations.emplace_back(ir);  //hierro
    ir.first = ir.second = R_MAG;
    recommendations.emplace_back(ir);  //magnesio
    ir.first = ir.second = R_POT;
    recommendations.emplace_back(ir);  //potasio
    ir.first = ir.second = R_PRO;
    recommendations.emplace_back(ir);  //proteinas
    ir.first = ir.second = R_SEL;
    recommendations.emplace_back(ir);  //selenio
    ir.first = ir.second = R_SOD;
    recommendations.emplace_back(ir);  //sodio
    ir.first = ir.second = R_VA;
    recommendations.emplace_back(ir);  //vitA
    ir.first = ir.second = R_VB1;
    recommendations.emplace_back(ir);  //vitB1
    ir.first = ir.second = R_VB12;
    recommendations.emplace_back(ir);  //vitB2
    ir.first = ir.second = R_VB6;
    recommendations.emplace_back(ir);  //vitB6
    ir.first = ir.second = R_VB12;
    recommendations.emplace_back(ir);  //vitB12
    ir.first = ir.second = R_VC;
    recommendations.emplace_back(ir);  //vitC
    ir.first = ir.second = R_VD;
    recommendations.emplace_back(ir);  //vitD
    ir.first = ir.second = R_VE;
    recommendations.emplace_back(ir);  //vitE
    ir.first = ir.second = R_YOD;
    recommendations.emplace_back(ir);  //yodo
    ir.first = ir.second = R_ZIN;
    recommendations.emplace_back(ir);  //zinc
    for (int i = 0; i < recommendations.size(); i++) {
        ir.first = (recommendations[i].first -
                    recommendations[i].first / MIN_RECOMMENDATIONS) *
                   nDays;
        ir.second = (recommendations[i].second * MAX_RECOMMENDATIONS) * nDays;
        recommendations[i] = ir;
    }
#ifdef DEBUG
    cout << "\n --- Debugging recommendations --- " << endl;
    cout << "ingRe size: " << recommendations.size() << endl;
#endif
}

void MPP::computeCompatibilities(void) {
    vector<double> sz(desserts.size(), 0.0);              //Vector de postres
    vector<vector<double>> sz2(secondDishes.size(), sz);  //Plano de postres y segundos platos
    dishesCompatibility.assign(mainDishes.size(), sz2);   //Prisma rectangular de primeros, segundos platos y postres
    vector<bool> gaElegidos;
    for (int y = 0; y < mainDishes.size(); y++)
        for (int x = 0; x < secondDishes.size(); x++)
            for (int z = 0; z < desserts.size(); z++) {
                gaElegidos.clear();
                gaElegidos.assign(FOOD_GROUPS, false);
                setChoosenFoodG(mainDishes[y].foodGroups, gaElegidos);
                dishesCompatibility[y][x][z] =
                    setPenalitiesToCompatibilities(secondDishes[x].foodGroups, gaElegidos);
                setChoosenFoodG(secondDishes[x].getFoodGroups(), gaElegidos);
                dishesCompatibility[y][x][z] +=
                    setPenalitiesToCompatibilities(desserts[z].foodGroups, gaElegidos);
            }
#ifdef DEBUG
    cout << "\n --- Debugging VectorCompatibilidad ---" << endl;
    cout << "Size primeros platos: " << mainDishes.size() << endl;
    cout << "Size segundos platos: " << secondDishes.size() << endl;
    cout << "Size postres: " << desserts.size() << endl;
    for (int y = 0; y < mainDishes.size(); y++) {
        for (int x = 0; x < secondDishes.size(); x++) {
            for (int z = 0; z < desserts.size(); z++) {
                cout << "dishesCompatibility[y: " << y << "][x: " << x << "][z: " << z << "]: ";
                cout << dishesCompatibility[y][x][z] << endl;
            }
        }
    }
#endif
}

void MPP::setChoosenFoodG(vector<int> gal, vector<bool>& galE) {
    for (int i = 0; i < gal.size(); i++)
        galE[gal[i]] = true;
}

double MPP::setPenalitiesToCompatibilities(const vector<int>& gal, vector<bool> galE) {
    double resultado = 0.0;
    for (int i = 0; i < gal.size(); i++) {
        if (galE[gal[i]])
            switch (gal[i]) {
                case 0:
                    resultado += penalties[0];
                    break;
                case 1:
                    resultado += penalties[1];
                    break;
                case 2:
                    resultado += penalties[2];
                    break;
                case 3:
                    resultado += penalties[3];
                    break;
                case 4:
                    resultado += penalties[4];
                    break;
                case 5:
                    resultado += penalties[5];
                    break;
                case 6:
                    resultado += penalties[6];
                    break;
                case 7:
                    resultado += penalties[7];
                    break;
                case 8:
                    resultado += penalties[8];
                    break;
                case 9:
                    resultado += penalties[9];
                    break;
                default:
                    break;
            }
    }
    return resultado;
}

void MPP::restart(void) {
#ifdef DEBUG
    cout << "Running restart @ MPP" << endl;
#endif
    for (int i = 0; i < getNumberOfVar(); i++)
        setVar(i, (double)(rand() % (int)getMaximum(i)));
}

Individual* MPP::clone(void) const {
    return new MPP();
}

void MPP::dependentCrossover(Individual* ind) {
    crossover_OPC(ind);
    ((MPP*)ind)->repair();
    repair();
}

void MPP::dependentMutation(double pm) {
    int x;
    bool mod = false;
    for (int i = 0; i < nDays; i++) {
        x = rand() % 100;
        if (x < (pm * 100)) {
            setVar(i * TYPES_OF_DISHES, rand() % mainDishes.size());
            setVar(i * TYPES_OF_DISHES + 1, rand() % secondDishes.size());
            setVar(i * TYPES_OF_DISHES + 2, rand() % desserts.size());
            mod = true;
        }
    }
    if (mod)
        repair();
}

void MPP::repair(void) {
    bool mod = false;
    while (!checkInfoN2()) {
        for (int i = 0; i < nDays; i++) {
            setVar(i * TYPES_OF_DISHES, rand() % mainDishes.size());
            setVar(i * TYPES_OF_DISHES + 1, rand() % secondDishes.size());
            setVar(i * TYPES_OF_DISHES + 2, rand() % desserts.size());
        }
        mod = true;
    }
    if (mod)
        evaluate();
}

bool MPP::checkInfoN(const int i) {
    double aux;
    for (int j = 0; j < NUTRIENTS; j++) {
        aux = mainDishes[getVar(i * TYPES_OF_DISHES)].nutritionalFacts[j] +
              secondDishes[getVar(i * TYPES_OF_DISHES + 1)].nutritionalFacts[j] +
              desserts[getVar(i * TYPES_OF_DISHES + 2)].nutritionalFacts[j];
        if (aux < recommendations[j].first || aux > recommendations[j].second)
            return false;
    }
    return true;
}

bool MPP::checkInfoN2(void) {
    double aux;
    for (int j = 0; j < NUTRIENTS; j++) {
        aux = 0;
        for (int i = 0; i < nDays; i++) {
            aux += mainDishes[getVar(i * TYPES_OF_DISHES)].nutritionalFacts[j] +
                   secondDishes[getVar(i * TYPES_OF_DISHES + 1)].nutritionalFacts[j] +
                   desserts[getVar(i * TYPES_OF_DISHES + 2)].nutritionalFacts[j];
        }
        if (aux < recommendations[j].first || aux > recommendations[j].second)
            return false;
    }
    return true;
}

void MPP::evaluate(void) {
    double precioTotal = 0;
    double valPP, valSP, valP, valTabla, valGAFirst;
    double numPP = penalties[15];
    double numSP = penalties[16];
    double numP = penalties[17];
    double valTotal = 0;
    vector<int> gaElegidos;          //Vector que guarda los grupos alimenticios pertenecientes a los platos elegidos
    vector<int> gaElegidosAnterior;  //Vector que guarda los ga pertenecientes al menu de la iteracion anterior
    vector<vector<int>> ultimos5GA;
    int x = 0;
#ifdef DEBUG
    cout << "\n --- Debugging Evaluate ---" << endl;
#endif
    for (int i = 0; i < nDays; i++) {
        x = i * TYPES_OF_DISHES;
        //- PRECIO -------------------------------------------------------------------------------------------------------------
        precioTotal += mainDishes[getVar(x)].price +
                       secondDishes[getVar(x + 1)].price +
                       desserts[getVar(x + 2)].price;

        //- INFORMACION NUTRICIONAL --------------------------------------------------------------------------------------------
        for (int j = 0; j < planNutritionalFacts.size(); j++)
            planNutritionalFacts[j] += mainDishes[getVar(x)].nutritionalFacts[j] +
                                       secondDishes[getVar(x + 1)].nutritionalFacts[j] +
                                       desserts[getVar(x + 2)].nutritionalFacts[j];
        setObj(0, precioTotal);
#ifdef DEBUG
        cout << "Day: " << i << endl;
        cout << "Num_tipoPlato: " << TYPES_OF_DISHES << endl;
        cout << "X: " << x << endl;
        cout << "Price: " << precioTotal << endl;
#endif
    }
    //- GRADO DE REPETICION -----------------------------------------------------------------------------------------------------
    for (int i = 0; i < nDays; i++) {
        x = i * TYPES_OF_DISHES;
        //PRIMER PLATO
        valPP = setValorPP(getVar(x));  //Numero de dias desde que se repitio el plato seleccionado
        for (int j = 0; j < mainDishes[getVar(x)].foodGroups.size(); j++) {
            //comprueba si ya habia aparecido en el menu el grupo alimenticio j, si no lo a�ade al vector gaElegidos
            if (foodGroupsByIteration(gaElegidos,
                                       mainDishes[getVar(x)].foodGroups[j]))
                gaElegidos.push_back(mainDishes[getVar(x)].foodGroups[j]);
        }
        //SEGUNDO PLATO
        valSP = setValorSP(getVar(x + 1));
        for (int k = 0; k < secondDishes[getVar(x + 1)].foodGroups.size(); k++) {
            if (foodGroupsByIteration(gaElegidos,
                                       secondDishes[getVar(x + 1)].foodGroups[k]))
                gaElegidos.push_back(secondDishes[getVar(x + 1)].foodGroups[k]);
        }
        //POSTRE
        valP = setValorP(getVar(x + 2));
        for (int l = 0; l < desserts[getVar(x + 2)].foodGroups.size(); l++) {
            if (foodGroupsByIteration(gaElegidos,
                                       desserts[getVar(x + 2)].foodGroups[l]))
                gaElegidos.push_back(desserts[getVar(x + 2)].foodGroups[l]);
        }
        valTabla = dishesCompatibility[getVar(x)][getVar(x + 1)][getVar(x + 2)];  //Obtener el valor de la tabla de platos de compatibilidad entre primeros y segundos platos
        valGAFirst = set_ValorGAFirstAlternativa(ultimos5GA, gaElegidos);             //Obtener el valor total del numero de dias desde que se repitieron grupos alimenticios
        valTotal += valTabla + numPP / valPP + numSP / valSP + numP / valP +
                    valGAFirst;
        sumValorPP();  //Suma los valores de platos y grupos alimenticios elegidos para el siguiente dia
        sumValorSP();
        sumValorP();
        sumValorGA();
        set_ultimos5GA(ultimos5GA, gaElegidos);
        gaElegidosAnterior = gaElegidos;
        gaElegidos.clear();
#ifdef DEBUG
        cout << "i: " << i << endl;
        cout << "Days: " << nDays << endl;
        cout << "ValTotal: " << valTotal << endl;
#endif
    }
    ultimos5GA.clear();
    gaElegidosAnterior.clear();
    setObj(1, valTotal);
}

bool MPP::foodGroupsByIteration(vector<int> vec, int valor) {
    bool resultado = true;
    for (int i = 0; i < vec.size(); i++)
        if (vec[i] == valor)
            resultado = false;
    return resultado;
}

int MPP::setValorPP(int id) {
    int valor = numeric_limits<int>::max();                         //Tengo que retornar el numero de dias desde que se eligio el plato por ultima vez
    if (mainDishes[id].daysWHSelect != numeric_limits<int>::max())  //Si el numero de dias es i_max, significa que nunca se ha elegido, por lo que retorno 0
        valor = mainDishes[id].daysWHSelect;                        //Si el numero de dias es distinto a i_max, retorno el valor y reseteo el numero de dias a 0
    mainDishes[id].daysWHSelect = 0;
    return valor;
}

int MPP::setValorSP(int id) {
    int valor = numeric_limits<int>::max();
    if (secondDishes[id].daysWHSelect != numeric_limits<int>::max())
        valor = secondDishes[id].daysWHSelect;
    secondDishes[id].daysWHSelect = 0;
    return valor;
}

int MPP::setValorP(int id) {
    int valor = numeric_limits<int>::max();
    if (desserts[id].daysWHSelect != numeric_limits<int>::max())
        valor = desserts[id].daysWHSelect;
    desserts[id].daysWHSelect = 0;
    return valor;
}

void MPP::sumValorPP(void) {
    for (int i = 0; i < mainDishes.size(); i++)
        if (mainDishes[i].daysWHSelect != numeric_limits<int>::max())
            mainDishes[i].daysWHSelect++;
}

void MPP::sumValorSP(void) {
    for (int i = 0; i < secondDishes.size(); i++)
        if (secondDishes[i].daysWHSelect != numeric_limits<int>::max())
            secondDishes[i].daysWHSelect++;
}

void MPP::sumValorP(void) {
    for (int i = 0; i < desserts.size(); i++)
        if (desserts[i].daysWHSelect != numeric_limits<int>::max())
            desserts[i].daysWHSelect++;
}

void MPP::sumValorGA(void) {
    for (int i = 0; i < foodGroups.size(); i++) {
        if (foodGroups[i].first != numeric_limits<int>::max())
            foodGroups[i].first++;
        foodGroups[i].second = numeric_limits<int>::max();
    }
}

//METODO PARA ANIADIR LOS GA DE UN DIA EN EL VECTOR DE GAS DE LOS ULTIMOS 5 DIAS
void MPP::set_ultimos5GA(vector<vector<int>>& ultimos5GA, vector<int> vec) {
    if (ultimos5GA.size() < 5)
        ultimos5GA.push_back(vec);
    else {
        ultimos5GA.erase(ultimos5GA.begin());
        ultimos5GA.push_back(vec);
    }
}

double MPP::set_ValorGAFirstAlternativa(vector<vector<int>>& ultimos5GA, vector<int> vec) {
    /* 0 Otros, 1 Carne, 2 Cereal, 3 Fruta, 4 Lacteo, 5 Legumbre, 6 Marisco, 7 Pasta, 8 Pescado, 9 Verdura */
    double penalizacionPorGA[10];
    for (int i = 0; i < 10; i++) {
        penalizacionPorGA[i] = penalties[i];
    }
    //double penalizacionPorDias[5] = {penalties[10],penalties[11],penalties[12],penalties[13],penalties[14]};
    double penalizacionPorDias[5];
    for (int i = 10, j = 0; i < 15; i++, j++) {
        penalizacionPorDias[j] = penalties[i];
    }
    bool pen[5] = {false, false, false, false, false};
    double resultado = 0;
    if (ultimos5GA.size() > 0) {
        for (int i = 0; i < vec.size(); i++) {
            for (int j = 0; j < ultimos5GA.size(); j++)
                for (int k = 0; k < ultimos5GA[j].size(); k++) {
                    if (vec[i] == ultimos5GA[j][k]) {
                        pen[j] = true;
                        resultado += penalizacionPorGA[vec[i]];
                    }
                }
        }
        for (int x = 0; x < 5; x++)
            if (pen[x]) {
                resultado += penalizacionPorDias[x];
                pen[x] = false;
            }
    }
    return resultado;
}

ostream& operator<<(ostream& os, const MPP& mpp) {
    os << "\n\n--- Main dishes ---\n\n";
    for (const Dish& dish : mpp.mainDishes) {
        os << dish;
    }
    os << "\n\n--- Second dishes ---\n\n";
    for (const Dish& dish : mpp.secondDishes) {
        os << dish;
    }
    os << "\n\n--- Desserts ---\n\n";
    for (const Dish& dish : mpp.desserts) {
        os << dish;
    }
    return os;
}
