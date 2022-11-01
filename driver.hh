#ifndef DRIVER_HH
#define DRIVER_HH

#include "ast.tab.hh"
#include "scanner.hh"
#include <memory>

class Driver
{
    public: 
            std::string file;
            std::unique_ptr<std::vector<std::unique_ptr<FuncAST>>> funcs;
            Scanner scanner;
            yy::parser parser;
    public:
            Driver(const std::string& file) : file(file), scanner(this, file), parser(*this, scanner)
            {
                
            }
            
            void parse()
            {
                parser.parse();
            }
};
#endif
