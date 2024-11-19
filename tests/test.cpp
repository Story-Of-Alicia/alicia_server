#include <iostream>
#include <thread>

int main() {
    std::jthread t([] { std::cout << "Hello, jthread!" << std::endl; });
    return 0;
}