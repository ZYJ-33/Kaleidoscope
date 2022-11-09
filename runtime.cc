#include<cstdio>

extern "C" double print_double(double x)
{
    fputc(x, stdout);
    return x;
}
