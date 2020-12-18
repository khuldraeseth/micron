#pragma once

#include <ostream>
#include <memory>

#include <Data/List.hpp>


template <>
std::ostream& operator<<(std::ostream& ost, std::shared_ptr<Node<char>> n) {
    if (n == nullptr) {
        return ost;
    }

    return ost << n->mVal << n->mNext;
}

using String = List<char>;

template <>
std::ostream& operator<<(std::ostream& ost, String xs) {
    return ost << '"' << xs.mHead << '"';
}

String operator""_s(char const* s, unsigned long n) {
    if (n == 0) {
        return nil<char>;
    }

    return *s |= operator""_s(s+1, n-1);
}
