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
 * \warning This attribute it g++ specific and will probably not work on other compilers so this
 * will automatically disable when g++ is not being used.
 */
#ifdef __GNUG__
#define NOT_IMPLEMENTED __attribute__((__warning__("Function not yet implemented (stub)")))
#else
#define NOT_IMPLEMENTED
#endif

// vim: set expandtab textwidth=100:
