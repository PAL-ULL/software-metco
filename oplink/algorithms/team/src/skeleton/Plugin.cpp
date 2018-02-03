/*******************************************************************
 * AUTHORS
 *   Carlos Segura
 ******************************************************************/

#include "Plugin.h"

#include <iostream>
#include <sstream>
#include <dlfcn.h>

//Es correcto no definir init, cuando no se aceptan parametros
bool Plugin::init(const vector<string> &params){
	return (params.size() == 0);
}

//Cargador de plugins
Plugin *loadPlugin(const string &kind, const string &pluginPath, const string &name, const vector<string> &scoreArgs, bool printError, bool initialize){
	void *hndl;
	stringstream libFich;
	if ((name[0] == '/') || (pluginPath.size() == 0)){//Nombre absoluto
		libFich << name << ".so";
	} else {//Nombre relativo
		libFich << pluginPath << "/" << name << ".so";	
	}
	string file = libFich.str();
	hndl = dlopen(file.c_str(), RTLD_NOW);
	if(hndl == NULL){
		if (printError){
			char *error;
			if ((error = dlerror()) != NULL)  {
		  	cerr << error << endl;	
			} 
			cerr << "Plugin " << kind << "::" << name << " file could not be opened: " << libFich.str() << endl;
		}
		return NULL;
	}
	Plugin* (*mkr)() = (Plugin* (*)())dlsym(hndl, "maker");
	if (mkr == NULL){
		if (printError)
			cerr << "maker function in " << kind << "::" << name << "does no exist"<< endl;
		return NULL;
	}
	Plugin *newSc = (*mkr)();
	if (newSc == NULL){
		if (printError)
			cerr << "maker function for " << kind << "::" << name << " returns NULL" << endl;
		return NULL;
	}
	if (initialize){
		if (!newSc->init(scoreArgs)){
			if (printError)
				cerr << "Plugin \"" << kind << "::" << name << "\" initialization failed" << endl;
			return NULL;
		}
	}
	newSc->setName(name);
	return newSc;
}
