#pragma once

#include <type_traits>
#include <utility>

#include <Data/Either.hpp>
#include <Data/Function.hpp>
#include <Data/List.hpp>
#include <Data/String.hpp>
#include <Data/Unit.hpp>

#include <Micron/Error.hpp>
#include <Micron/Prim.hpp>


auto const notFollowedBy = [](auto ma) -> Micron<Unit> {
    return {[ma](String s) {
        auto const [s1, r1] = ma.fn(s);
        if (isLeft(r1)) {
            return std::make_pair(s, Right<Error,Unit>(Unit::Unit));
        }

        return std::make_pair(s, Left<Error,Unit>("notFollowedBy not satisfied"_s));
    }};
};

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

auto const pure = []<typename A>(A a) -> Micron<A> {
    return {[a](String s) {
        return std::make_pair(s, Right<Error,A>(a));
    }};
};

auto const ap = []<typename Fn, typename A, typename B = std::invoke_result_t<Fn,A>>(Micron<Fn> mf, Micron<A> ma) -> Micron<B> {
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

auto const liftA2 = []<typename Fn, typename A, typename B>(Fn f, Micron<A> ma, Micron<B> mb) {
    return ap(fmap(curry(f), ma), mb);
};

auto const bind = []<typename Fn, typename A, typename B = UnMicronT<std::invoke_result_t<Fn,A>>>(Micron<A> ma, Fn f) -> Micron<B> {
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
