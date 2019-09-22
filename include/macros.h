/**
 * \file macros.h
 * \author Kobus van Schoor
 */

#pragma once

/**
 * \brief This macro marks a function as "not yet implemented" (hence the function is a stub).
 *
 * If this macro is used on a function it means that the function is not yet implemented and that it
 * should not be considered to be in a working state. If the function is used, a message will be
 * generated at compile-time warning the user that the function is not implemented.
 *
 * \note This is intended to be only used during active development
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

/**
 * \brief This is the prefix that is added to all log messages
 */
#define LOG_FORMAT __FILE__ << "(" << __LINE__ << "): "

/**
 * \brief Used to log serious error messages that should not be ignored
 *
 * All these log messages will be printed to stderr
 */
#define LOG_ERR(msg) std::cerr << LOG_FORMAT << msg << std::endl


#ifndef AI_NO_LOG_LOGIC
    /**
     * \brief Used to log messages about logic decisions being made
     *
     * These messages can be disabled by defining AI_NO_LOG_LOGIC
     */
    #define LOG_LOGIC(msg) std::cout << LOG_FORMAT << msg << std::endl
#endif

/**
 * \def LOG_INFO
 * \brief Can be used to log informational messages
 *
 * The macro will automatically be disabled when compiling with AI_RELEASE being defined (e.g. using
 * compiler flags)
 */

#ifdef AI_RELEASE
    // the do while gets optimized as a noop
    #define LOG_INFO(msg) do {} while(0)
#else
    #define LOG_INFO(msg) std::cout << LOG_FORMAT << msg << std::endl
#endif

// vim: set expandtab textwidth=100:
