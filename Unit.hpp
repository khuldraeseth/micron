#pragma once

#include <ostream>

enum class Unit {
    Unit
};

std::ostream& operator<<(std::ostream& ost, Unit _) {
    return ost << "()";
}
