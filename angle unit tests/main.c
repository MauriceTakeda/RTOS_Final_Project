#include <stdio.h>

#define CTEST_MAIN

#define CTEST_SEGFAULT
#define CTEST_COLOR_OK

#include "ctest.h"

//----------------------------------------------------------------------------------------------------------------------------------
/// @Makefile
/// 1. type 'make' in command line for the project to be built
/// 2. type 'make remake' to rebuild your project
/// 3. type './angletask' to run your unit tests
//----------------------------------------------------------------------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    int result = ctest_main(argc, argv);

    printf("\nRan all of the tests associated with angle task\n");
    return result;
}

