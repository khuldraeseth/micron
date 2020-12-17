#include <iostream>

#include "Either.hpp"
#include "List.hpp"
#include "Micron.hpp"

int plus(char x, char y) {
    return (x - '0') + (y - '0');
}

int main() {
    String foo = "47254 824673r"_s;

    std::cout << runMicron(then(then(many(digit), spaces), many(digit)), foo) << std::endl;
}
