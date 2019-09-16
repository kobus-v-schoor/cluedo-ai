#pragma once

template <typename T>
T rand_enum(T max_enum) {
    return T(rand() % (int(max_enum) + 1));
}

// vim: set expandtab textwidth=100:
