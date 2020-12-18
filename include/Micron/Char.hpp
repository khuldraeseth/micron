#pragma once

#include <cctype>

#include <Data/String.hpp>
#include <Data/Unit.hpp>

#include <Micron/Combinator.hpp>
#include <Micron/Prim.hpp>


//oneOf, noneOf

auto const satisfy = [](auto f) {
    return Micron<char> {[f](String s) {
        if (null(s)) {
            return std::make_pair(s, Left<Error,char>("unexpected eof"_s));
        }

        if (!f(head(s))) {
            return std::make_pair(s, Left<Error,char>("unsatisfied"_s));
        }

        return std::make_pair(tail(s), Right<Error,char>(head(s)));
    }};
};

Micron<Unit> const space = constMap(Unit::Unit, satisfy([](char c) { return std::isspace(c); }));
Micron<Unit> const spaces = skipMany(space);

auto const oneChar = [](char c) {
    return satisfy([c](char x) { return x == c; });
};

Micron<char> newline = oneChar('\n');
Micron<char> crlf = then(oneChar('\r'), newline);
Micron<char> endOfLine = orElse(newline, crlf);

Micron<char> tab = oneChar('\t');

Micron<char> upper = satisfy([](auto c) { return std::isupper(c); });
Micron<char> lower = satisfy([](auto c) { return std::islower(c); });
Micron<char> alphaNum = satisfy([](auto c) { return std::isalnum(c); });
Micron<char> letter = satisfy([](auto c) { return std::isalpha(c); });
Micron<char> digit = satisfy([](auto c) { return std::isdigit(c); });
Micron<char> hexDigit = satisfy([](auto c) { return std::isxdigit(c); });
Micron<char> octDigit = satisfy([](auto c) { return '0' <= c && c <= '7'; });

Micron<char> anyChar = satisfy([](auto _) { return true; });

auto string(String s) -> Micron<String> {
    if (null(s)) {
        return pure(""_s);
    }

    return liftA2(cons, oneChar(head(s)), string(tail(s)));
}

Micron<Unit> const eof = notFollowedBy(anyChar);
