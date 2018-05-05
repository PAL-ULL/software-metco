#ifndef __TDCLP_H__
#define __TDCLP_H__

#include "Individual.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

const long double WMAX = 7200.0;

typedef struct {
        int idItem;
        int pat;
        double len;
        double wid;
        double hei;
        double wei;
        double vol;
        int orient;
        bool pack;
        int number;
        int index;   // Mantener actualizados auxWei y auxWid
} P_Order;


typedef struct {
        int id;
        int numberPieces;

} P_Cont;


class Holes {
        public: 
                Holes();
                Holes(double H_wid, double H_len, double H_hei, double H_vol);
                inline double getWid() {return H_wid;}
                inline double getLen() {return H_len;}
                inline double getHei() {return H_hei;}
                inline double getVol() const {return H_vol;}
                
        private:
                double H_wid;
                double H_len;
                double H_hei;
                double H_vol;
};


class TDCLP : public Individual{
public:
        // Destructor
        ~TDCLP (void){}
        
        // Initialization method
        bool init (const vector<string> &params);
        
        // Evaluation
        void evaluate (void);

         // Clone
         Individual* clone (void) const;
         
         // Print
         void print (ostream &os) const;
         
         // Reads a problem from file
         bool readFile(const string filename);

        // Generation of an individual
        void restart(void);
        void randomRestart(void);

        // Mutation
        void mutation_moves(double pm);
        void mutation_add(double pm);
        void mutation_remove(double pm);
        void mutation_replace(double pm);
        void mutation_change(double pm);
        void mutation_swap(double pm);
        void mutation_compact(double pm);
        void mutation_shift(double pm);

        
        // Funciones de la mutacion
        int count_type(int type);
        void create_vector(vector<int> &positions, int type, int pos);
        
        // Crossover
        void crossover_onepoint(Individual *ind);
        // Generacion aleatoria
        int uniformRandom(int maxValue) const;
        
        // Maximo, Minimo y Direccion
        double inline getMaximum(const int i) const { return 5; }
        double inline getMinimum(const int i) const { return 0; }
        unsigned int getOptDirection(const int i) const;

        // Heuristica
        void orientation_pieces(int id, int rot);
        void fill_items(double *width, double *depth, double *height);
        void fillVect();
        int count_items(double num, int count_level, int type);
        void sortItems();
        void heuristic_AC();
        void heuristic_AS();
        void heuristic3();
        void heuristic_LC();
        void heuristic_LS();
        
        void fixsolutions();
        void printInd(Individual *ind);
private:
        void dependentMutation (double pm);
        void dependentCrossover (Individual* ind);
                                        
        // Length and width of patterns
        static double *len;
        static double *wid;
        static double *hei;
        static double *wei;
        static int *orient;
        static int *number;
        
        static vector<int> ori1;
        static vector<int> ori2;
        static vector<int> ori3;
        
        static double widSheet, lenSheet, heiSheet, volSheet, weiSheet;  // Width, Length and Height of mother sheet

        static vector<int> cont_items;  // Cada celda contiene el numero de piezas de cada tipo
        
        int sizeGen;
        int widR, depR, heiR;
        
        unsigned int type_heuristic;
        unsigned int type_mutation;
        
        static int numPat;                               // Number of patterns
        static int totalNumPat;                          // Number total of pieces 
        
        vector<P_Order> items;    // Items ordenados segun la heuristica
        vector<P_Order> itemsCont;
        // Variables

        vector<Holes> holesF;       // Vector de huecos Front dentro del contenedor
        vector<Holes> holesA;       // Vector de huecos Above dentro del contenedor
        vector<Holes> holesB;       // Vector de huecos Beside dentro del contenedor

        // Globales
        static bool load;
};
#endif