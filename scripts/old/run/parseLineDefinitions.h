#ifndef PARSE_LINE_DEFINITIONS_H
#define PARSE_LINE_DEFINITIONS_H

#include <map>
#include <string>
#include <sstream>
#include <string.h>

using namespace std;

class LessThan_casecmp : public std::binary_function<const string, const string, bool> {
public:
    inline bool operator () (const string str1, const string str2) const {
        return strcasecmp (str1.c_str(), str2.c_str()) < 0;
    }
};

void set_variable(const string &key, const int &value);
void set_variable(const string &key, const double &value);
void set_variable(const string &key, const string &value);
string get_variable(const string &key);
void line_parser(const char *line, const char *error);

extern map < string, string, LessThan_casecmp> variables;
extern stringstream line_parsed;
struct yy_buffer_state * Line_scan_string(const char *);
void Line_delete_buffer(struct yy_buffer_state *);
int Lineparse();


#endif
