#include "driver.hh"
#include "./llvm_driver.h"

extern "C" double print_double(double x);

int main(int argc, char** argv)
{
    Driver d(argv[1]);
    d.parse();
    LLVM_Driver lld(std::move(d.decls), std::move(d.funcs));
    return 0;
}

