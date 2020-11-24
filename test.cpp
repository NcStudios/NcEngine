#include <iostream>

struct Base
{
    int baseInt = 0;
};

struct Child : public Base
{
    int childInt = 1;
};

void Foo(Base in)
{
    std::cout << "sizeof in Foo: " << sizeof(in) << '\n';
}

int main()
{
    Base base;
    Child child;

    std::cout << "sizeof base: " << sizeof(base) << '\n';
    std::cout << "sizeof child: " << sizeof(child) << '\n';

    Foo(base);
    Foo(child);

    return 0;
}