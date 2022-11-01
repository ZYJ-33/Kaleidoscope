#ifndef _SCANNER
#define _SCANNER

#include"ast.tab.hh"
#if ! defined(yyFlexLexerOnce)
#undef yyFlexLexer
#include<FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL yy::parser::symbol_type Scanner::get_next_token()

#include<fstream>

class Driver;
class Scanner: public yyFlexLexer
{
    public:
            Driver* drv;
            std::ifstream input;
    public:
            Scanner(Driver* drv, const std::string& file):drv(drv)
            {
                    input.open(file, std::ios::in);
                    switch_streams(&input, 0);
            }
            virtual ~Scanner(){}
            virtual yy::parser::symbol_type get_next_token();
};

#endif
