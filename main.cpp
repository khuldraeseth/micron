#include <iostream>

#include <Data/String.hpp>
#include <Micron/Micron.hpp>


int main() {
    String foo = "47254 824673r"_s;

    std::cout << runMicron(then(then(many(digit), spaces), many(digit)), foo) << std::endl;
}
