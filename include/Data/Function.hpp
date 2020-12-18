#pragma once


auto const curry = [](auto const& f) {
    return [f](auto const& x) {
        return [f,x](auto const& y) {
            return f(x, y);
        };
    };
};
