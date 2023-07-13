#include <stdio.h>
#include <stdlib.h>
#include <test_static.h>
#include <test_shared.h>


int main(int argc, char** argv) {
    printf("Hello, World\n");
    printf("%s\n", getTestStaticString());
    printf("%s\n", getTestSharedString());
    return 0;
}