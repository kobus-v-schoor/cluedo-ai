#pragma once

/**
 * \brief This macro marks a function as not yet implemented (stub).
 *
 * If this macro is used on a function it means that the function is not yet implemented and that it
 * should not be considered to be in a working state. If the function is used a message will be
 * generated at compile-time warning the user that the function is not implemented.
 *
 * This is intended to be only used during development
 *
 * \note This only uses the deprecated attribute because it is portable between clang and g++, the
 * function is not actually deprected it is just not implemented yet.
 */
#define NOT_IMPLEMENTED __attribute__((__deprecated__("Function not yet implemented (stub)")))

// vim: set expandtab textwidth=100:
