%{

#include <string>
#include "parseLine.tab.h"
#include "parseDefinitions.h"

using namespace std;

#include <iostream>
using namespace std;

int linelex();

%}

%%

%[a-z0-9]*                       { stringValue = &yytext[1]; return VARIABLE; }
[^%]*                            { stringValue = yytext; return STRING; }
%%

int yywrap() { return 1; }
