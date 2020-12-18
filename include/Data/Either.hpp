#pragma once

#include <ostream>
#include <utility>
#include <variant>

template <typename A, typename B>
using Either = std::variant<A,B>;

template <typename A, typename B>
Either<A,B> Left(A a) {
    return Either<A,B>(std::in_place_index<0>, a);
}

template <typename A, typename B>
Either<A,B> Right(B b) {
    return Either<A,B>(std::in_place_index<1>, b);
}

template <typename A, typename B>
bool isLeft(Either<A,B> e) {
    return e.index() == 0;
}

template <typename A, typename B>
bool isRight(Either<A,B> e) {
    return e.index() == 1;
}

template <typename A, typename B>
A fromLeft(Either<A,B> e) {
    return std::get<0>(e);
}

template <typename A, typename B>
B fromRight(Either<A,B> e) {
    return std::get<1>(e);
}

template <typename A, typename B>
std::ostream& operator<<(std::ostream& ost, Either<A,B> e) {
    if (isLeft(e)) {
        return ost << "Left (" << fromLeft(e) << ')';
    }

    return ost << "Right (" << fromRight(e) << ')';
}
