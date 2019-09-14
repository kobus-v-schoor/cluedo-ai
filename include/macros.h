#pragma once

/**
 * \brief This macro marks a function as not yet implemented.
 *
 * If this macro is used on a function it means that the function is not yet
 * implemented and that it should not be considered working. If the function is
 * used a message will be generated at compile-time warning the user that the
 * function is not implemented.
 *
 * \warning This attribute it G++ specific and will not compile on other
 * compilers
 */
#define NOT_IMPLEMENTED __attribute__((__warning__("Function not yet implemented (stub)")))
