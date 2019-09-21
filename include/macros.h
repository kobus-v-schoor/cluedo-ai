#pragma once

/**
 * \brief This macro marks a function as "not yet implemented" (hence the function is a stub).
 *
 * If this macro is used on a function it means that the function is not yet implemented and that it
 * should not be considered to be in a working state. If the function is used, a message will be
 * generated at compile-time warning the user that the function is not implemented.
 *
 * This is intended to be only used during development
 *
 * \note This only uses the deprecated attribute because it is portable between clang and g++, the
 * function is not actually deprected, it is just not implemented yet.
 */
#define NOT_IMPLEMENTED __attribute__((__deprecated__("Function not yet implemented (stub)")))

/**
 * \brief Macro to set a breakpoint for gdb in code
 *
 * Using this macro somewhere in the code will signal an interrupt that will automatically stop
 * execution in gdb. This makes it easier to set a breakpoint without needing to do it in gdb
 * manually on every execution
 * \warning This macro should only ever be needed during development. It should not be used in any
 * production code
 * \warning This probably won't work on Windows
 */
#include <csignal>
#define BREAK std::raise(SIGINT);

#include <iostream>
#define LOG_FORMAT __FILE__ << "(" << __LINE__ << "): "

/**
 * \brief Used to log serious error messages that should not be ignored
 *
 * All these log messages will be printed to stderr
 */
#define ERR_LOG(msg) std::cerr << LOG_FORMAT << msg << std::endl


#ifndef AI_NO_LOGIC_LOG
    /**
     * \brief Used to log messages about logic decisions being made
     *
     * These messages can be disabled by defining AI_NO_LOGIC_LOG
     */
    #define LOGIC_LOG(msg) std::cout << LOG_FORMAT << msg << std::endl
#endif

/**
 * \def INFO_LOG
 * \brief Can be used to log informational messages
 *
 * The macro will automatically be disabled when compiling with AI_RELEASE being defined (e.g. using
 * compiler flags)
 */

#ifdef AI_RELEASE
    // the do while gets optimized as a noop
    #define INFO_LOG(msg) do {} while(0)
#else
    #define INFO_LOG(msg) std::cout << LOG_FORMAT << msg << std::endl
#endif

// vim: set expandtab textwidth=100:
