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

template <typename A>
Either<Error,A> runMicron(Micron<A> ma, String s) {
    return ma.fn(s).second;
}

template <typename Fn>
Micron<char> satisfy(Fn f) {
    return Micron<char> {[f](String s) {
        if (null(s)) {
            return std::make_pair(s, Left<Error,char>("unexpected eof"_s));
        }

        if (!f(head(s))) {
            return std::make_pair(s, Left<Error,char>("unsatisfied"_s));
        }

        return std::make_pair(tail(s), Right<Error,char>(head(s)));
    }};
}

Micron<char> const anyChar = satisfy([](char _) { return true; });

Micron<char> const digit = satisfy([](char c) { return std::isdigit(c); });

template <typename A>
Micron<Unit> notFollowedBy(Micron<A> ma) {
    return Micron<Unit> {[ma](String s) {
        auto const [s1, r1] = ma.fn(s);
        if (isLeft(r1)) {
            return std::make_pair(s, Right<Error,Unit>(Unit::Unit));
        }

        return std::make_pair(s, Left<Error,Unit>("notFollowedBy not satisfied"_s));
    }};
}

Micron<Unit> const eof = notFollowedBy(anyChar);

template <typename A>
Micron<A> fail(Error e) {
    return Micron<A> {[e](String s) {
        return std::make_pair(s, Left<Error,A>(e));
    }};
}

template <typename Fn, typename A, typename B = std::invoke_result_t<Fn,A>>
Micron<B> fmap(Fn f, Micron<A> ma) {
    return Micron<B> {[f,ma](String s) {
        auto const [s1, r1] = ma.fn(s);
        if (isLeft(r1)) {
            return std::make_pair(s1, Left<Error,B>(fromLeft(r1)));
        }

        return std::make_pair(s1, Right<Error,B>(f(fromRight(r1))));
    }};
}

template <typename A>
Micron<A> pure(A a) {
    return Micron<A> {[a](String s) {
        return std::make_pair(s, Right<Error,A>(a));
    }};
}

template <typename Fn, typename A, typename B = std::invoke_result_t<Fn,A>>
Micron<B> ap(Micron<Fn> mf, Micron<A> ma) {
    return Micron<B> {[mf,ma](String s) {
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
}

auto curry(auto const& f) {
    return [f](auto const& x) {
        return [f,x](auto const& y) {
            return f(x, y);
        };
    };
}

template <typename Fn, typename A, typename B, typename C = std::invoke_result_t<Fn,A,B>>
Micron<C> liftA2(Fn f, Micron<A> ma, Micron<B> mb) {
    return ap(fmap(curry(f), ma), mb);
}

template <typename Fn, typename A, typename B>
Micron<B> bind(Micron<A> ma, Fn f) {
    return Micron<B> {[ma,f](String s) {
        auto const [s1, r1] = ma.fn(s);
        if (isLeft(r1)) {
            return std::make_pair(s, Left<Error,B>(fromLeft(r1)));
        }

        return f(fromRight(r1)).fn(s1);
    }};
}

template <typename A> Micron<A> const empty = fail<A>("empty"_s);

template <typename A>
Micron<A> orElse(Micron<A> m1, Micron<A> m2) {
    return Micron<A> {[m1,m2](String s) {
        auto const [s1, r1] = m1.fn(s);
        if (isRight(r1)) {
            return std::make_pair(s1, r1);
        }

        return m2.fn(s);
    }};
}

template <typename A>
Micron<List<A>> many(Micron<A> ma) {
    if (isRight(ma.fn(""_s).second)) {
        return fail<List<A>>("combinator `many` is applied to a parser that accepts an empty string"_s);
    }

    return Micron<List<A>> {[ma](String s) {
        auto const [s1, r1] = ma.fn(s);
        if (isLeft(r1)) {
            return std::make_pair(s, Right<Error,List<A>>(nil<A>));
        }

        return fmap(curry(cons)(fromRight(r1)), many(ma)).fn(s1);
    }};
}

template <typename A>
Micron<List<A>> some(Micron<A> ma) {
    return liftA2(cons, ma, many(ma));
}

template <typename A>
Micron<List<A>> many1(Micron<A> ma) {
    return some(ma);
}

template <typename A, typename B>
Micron<B> constMap(B b, Micron<A> ma) {
    return fmap([b](auto _) { return b; }, ma);
}

template <typename A, typename B>
Micron<B> then(Micron<A> ma, Micron<B> mb) {
    return liftA2([](auto a, auto b) { return b; }, ma, mb);
}

template <typename A, typename B>
Micron<A> before(Micron<A> ma, Micron<B> mb) {
    return liftA2([](auto a, auto b) { return a; }, ma, mb);
}

template <typename A>
Micron<Unit> skipMany(Micron<A> ma) {
    return constMap(Unit::Unit, many(ma));
}

Micron<Unit> const spaces = skipMany(satisfy([](char c) { return std::isspace(c); }));
