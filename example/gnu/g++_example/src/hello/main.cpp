#include <iostream>
#include <test_static.hpp>
#include <test_shared.hpp>


int main(int argc, char** argv) {
    std::cout << "Hello, World" << std::endl;
    std::cout << getTestStaticString() << std::endl;
    std::cout << getTestSharedString() << std::endl;
    return 0;
}