/**
 * \file tests.h
 * \author Kobus van Schoor
 */

#pragma once

template <typename T>
T randEnum(T max_enum) {
    return T(rand() % (int(max_enum) + 1));
}

// vim: set expandtab textwidth=100:
