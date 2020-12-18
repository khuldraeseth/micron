#pragma once

#include <functional>
#include <utility>

#include <Data/Either.hpp>
#include <Data/String.hpp>

#include <Micron/Error.hpp>


template <typename A>
struct Micron {
    using Fn = std::function<std::pair<String,Either<Error,A>>(String)>;

    Fn fn {};
};

auto const runMicron = [](auto ma, auto s) {
    return ma.fn(s).second;
};
