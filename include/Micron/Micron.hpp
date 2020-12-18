#pragma once

#include <cctype>
#include <functional>
#include <type_traits>
#include <utility>

#include <Data/Either.hpp>
#include <Data/List.hpp>
#include <Data/String.hpp>
#include <Data/Unit.hpp>


using Error = String;

template <typename A>
struct Micron {
    using Fn = std::function<std::pair<String,Either<Error,A>>(String)>;

    Fn fn {};
};

auto const runMicron = [](auto ma, auto s) {
    return ma.fn(s).second;
};

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

Micron<char> const anyChar = satisfy([](char _) { return true; });

Micron<char> const digit = satisfy([](char c) { return std::isdigit(c); });
Micron<char> const alpha = satisfy([](char c) { return std::isalpha(c); });

auto const notFollowedBy = [](auto ma) -> Micron<Unit> {
    return {[ma](String s) {
        auto const [s1, r1] = ma.fn(s);
        if (isLeft(r1)) {
            return std::make_pair(s, Right<Error,Unit>(Unit::Unit));
        }

        return std::make_pair(s, Left<Error,Unit>("notFollowedBy not satisfied"_s));
    }};
};

Micron<Unit> const eof = notFollowedBy(anyChar);

template <typename A>
auto const fail = [](Error e) -> Micron<A> {
    return Micron<A> {[e](String s) {
        return std::make_pair(s, Left<Error,A>(e));
    }};
};

auto const fmap = []<typename Fn, typename A, typename B = std::invoke_result_t<Fn,A>>(Fn f, Micron<A> ma) -> Micron<B> {
    return {[f,ma](String s) {
        auto const [s1, r1] = ma.fn(s);
        if (isLeft(r1)) {
            return std::make_pair(s1, Left<Error,B>(fromLeft(r1)));
        }

        return std::make_pair(s1, Right<Error,B>(f(fromRight(r1))));
    }};
};

auto const pure = []<typename A>(auto a) -> Micron<A> {
    return {[a](String s) {
        return std::make_pair(s, Right<Error,A>(a));
    }};
};

auto const ap = []<typename B>(auto mf, auto ma) -> Micron<B> {
    return {[mf,ma](String s) {
        auto const [s1, r1] = mf.fn(s);
        if (isLeft(r1)) {
            return std::make_pair(s1, Left<Error,B>(fromLeft(r1)));
        }

        auto const [s2, r2] = ma.fn(s1);
        if (isLeft(r2)) {
            return std::make_pair(s2, Left<Error,B>(fromLeft(r2)));
        }

        return std::make_pair(s2, Right<Error,B>(fromRight(r1)(fromRight(r2))));
    }};
};

auto curry = [](auto const& f) {
    return [f](auto const& x) {
        return [f,x](auto const& y) {
            return f(x, y);
        };
    };
};

auto const liftA2 = [](auto f, auto ma, auto mb) {
    return ap(fmap(curry(f), ma), mb);
};

template <typename B>
auto const bind = [](auto ma, auto f) -> Micron<B> {
    return {[ma,f](String s) {
        auto const [s1, r1] = ma.fn(s);
        if (isLeft(r1)) {
            return std::make_pair(s, Left<Error,B>(fromLeft(r1)));
        }

        return f(fromRight(r1)).fn(s1);
    }};
};

template <typename A> Micron<A> const empty = fail<A>("empty"_s);

auto const orElse = []<typename A>(Micron<A> m1, Micron<A> m2) -> Micron<A> {
    return {[m1,m2](String s) {
        auto const [s1, r1] = m1.fn(s);
        if (isRight(r1)) {
            return std::make_pair(s1, r1);
        }

        return m2.fn(s);
    }};
};

template <typename A>
auto many0(Micron<A> ma) -> Micron<List<A>> {
    return {[ma](String s) {
        auto const [s1, r1] = ma.fn(s);
        if (isLeft(r1)) {
            return std::make_pair(s, Right<Error,List<A>>(nil<A>));
        }

        return fmap(curry(cons)(fromRight(r1)), many0(ma)).fn(s1);
    }};
}

auto const many = []<typename A>(Micron<A> ma) -> Micron<List<A>> {
    if (isRight(ma.fn(""_s).second)) {
        return fail<List<A>>("combinator `many` is applied to a parser that accepts an empty string"_s);
    }

    return many0(ma);
};

auto const some = [](auto ma) {
    return liftA2(cons, ma, many(ma));
};

auto const many1 = some;

auto const constMap = [](auto b, auto ma) {
    return fmap([b](auto _) { return b; }, ma);
};

auto const then = [](auto ma, auto mb) {
    return liftA2([](auto a, auto b) { return b; }, ma, mb);
};

auto const before = [](auto ma, auto mb) {
    return liftA2([](auto a, auto b) { return a; }, ma, mb);
};

auto const skipMany = [](auto ma) -> Micron<Unit> {
    return constMap(Unit::Unit, many(ma));
};

Micron<Unit> const spaces = skipMany(satisfy([](char c) { return std::isspace(c); }));
