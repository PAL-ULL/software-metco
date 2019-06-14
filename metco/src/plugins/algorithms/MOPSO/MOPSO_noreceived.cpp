/***********************************************************************************
 * AUTORES
 *   - Alejandro Cervantes
 * 
 * FECHA
 *   Diciembre 2007
 *
 * *********************************************************************************/


#include "MOPSO.h"
#include "randomlib.h"

// Constructor
MOPSO::MOPSO () {
}


// Destructor
MOPSO::~MOPSO (void) {
	free_arrays();
}


// Define una generaci?n de b?squeda del algoritmo
void MOPSO::runGeneration() {

//  cout << "...MOPSO::runGeneration initial population at generation " << getGeneration() << endl << flush; // Alex
// printMOPSOPopulation(getGeneration(),5);

	if (primeraGeneracion)
	{
		primeraGeneracion=false;
		for(i = 0; i < M; i++)
		{
			for(j = 0; j < D; j++)
	        {
//	          vel[(D * i) + j] = 0.0;
	          vel[(D * i) + j] = RandomDouble(-1.0, 1.0);
	        }
		}
		copy_fbests();
		copy_pbests();

		evaluation();

	    searchAndInsert(noDomP, noDomF, opt, &lastPart,fun);
	    makeHyper(noDomF, linf, lsup, amp, start, hyperspace, partPos, ndiv, MEM, NF, lastPart, maxCube);
	}
	else
	{
		setGenerateArchive (true);

//		for (int g=0; g<pStep;g++){//bucle para ejecutar en local
	//	while(getPerformedEvaluations() < getEvaluations()){//bucle para ejecutar en local

/*if(getPerformedEvaluations() < (getEvaluations()-150))
		setGenerateArchive (true);*/

			hyperFit(hyperspace, hyperPoolL, hyperPoolF, &hypsum, maxCube, &poolC);
			state=false;
	      
			// Actualiza las velocidades de todas las particulas
			for(i = 0; i < M; i++)
			{
				if(state==false)
				  state=true;
				h = lead(hyperspace, hyperPoolL, partPos, hyperPoolF, hypsum, lastPart, poolC);
				for(j = 0; j < D; j++)
				{
					double pos = (*population)[i]->getVar(j);
					vel[(D * i) + j] = velocity(0.4, vel[(D * i) + j], 1.0, 1.0, pbests[(D * i) + j], noDomP[(D * h) + j], pos);
				}
			}

			// Actualiza las posiciones de todas las particulas
			for(i = 0; i < M; i++)
			{
		      for(j = 0; j < D; j++)
		      {
					double pos = (*population)[i]->getVar(j);
					double newpos = pos + vel[(D * i) + j];
					if (newpos >= 1.0)
					{
						newpos = 1.0;
		 				vel[(D * i) + j] = -vel[(D * i) + j];
					}
					else if (newpos <=0.0)
					{
						newpos = 0.0;
		 				vel[(D * i) + j] = -vel[(D * i) + j];
					}
					(*population)[i]->setVar(j, newpos);
		      }
			}

			// Mutacion
		/*    if(getGeneration()<Gmax*pM){
		      //cout<<t<<endl;
		      muta(pM, M, D,getGeneration(),Gmax,fun);
		    }*/
			muta(pM, M, D,1,80,fun);
	    
		    // Evaluacion
		    evaluation();

			int inserted  = searchAndInsert2(noDomP, noDomF, linf, lsup, amp, start, hyperspace, partPos, ndiv, opt, maxCube, &lastPart,fun);
			int exchanged = ifbestInterchange(fbests, pbests,opt,fun);
//		} // fin de bucle para ejecutar en local
//		printMOPSOArchive(getGeneration());

	}

//    cout << "...MOPSO::runGeneration archive at generation " << getGeneration() << endl << flush; // Alex
//	printMOPSOArchive(getGeneration(),true);

// cout << "...MOPSO::runGeneration end " << getGeneration() << endl << flush; // Alex
	
}


// Inicializa los par?metros iniciales del algoritmo
bool MOPSO::initParams(const vector<double> &params) {

cout << "...MOPSO::initParams begin" << endl << flush; // Alex
	
	// Check for the proper parameters
	if (params.size() != 5) {
		cerr << "Error MOPSO: Incorrect parameters (pm pSize aSize nDiv gMax)" << endl;
//		cerr << " zdt1: fun=30001 nPars=30 nFun=2" << endl;
		return false;
	}

	// Initiate the parameters
	this->pm = params[0];
	pM          = this->pm;

//	this->pSize       = (int)params[1];
	M           = (int)params[1];	
	setPopulationSize((int)params[1]);

	this->aSize = (int)params[2];
	MEM = this->aSize;

	ndiv = (int)params[3];   // Numero de divisiones del archivo
	Gmax = (int)params[4];   // Iteracion hasta la que hay mutacion
//	pStep = (int)params[5];   // Iteracion en la que se muestra resultado
//	D    = (int)params[6];   // Numero de parametros del problema
//	NF   = (int)params[7];   // Numero de funciones del problema

//	opt = (int)params[5];   // Sentido de la optimizacion
	fun  = 0;   // Obsoleto, identifica la funcion que se optimiza

    // Other params are taken from the individual
	const Individual *anInd = getSampleInd();
    D  = anInd->getNumberOfVar();
    NF = anInd->getNumberOfObj();


	initialize_arrays();  // Call the original MOPSO init code

cout << "...MOPSO::initParams" << endl << flush; // Alex
cout << "........M=" <<  M << endl << flush; // Alex
cout << "......MEM=" <<  MEM << endl << flush; // Alex
cout << ".....ndiv=" <<  ndiv << endl << flush; // Alex
cout << "........D=" <<  D << endl << flush; // Alex
cout << ".......NF=" <<  NF << endl << flush; // Alex

    // Get the direction of optimization from the sample individual
    // once the array has been created
    for (unsigned int i=0; i<NF;i ++)
    {
        opt[i]=anInd->getInternalOptDirection(i);
cout << "......opt[" << i << "]=" << opt[i] << endl << flush; // Alex
    }

	return true;
}


/*
 * Muestra la poblacion por stdout
 * Unicamente muestra las primeras dimensiones (parametro dim)
 */
void MOPSO::printMOPSOPopulation(int generation,int dim) {
    cout << "# Population Iteration " << generation << std::endl << std::flush;

	for (int i = 0;  i < getPopulationSize(); i++)
	{
	    for(int j = 0; j < dim; j++)
	    {
	      if (j>0) 
	      {
	      	std::cout << ","; 
	      }
	      std::cout << (*population)[i]->getVar(j);
	    }
      std::cout << std::endl;
	}
}

/*
 *  Recorre el archivo interno y crea un individuo para cada elemento
 *  Inserta dicho individuo en el frente proporcionado
 */


void MOPSO::printMOPSOArchive(int generation,bool printVars) {
cout << "# Front Iteration " << generation << std::endl << std::flush;

	for (int i = 0;  i < lastPart; i++)
	{
        int j=0;
        if (printVars)
        {
    	    for(j = 0; j < D; j++)
    	    {
    	      if (j>0) 
    	      {
    	      	std::cout << ","; 
    	      }
    	      std::cout << noDomP[(D * i) + j];
    	    }
        }
	    for(j = 0; j < NF; j++)
	    {
	      if (j>0) 
	      {
	      	std::cout << ","; 
	      }
	      std::cout << noDomF[(NF * i) + j];
	    }
      std::cout << std::endl;
	}
}

  
void MOPSO::getSolution(MOFront *p) const {

cout << "...MOPSO::getSolution" << endl << flush; // Alex
//cout << "# Front " << i << std::endl << std::flush;

	Individual *anInd = (*population)[0];
//		Individual *in = sampleInd->internalClone(&noDomP[D*i], &noDomF[NF*i], &migrationData);
//		Individual *in = getSampleInd()->internalClone();

	double migrationData = 0.0;

	for (unsigned int i = 0;  i < lastPart; i++)
	{
		for (int dim=0;dim<D;dim++) {
		  anInd->setVar(dim,noDomP[D*i+dim]);
        }
		for (int objf=0;objf<NF;objf++) {
		  anInd->setObj(objf,noDomP[i*NF+objf]);
        }
		p->insert(anInd);
	}


//cout << "...MOPSO::getSolution end" << endl << flush; // Alex

}



// Muestra la informaci?n relativa al algoritmo
void MOPSO::printInfo(ostream &os) const {
	os << "Algorithm MOPSO" << endl;
	os << "Number of Evaluations = " << getPerformedEvaluations() << endl;
	os << "Mutation Probability = " << pm << endl;
	os << "Population Size = " << getPopulationSize() << endl;				
	os << "Archive Size = " << pc << endl;
	os << "Archive Divisions = " << ndiv << endl;
	os << "Problem dimension = " << D << endl;
	os << "Problem objectives = " << NF << endl;
}


/**************************************************************
 * Los siguientes metodos son especificos de MOPSO
 * Tomados del codigo de referencia de MOPSO (por Claudio Coello)
 *
 * *************************************************************/

void MOPSO::evaluation()
{
	for (int i = getPopEvaluatedIndividuals(); i < getPopulationSize(); i++)
	{
	    evaluate((*population)[i]);
	    insertInArchive((*population)[i]);
	}
}


void MOPSO::initialize_arrays () {
// INITIALIZATION CODE FROM COELLO MOPSO

cout << "...MOPSO::initialize_arrays" << endl << flush; // Alex

  lastPart = 0;
  maxCube = (unsigned int) pow(ndiv, NF);

  hyperPoolL = new unsigned int[MEM];
  hyperPoolF = new double[MEM];
  pbests = new double [M * D];
  vel = new double [M * D];
  fbests = new double [M * NF];
  noDomP = new double [MEM * D];
  noDomF = new double[MEM * NF];
  opt  = new unsigned int[NF];
  
  amp = new double [NF];
  start = new double [NF];
  linf = new unsigned int[NF];
  lsup = new unsigned int[NF];
    
  hyperspace = new unsigned int[maxCube];
  partPos = new unsigned int[MEM];

//  selec= new unsigned int[80/10];
  state=false;

  //Numero de restricciones (revisar)
  rest = 0;
  //Numero de Generaciones (revisar)
  t = 0;
  primeraGeneracion=true;
	
  arraysInitialized=true;
cout << "...MOPSO::prepareNewExecution end" << endl << flush; // Alex

}

void MOPSO::free_arrays()
{
	cout << "...MOPSO::free_arrays" << endl << flush; // Alex
	if (arraysInitialized)
	{
	  delete [] pbests;
	  delete [] vel;
	  delete [] fbests;
	  delete [] noDomP;
	  delete [] noDomF;
	  delete [] opt;

	  delete [] amp;
	  delete [] start;
	  delete [] linf;
	  delete [] lsup;
	  delete [] hyperspace;
	  delete [] partPos;
	  delete [] hyperPoolL;
	  delete [] hyperPoolF;
//	  delete [] selec;
	  arraysInitialized=false;
	}
	else
	{
		cout << "...MOPSO::free_arrays Invalid call, arrays not initialized!!!" << endl << flush; // Alex
	}

}

void MOPSO::copy_fbests () {
// cout << "...MOPSO::copy_fbests" << endl << flush; // Alex
  unsigned int i,j;

  for(i = 0; i < M; i++)
    for (j = 0; j < NF; j++)
	    fbests[i*NF+j]=(*population)[i]->getObj(j);    

// cout << "...MOPSO::copy_fbests last copied is " << i*NF+j << endl << flush; // Alex

}

void MOPSO::copy_pbests () {
// cout << "...MOPSO::copy_pbests" << endl << flush; // Alex
  unsigned int i,j;

  for(i = 0; i < M; i++)
    for (j = 0; j < D; j++)
	    pbests[i*D+j]=(*population)[i]->getVar(j);    

// cout << "...MOPSO::copy_pbests last copied is " << i*D+j << endl << flush; // Alex
	
}

void MOPSO::searchAndInsert(double *noDomP, double *noDomF, unsigned int * opt, unsigned int *lastPart,unsigned int fun) {
// cout << "...MOPSO::searchAndInsert" << endl << flush; // Alex
// cout << "......M=" << M << endl << flush; // Alex
// cout << "......D=" << D << endl << flush; // Alex
// cout << "......NF=" << NF << endl << flush; // Alex
  unsigned int i, j, k;

  for(i = *lastPart, k = 0; k < M; i++, k++) {
// cout << "......Particula " << k << endl << flush; // Alex
    for(j = 0; j < D; j++)
    {
      noDomP[(D * i) + j] = (*population)[k]->getVar(j);
// cout << ".........posicion[" << j << "]=" << (*population)[k]->getVar(j) << endl << flush; // Alex
    }
    for(j = 0; j < NF; j++) {
      noDomF[(NF * i) + j] = (*population)[k]->getObj(j);
// cout << ".........fitness[" << j << "]=" << (*population)[k]->getVar(j) << endl << flush; // Alex
    }
    *lastPart += 1;
  }
	
  //elimina las part?culas dominadas
// cout << "......Eliminando particulas dominadas" << endl << flush; // Alex
  delPartDom(noDomP, noDomF, lastPart, D, NF, opt,fun);
// cout << "......Eliminadas particulas dominadas" << endl << flush; // Alex

}

void MOPSO::delPartDom(double * noDomP, double *noDomF, unsigned int *lastPart, unsigned int D, unsigned int NF, unsigned int * opt, unsigned int fun) {
  unsigned int h, i, j;// k;
  h = 0;
  i = h + 1;
  do {
    do {
      switch (compVec(noDomF, NF, h, i, opt,fun,noDomP,D)) {
      case 1: for(j = 0; j < D; j++)
				noDomP[(D * i) + j] = noDomP[(D * (*lastPart - 1)) + j];
	      for(j = 0; j < NF; j++)
				noDomF[(NF * i) + j] = noDomF[(NF * (*lastPart - 1)) + j];
	      *lastPart -= 1;
      break;
      case 2:	 for(j = 0; j < D; j++)
					noDomP[(D * h) + j] = noDomP[(D * (*lastPart - 1)) + j];
			      for(j = 0; j < NF; j++)
					noDomF[(NF * h) + j] = noDomF[(NF * (*lastPart - 1)) + j];
	  *lastPart -= 1;

      i = h + 1;
      break;
      case 3:	i += 1;
	break;
      case 4:
		  noDomP[(D * i) + j] = noDomP[(D * (*lastPart - 1)) + j];
	      for(j = 0; j < NF; j++)
				noDomF[(NF * i) + j] = noDomF[(NF * (*lastPart - 1)) + j];
	      *lastPart -= 1;
	
		  noDomP[(D * h) + j] = noDomP[(D * (*lastPart - 1)) + j];
	      for(j = 0; j < NF; j++)
				noDomF[(NF * h) + j] = noDomF[(NF * (*lastPart - 1)) + j];
	      *lastPart -= 1;
	
	      i = h + 1;
	
		break;

      }
    } while (i < *lastPart);
    h += 1;
    i = h + 1;
  } while (h < *lastPart - 1);
}


/*
 * Devuelve:
 * 0
 * 1
 * 2
 * 3
 */
 
unsigned int MOPSO::compareVector2(double *noDomF, unsigned int h, unsigned int i, unsigned int * opt, unsigned int fun ,double *noDomP) {
// cout << "...MOPSO::compareVector2" << endl << flush; // Alex
  unsigned int sum = 0, ret, j;

//  if(constraints(noDomP+(D*h),fun) < constraints(pop+(D*i),fun))return 1;
//  if(constraints(noDomP+(D*h),fun) > constraints(pop+(D*i),fun))return 2;

  //sum = sumcompVec(noDomF, NF, h, i, opt);


  for(j = 0; j < NF; j++)
  {
// cout << "...MOPSO::compareVector2 Objetivo " << j << endl << flush; // Alex
// cout << ".............. Valor     " << h << "=" << noDomF[(NF * h) + j] << endl << flush; // Alex
// cout << ".............. Particula " << i << "=" << (*population)[i]->getObj(j) << endl << flush; // Alex

//    if (((noDomF[(NF * h) + j] < fitness[(NF * i) + j]) && (opt == 0)) || ((noDomF[(NF * h) + j] > fitness[(NF * i) + j]) && (opt == 1)))

	    if (((noDomF[(NF * h) + j] < (*population)[i]->getObj(j)) && (opt[j] == 0)) 
	      || ((noDomF[(NF * h) + j] > (*population)[i]->getObj(j)) && (opt[j] == 1)))
	      {
	      sum += 1;
// cout << ".............. " << h << " domina a " << i << endl << flush; // Alex
	      }
	   else
	   {
// cout << ".............. " << h << " NO domina a " << i << endl << flush; // Alex
	   }
  }
  
   	
  if (sum == NF)
    ret = 1;
  else
    if (sum == 0)
      ret = 2;
    else
      ret = 3;

// cout << ".............. Resultado: " << ret << endl << flush; // Alex
			
  return ret;		
}


unsigned int MOPSO::verifyDom(double *noDomP, double *noDomF, unsigned int *hyperspace, unsigned int *partPos, unsigned int * opt, unsigned int *lastPart, unsigned int i,unsigned int fun) {
// cout << "...MOPSO::verifyDom" << endl << flush; // Alex
  unsigned int h = 0, j;

  do {
// cout << "...MOPSO::verifyDom Comprobando Valor " << h << endl << flush; // Alex
    switch (compareVector2(noDomF, h, i, opt,fun,noDomP)) {
      //Si fitness es dominada por un noDom.
    case 1: return 0;
                 		//Si fitness domina a un noDom.
    case 2:	//La part?cula noDom dominada se elimina del repositorio
      for(j = 0; j < D; j++)
		noDomP[(D * h) + j] = noDomP[(D * (*lastPart - 1)) + j];
      for(j = 0; j < NF; j++)
		noDomF[(NF * h) + j] = noDomF[(NF * (*lastPart - 1)) + j];
      //La part?cula se elimina del hyperespacio
// cout << "...MOPSO::verifyDom Valor " << h << " eliminado del frente" << endl << flush; // Alex
      hyperspace[partPos[h]] -= 1;
      partPos[h] = partPos[*lastPart - 1];
      //Se disminute el n?mero de part?culas en el repositorio
      *lastPart -= 1;
      break;
    case 3:	h += 1;
      break;
    case 4:	//La part?cula noDom dominada se elimina del repositorio
      for(j = 0; j < D; j++)
		noDomP[(D * h) + j] = noDomP[(D * (*lastPart - 1)) + j];
      for(j = 0; j < NF; j++)
		noDomF[(NF * h) + j] = noDomF[(NF * (*lastPart - 1)) + j];
      //La part?cula se elimina del hyperespacio
// cout << "...MOPSO::verifyDom Valor " << h << " eliminado del frente (2)" << endl << flush; // Alex
      hyperspace[partPos[h]] -= 1;
      partPos[h] = partPos[*lastPart - 1];
      //Se disminute el n?mero de part?culas en el repositorio
      *lastPart -= 1;
      break;

    }
  } while(h < *lastPart);

  return 1;
}

unsigned int MOPSO::locateHypercube(double *start, double *amp, unsigned int ndiv, unsigned int part) {
// cout << "...MOPSO::locateHypercube" << endl << flush; // Alex
  unsigned int temp, loc = 0;
  unsigned int i = NF - 1, j;

  for(j = 0; j < NF ; j++ , i--) {
    temp = (long int) floor(((*population)[part]->getObj(j) - start[j]) / amp[j]);
    if (temp > ndiv || temp < 0) return 0;
    loc += (long int) floor(temp * pow(ndiv, i));
  }
  return loc + 1;
}
 
/* 
 * Alex: devuelve 1 si la particula es seleccionada para insercion
 */
int MOPSO::searchAndInsert2(double *noDomP, double *noDomF, unsigned int *linf, unsigned int *lsup, double *amp, double *start, unsigned int *hyperspace, unsigned int *partPos, unsigned int ndiv, unsigned int * opt, unsigned int maxCube, unsigned int *lastPart, unsigned int fun) {
// cout << "...MOPSO::searchAndInsert2" << endl << flush; // Alex
  unsigned int i, j, k, l, aux_1, aux_2;

  int retcode = 0;
  
  for(k = 0; k < M; k++) {
    //Se verifica que la particula candidata a ingresar al repositorio NO sea dominada por las dem?s
    //y de paso elimina a las que logre dominar (actualizando el hypercubo).
    //        if(constraints(noDomP+(NF*k),fun)>0)continue;
	

    if (verifyDom(noDomP, noDomF, hyperspace, partPos, opt, lastPart, k,fun) == 1) {
      retcode = 1;
      //Si el repositorio a?n no se ha llenado, ingresa la part?cula y se actualiza el hypercubo
      if (*lastPart < MEM) {
		i = *lastPart;
		for(j = 0; j < D; j++)
		  noDomP[(D * i) + j] = (*population)[k]->getVar(j);
		for(j = 0; j < NF; j++)
		  noDomF[(NF * i) + j] = (*population)[k]->getObj(j);
		*lastPart += 1;
		partPos[i] = locHyper(noDomF, start, amp, ndiv, NF, i);
		if (partPos[i] != 0) {
		  partPos[i] -= 1;
		  hyperspace[partPos[i]] += 1;
		}
		else
		  makeHyper(noDomF, linf, lsup, amp, start, hyperspace, partPos, ndiv, MEM, NF, *lastPart, maxCube);

      } 
      //En caso de que la part?cula no haya sido dominada, para ingresar en el repositorio debe de
      //encontrarse en un hypercubo no muy poblado.
      else {
		aux_1 = locateHypercube(start, amp, ndiv, k);
		aux_2 = search_max(hyperspace, maxCube);
		//Si la part?cula esta fuera del rango se recalcula el hyperespacio eliminando
		//una part?cula del ?rea m?s poblada
		if (aux_1 == 0) {
		  for(l = 0; l < *lastPart; l++)
		    if (partPos[l] == aux_2)
		      break;
		  for(j = 0; j < D; j++)
		    noDomP[(D * l) + j] = (*population)[k]->getVar(j);
		  for(j = 0; j < NF; j++)
		    noDomF[(NF * l) + j] = (*population)[k]->getObj(j);
		  makeHyper(noDomF, linf, lsup, amp, start, hyperspace, partPos, ndiv, MEM, NF, *lastPart, maxCube);

		}
				//Sino se busca eliminar a una part?cula del ?rea m?s poblada
	else {
	  	  aux_1 -= 1;
	  //	  if(aux_2<1){cout<<"cotzito"<<endl;exit(0);}

          			//Si el ?rea de la part?cula candidata es menos poblada que el ?rea m?s poblada en el hyperespacio
          			//se inserta la part?cula reemplazandola por una del ?rea m?s poblada.
          			//Sino se descarta.
		  if (hyperspace[aux_1] < hyperspace[aux_2]) {
		    for(l = 0; l < *lastPart; l++)
		      if (partPos[l] == aux_2)
			break;
		    for(j = 0; j < D; j++)
		      noDomP[(D * l) + j] = (*population)[k]->getVar(j);
		    for(j = 0; j < NF; j++)
		      noDomF[(NF * l) + j] = (*population)[k]->getObj(j);
		    /*
		    if(partPos[l]-aux_1<0)cout<<"saludos"<<endl;
		    if(aux_2<0)cout<<"saludos"<<endl;
		    if(l<0||l>=MEM)cout<<"saludos"<<endl;
		    if(partPos[l]>=maxCube)cout<<"saludos"<<endl;
		    if(hyperspace[aux_2]<0)cout<<"saludos"<<endl;
		    cout<<partPos[l]<<" "<<aux_1<<" "<<maxCube<<endl;
		    */
		    partPos[l] = aux_1;
		    hyperspace[partPos[l]] += 1;
		    hyperspace[aux_2] -= 1;
	
		  /*GTP
		    // GTP*/
		  }
		}

      }

    } // IF
  } // FOR
  return retcode;
}

unsigned int MOPSO::bestMoo(unsigned int part, unsigned int * opt,unsigned int fun) {
// cout << "...MOPSO::bestMoo" << endl << flush; // Alex
  unsigned int i, sum, ret;
	
  //  if(constraints(pop,fun) < constraints(fbests,fun))return 0;
  //if(constraints(pop,fun) > constraints(fbests,fun))return 1;
  //  cout<<"                        holas"<<endl;
  for(i = 0; i < NF; i++)
/*
    if ((((*population)[part]->getObj(i) < fbests[M*part+i]) && (opt == 0)) 
    || (((*population)[part]->getObj(i) > fbests[M*part+i]) && (opt == 1)))
      sum += 1;
*/
    if ((((*population)[part]->getObj(i) < fbests[NF*part+i]) && (opt[i] == 0)) 
    || (((*population)[part]->getObj(i) > fbests[NF*part+i]) && (opt[i] == 1)))
      sum += 1;
	
  if (sum == NF)
    ret = 0;
  else
    if (sum == 0)
      ret = 1;
    else
      ret = RandomInt(0, 1);
		
  return ret;
}

/* Alex
 * Devuelve 1 si hizo intercambio
 */
int MOPSO::ifbestInterchange(double *fbests, double *pbests,unsigned int * opt,unsigned int fun) {
// cout << "...MOPSO::ifbestInterchange" << endl << flush; // Alex
  unsigned int i, j;
  int retcode = 0;
  
  for(i = 0; i < M; i++) {
//    if (best_moo(&fitness[NF * i], &fbests[NF * i], NF, opt,fun,pop+(NF*i))) {
    if (bestMoo(i, opt,fun)) 
    {
      for(j = 0; j < NF; j++)
		fbests[(NF * i) + j] = (*population)[i]->getObj(j);
      for(j = 0; j < D; j++)
      	pbests[(D * i) + j] = (*population)[i]->getVar(j);
      retcode = 1;
    }
  }
  return retcode;
}


unsigned int MOPSO::lead(unsigned int *hyperspace, unsigned int *hyperPoolL, unsigned int *partPos, double *hyperPoolF, double hypsum, unsigned int lastPart, unsigned int poolC) {
  unsigned int h, i, rand;

  h = hyperWheel(hyperPoolL, hyperPoolF, hypsum, poolC);

// cout << "...MOPSO::lead:tras hyperWheel, h=" << h << endl << flush;

  rand = RandomInt(1, hyperspace[h]);

// cout << "...MOPSO::lead:tras RandomInt()" << endl << flush;

  //for(i = 0; i < 900;i++){
  //if(hyperspace[i]>0) cout<<hyperspace[h]<<endl;
  //}
  //  cout<<"lastPart "<<lastPart<<endl;
  //for(i=0;i<lastPart;i++)
  //cout<<partPos[i]<<endl;
  //exit(0);

  for(i = 0; i < lastPart; i++) {
    if (partPos[i] == h)
      rand -= 1;
    if (rand < 1)
      break;
  }

// cout << "...MOPSO::lead:tras seleccionar particula" << endl << flush;
	
  return i;
}

void MOPSO::makeHyper(double *noDomF, unsigned int *linf, unsigned int *lsup, double *amp, double *start, unsigned int *hyperspace, unsigned int *partPos, unsigned int ndiv, unsigned int MEM, unsigned int NF, unsigned int lastPart, unsigned int maxCube) {
  double *aux;
  unsigned int i, j;
  aux = new double [MEM];

  for(j = 0; j < NF; j++) {
    for(i = 0; i < lastPart; i++)
    {
      aux[i] = noDomF[(NF * i) + j];
// cout << "...MOPSO::makeHyper aux[" << i << "]=" << aux[i] << endl << flush;
    }
    linf[j] = search_min(aux, i);
    lsup[j] = search_max(aux, i);
// cout << "...MOPSO::makeHyper linf[" << j << "]=" << linf[j] << endl << flush;
// cout << "...MOPSO::makeHyper lsup[" << j << "]=" << lsup[j] << endl << flush;
  }

  for(i = 0; i < NF; i++) {
    amp[i] = (noDomF[(NF * lsup[i]) + i] - noDomF[(NF * linf[i]) + i]) / (double)(ndiv - 1);
    start[i] = noDomF[(NF * linf[i]) + i] - (amp[i] / 2.0);
  }

  for(i = 0; i < maxCube; i++)
    hyperspace[i] = 0;

// cout << "...MOPSO::makeHyper calculating number of particles per division" << endl << flush;

  for(i = 0; i < lastPart; i++) {
    partPos[i] = locHyper(noDomF, start, amp, ndiv, NF, i) - 1;
// cout << "...... particle " << i << " in hypercube " << partPos[i] << endl << flush;
    hyperspace[partPos[i]] += 1;
  }
	
  delete [] aux;

}

void MOPSO::hyperFit(unsigned int *hyperspace, unsigned int *hyperPoolL, double *hyperPoolF, double *hypsum, unsigned int maxCube, unsigned int *poolC) {
  unsigned int i;
  *poolC = 0;
  *hypsum = 0;
	
  for(i = 0; i < maxCube; i++)
    if (hyperspace[i] > 0) {
      hyperPoolF[*poolC] = 10.0 / hyperspace[i];
      hyperPoolL[*poolC] = i;
      *hypsum += hyperPoolF[*poolC];
      *poolC += 1;
    }
}

unsigned int MOPSO::hyperWheel(unsigned int *hyperPoolL, double *hyperPoolF, double hypsum, unsigned int poolC) {
  unsigned int i = 0;
  double rand, sum = 0;

// cout << "...MOPSO::hyperWheel hypsum=" << hypsum << ",poolC=" << poolC << endl << flush;

  rand = RandomDouble(0.0, 1.0) * hypsum;
  do {
    sum = sum + hyperPoolF[i];
    i++;
  } while ((i < poolC) && (sum < rand));
  return hyperPoolL[i - 1];
}


//Calcula la velocidad de las particulas
double MOPSO::velocity(double W, double Vi, double C1, double C2, double Pb, double Pg, double Xi) {
  return W * Vi + C1 * RandomDouble(0.0, 1.0) * (Pb - Xi)
    + C2 * RandomDouble(0.0, 1.0) * (Pg - Xi);
}

// Compara vectores
unsigned int MOPSO::compVec(double *noDomF, unsigned int NF, unsigned int h, unsigned int i, unsigned int * opt,unsigned int fun,double *noDomP,unsigned int D) {
  unsigned int sum = 0, ret, j;

/*	
  //sum = sumcompVec(noDomF, NF, h, i, opt);
    if(constraints(noDomF+(NF*h),fun)>0&&constraints(noDomF+(NF*i),fun)>0){
   return 4;
   }
  else if(constraints(noDomF+(NF*h),fun)>0)return 2;
  else if(constraints(noDomF+(NF*i),fun)>0)return 1;
  //if(constraints(noDomP+(D*h),fun) > constraints(pop+(D*i),fun))return 1;
  //if(constraints(noDomP+(D*h),fun) < constraints(pop+(D*i),fun))return 2;

  //  cout<<"                                          rakam"<<endl;
*/

  for(j = 0; j < NF; j++)
    if (((noDomF[(NF * h) + j] < noDomF[(NF * i) + j]) && (opt[j] == 0)) || 
        ((noDomF[(NF * h) + j] > noDomF[(NF * i) + j]) && (opt[j] == 1)))
      sum += 1;
	
  if (sum == NF)
    ret = 1;
  else
    if (sum == 0)
      ret = 2;
    else
      ret = 3;
			
  return ret;		
}

unsigned int MOPSO::locHyper(double *vect, double *start, double *amp, unsigned int ndiv, unsigned int NF, unsigned int part) {
  unsigned int temp, loc = 0;
  unsigned int i = NF - 1, j;

  for(j = 0; j < NF ; j++ , i--) {
    temp = (long int) floor((vect[(NF * part) + j] - start[j]) / amp[j]);
    if (temp > ndiv || temp < 0) return 0;
    loc += (long int) floor(temp * pow(ndiv, i));
  }
  return loc + 1;
}

//Busca el valor maximo de un arreglo de tipo entero sin signo
unsigned int MOPSO::search_max(unsigned int *f, unsigned int M) {
  unsigned int i, fmax = 0;

  for(i = 1; i < M; i++)
    if (f[fmax] < f[i])
      fmax = i;
	
  return fmax;
}

//Busca el valor maximo de un arreglo de tipo double
unsigned int MOPSO::search_max(double *f, unsigned int M) {
  unsigned int i, fmax = 0;

  for(i = 1; i < M; i++)
    if (f[fmax] < f[i])
      fmax = i;
	
  return fmax;
}

//Busca el valor minimo de un arreglo de tipo double
unsigned int MOPSO::search_min(double *f, unsigned int M) {
  unsigned int i, fmin = 0;

  for(i = 1; i < M; i++)
    if (f[fmin] > f[i])
      fmin = i;
	
  return fmin;
}

void MOPSO::muta(double pM, int M, unsigned int D,int Gen,int totGen,int fun){
// cout << "...MOPSO::muta" << endl << flush; // Alex
  unsigned int i,j;
  int dimension=0;
  double varInf[D],varSup[D],rango,linftemp,lsuptemp;
  double valtemp=0;
  ranges(fun,D,varInf,varSup);
  for(i = 0;i < M;i++){
    if(flip(pow(1.0-(double)Gen/(totGen*pM),1.5))){
      dimension=RandomInt(0,D-1);
      rango=(varSup[dimension]-varInf[dimension])*pow(1.0-(double)Gen/(totGen*pM),1.5)/2;//totGen
      valtemp=RandomDouble(rango,-rango);

//      IndividualZDT1MOPSO * ind = (IndividualZDT1MOPSO *) (*population)[i];     
//      ind->mutation(dimension,rango,linf[dimension],lsup[dimension]);

		if((*population)[i]->getVar(dimension)-rango<varInf[dimension]) linftemp=varInf[dimension]; 
		else linftemp=(*population)[i]->getVar(dimension)-rango;
		if((*population)[i]->getVar(dimension)+rango>varSup[dimension]) lsuptemp=varSup[dimension]; 
		else lsuptemp=(*population)[i]->getVar(dimension);
		
		(*population)[i]->setVar(dimension,RandomDouble(linftemp,lsuptemp));//mutation_operator(pop[ngen+(D*i)],Gen);

    }
  }
}


void MOPSO::ranges(unsigned int fun,int D,double *linf,double * lsup)
{

	unsigned int i;
	const Individual *anInd = getSampleInd();
	
    for(i = 0; i < D; i++) 
    {
		linf[i] = anInd->getMinimum(i);
    	lsup[i] = anInd->getMaximum(i);
  	}
}
