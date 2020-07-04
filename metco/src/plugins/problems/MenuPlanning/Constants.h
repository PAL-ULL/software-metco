
/**
 *  Alejandro Marrero - alu0100825008@ul.edu.es
 *  06-07-2020
 **/

#ifndef __MPP_CONSTANTS_H__
#define __MPP_CONSTANTS_H__

#include <vector>

// Datos generales
const int num_tipoPlato = 3;
const int num_nutr = 21;
const int num_alerg = 7;
const int num_incomp = 5;
const int num_gAl = 10;
const int num_obj = 2;

// Penalizaciones
const int NUM_PENALTIES = 18;
const double p_otros = 0.1;
const double p_carnes = 3;
const double p_cereales = 0.3;
const double p_frutas = 0.1;
const double p_lacteos = 0.3;
const double p_legumbres = 0.3;
const double p_marisco = 2;
const double p_pasta = 1.5;
const double p_pescado = 0.5;
const double p_verdura = 0.1;
const double p_1d = 3;
const double p_2d = 2.5;
const double p_3d = 1.8;
const double p_4d = 1;
const double p_5d = 0.2;
const double p_pp = 8;
const double p_sp = 10;
const double p_p = 2;

const std::vector<double> penalizaciones = {
    p_otros,   p_carnes, p_cereales, p_frutas,  p_lacteos, p_legumbres,
    p_marisco, p_pasta,  p_pescado,  p_verdura, p_1d,      p_2d,
    p_3d,      p_4d,     p_5d,       p_pp,      p_sp,      p_p};

// Cantidad de nutrientes recomedados (por almuerzo)
const int CALCIUM_INDEX = 1;
const int ENERGY_INDEX = 2;
const int FAT_INDEX = 4;
const int IRON_INDEX = 5;
const int POTASIUM_INDEX = 7;
const int PROTEIN_INDEX = 8;

// Nutrientes por dia
const int FORCED_INDEXES_SIZE = 3;
const int FORCED_INDEXES[3] = {ENERGY_INDEX, FAT_INDEX, PROTEIN_INDEX};
const double FORCED_MIN[3] = {0.85, 0.75, 0.75};
const double FORCED_MAX[3] = {11.15, 1.25, 1.25};

// Cantidad de nutrientes recomedados (por almuerzo)
const double ingR_acFol = 135;
const double ingR_cal = 585;
const double ingR_en = 1012;
const double ingR_fos = 562.5;
const double ingR_gra = 31.72;
const double ingR_hie = 8.55;
const double ingR_mag = 112.5;
const double ingR_pot = 2025;
const double ingR_pro = 27.08;
const double ingR_sel = 25.75;
const double ingR_sod = 870;
const double ingR_vA = 450;
const double ingR_vB1 = 0.41;
const double ingR_vB2 = 0.63;
const double ingR_vB6 = 0.54;
const double ingR_vB12 = 2.28;
const double ingR_vC = 27;
const double ingR_vD = 4.65;
const double ingR_vE = 6.3;
const double ingR_yod = 67.5;
const double ingR_zin = 6.75;

const double ingR[21] = {
    ingR_acFol, ingR_cal,  ingR_en,  ingR_fos, ingR_gra, ingR_hie, ingR_mag,
    ingR_pot,   ingR_pro,  ingR_sel, ingR_sod, ingR_vA,  ingR_vB1, ingR_vB2,
    ingR_vB6,   ingR_vB12, ingR_vC,  ingR_vD,  ingR_vE,  ingR_yod, ingR_zin};

const double minReq[21] = {0.70, 0.70, 0.90, 0.70, 0.90, 0.70, 0.70,
                           0.70, 0.90, 0.70, 0.70, 0.70, 0.70, 0.70,
                           0.70, 0.70, 0.70, 0.70, 0.70, 0.70, 0.70};
const double maxReq[21] = {1.30, 1.30, 1.10, 1.30, 1.10, 1.30, 1.30,
                           1.30, 1.10, 1.30, 1.30, 1.30, 1.30, 1.30,
                           1.30, 1.30, 1.30, 1.30, 1.30, 1.30, 1.30};
const int NPLATOS[3] = {19, 34, 14};

#endif