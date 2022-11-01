#include "driver.hh"

int main(int argc, char** argv)
{
    Driver d(argv[1]);
    d.parse();
    return 0;
}

