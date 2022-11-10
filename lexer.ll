%option nodefault
%option noyywrap
%option yyclass="Scanner"
%option c++
%{
    #include "ast.tab.hh"
    #include "driver.hh"
    #include "./scanner.hh"
    yy::parser::symbol_type make_NUM(const std::string& text);
    yy::parser::symbol_type make_ID(const std::string& text);
    yy::parser::symbol_type yylex();
%}


id [a-zA-Z_]+[a-zA-Z_0-9]*
num (([0-9]+"."[0-9]+)|0|[1-9][0-9]*)
blank [ \t\r]

%%
"("         {return yy::parser::make_LEFT_PARA();}
")"         {return yy::parser::make_RIGHT_PARA();}
"{"         {return yy::parser::make_LEFT_BRACK();}
"}"         {return yy::parser::make_RIGHT_BRACK();}
"def"       {return yy::parser::make_DEF();}
"extern"    {return yy::parser::make_EXTERN();}
"if"        {return yy::parser::make_IF();}
"then"      {return yy::parser::make_THEN();}
"else"      {return yy::parser::make_ELSE();}
","         {return yy::parser::make_COMMA();}
"<"         {return yy::parser::make_LESS('<');}
"=="        {return yy::parser::make_EQUAL('E');}
"!="        {return yy::parser::make_UNEQUAL('U');}
"="         {return yy::parser::make_ASSIGN();}
"in"        {return yy::parser::make_IN();}
"and"       {return yy::parser::make_AND('&');}
"or"        {return yy::parser::make_OR('|');}
"for"       {return yy::parser::make_FOR();}
"*"         {return yy::parser::make_TIME('*');}
"/"         {return yy::parser::make_DIV('/');}
"+"                         {return yy::parser::make_ADD('+');}
"-"                         {return yy::parser::make_SUB('-');}
{num}             {return make_NUM(yytext);}
{id}     {return make_ID(yytext);}
{blank}+         {}
(\n)+        {}
.            {throw yy::parser::syntax_error("invalid character: "+std::string(yytext) );}
<<EOF>>      {return yy::parser::make_YYEOF();}
%%

yy::parser::symbol_type make_NUM(const std::string& text)
{
    double num = atof(text.c_str());
    return yy::parser::make_NUM(num);
}
yy::parser::symbol_type make_ID(const std::string& text)
{
    std::unique_ptr<std::string> id(new std::string(text));
    return yy::parser::make_ID(std::move(id));
}
void yyerror(char* msg)
{
    printf("%s\n", msg);
    exit(1);
}
