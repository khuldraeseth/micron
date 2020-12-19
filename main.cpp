#include <iostream>

#include <Data/String.hpp>
#include <Micron/Micron.hpp>


Micron<char> const repeated = bind(digit, oneChar);

int main() {
    String foo = "447722cc54 824673r"_s;

    std::cout << runMicron(many(orElse(repeated, letter)), foo) << std::endl;
}
