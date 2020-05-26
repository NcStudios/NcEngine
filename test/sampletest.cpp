#include "framework/nctest.h"


int main()
{
    using namespace nc::test;

    NC_TEST testSuite;

    testSuite("sample test 1", []() { return true; } );
    testSuite("sample test 2", []() { return false; });

    testSuite.DisplayResults();

    return 0;
}