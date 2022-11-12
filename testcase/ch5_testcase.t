extern print_double(d)

def main()
    fib(4)

def fib(x)
    if (x==0 or x==1) then
    {
        1
    }
    else
    {
        fib(x-1) + fib(x-2)
    }
