
/*******************************************************************
 * AUTHORS
 *   Carlos Segura
 *
 * DESCRIPTION
 *   Plugin Base Class: the different kinds of plugins must inherit
 *   from the class.
 * *****************************************************************/

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include <string>
#include <vector>

using namespace std;

class Plugin {
	public:
		string name;
		virtual bool init(const vector<string> &params);
		virtual string getName() const { return name; }
		virtual void setName(const string &n) { name = n; }
		virtual string getParameterName(int i) const { return ""; }
		virtual string getParameterValue(int i) const { return ""; }
		virtual int getNumberOfParameters() const { return 0; }
		virtual ~Plugin() {}
};

Plugin *loadPlugin(const string &pluginKind, const string &pluginPath, const string &name, const vector<string> &scoreArgs, bool printError, bool initialize);

#endif
