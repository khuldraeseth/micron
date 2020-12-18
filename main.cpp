#include <iostream>

#include <Data/String.hpp>
#include <Micron/Micron.hpp>


auto const oneChar = [](char c) -> Micron<char> {
    return satisfy([c](char x) { return x == c; });
};

Micron<char> repeated = bind<char>(digit, oneChar);

int main() {
    String foo = "447722cc54 824673r"_s;

    std::cout << runMicron(many(orElse(repeated, alpha)), foo) << std::endl;
}
