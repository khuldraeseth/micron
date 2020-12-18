#pragma once

#include <memory>
#include <ostream>


template <typename T>
struct Node {
    T const mVal {};
    std::shared_ptr<Node<T>> const mNext {};

    Node() = default;
    Node(T val, std::shared_ptr<Node<T>> next) : mVal{val}, mNext{next} {}
};

template <typename T>
std::ostream& operator<<(std::ostream& ost, std::shared_ptr<Node<T>> n) {
    if (n == nullptr) {
        return ost;
    }

    if (n->mNext == nullptr) {
        return ost << n->mVal;
    }

    return ost << n->mVal << ',' << n->mNext;
}

template <typename T>
struct List {
    std::shared_ptr<Node<T>> const mHead {};

    List() = default;
    List(std::shared_ptr<Node<T>> head) : mHead{head} {}
};

template <typename T>
List<T> nil {};

template <typename T>
bool null(List<T> xs) {
    return xs.mHead == nullptr;
}

template <typename T>
T head(List<T> xs) {
    return xs.mHead->mVal;
}

template <typename T>
List<T> tail(List<T> xs) {
    return {xs.mHead->mNext};
}

template <typename T>
std::ostream& operator<<(std::ostream& ost, List<T> xs) {
    return ost << '[' << xs.mHead << ']';
}

template <typename T>
List<T> operator|=(T const& x, List<T> xs) {
    return {std::make_shared<Node<T>>(x, xs.mHead)};
}

auto const cons = [](auto const& x, auto const& xs) {
    return x |= xs;
};

template <typename T>
List<T> operator&=(List<T> xs, List<T> ys) {
    if (null(xs)) {
        return ys;
    }

    return head(xs) |= tail(xs) &= ys;
}
