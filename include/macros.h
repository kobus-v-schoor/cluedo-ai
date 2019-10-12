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
#include <vector>
#include <string>

namespace AI {
    class LogicLog {
        public:
            /**
             * \brief Add a line to the log
             */
            static void addLog(std::string msg);

            /**
             * \brief Returns all the log lines since the last time this function was called
             */
            static std::vector<std::string> readFromLast();

            /**
             * \brief Returns the full log
             */
            static std::vector<std::string> readFull();

        private:
            static std::vector<std::string> log;
            static int lastPos;
    };

    __attribute__((unused))
    static LogicLog logicLog;
}

#include <string.h>
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

/**
 * \brief This is the prefix that is added to all log messages
 */
#define LOG_FORMAT __FILENAME__ << "(" << __LINE__ << "): "

#ifdef NO_LOGGING
    #define LOG_LOGIC(msg) do {} while(0)
    #define LOG_ERR(msg) do {} while(0)
#else
    /**
     * \brief Used to log serious error messages that should not be ignored
     *
     * All these log messages will be printed to stderr
     */
    #define LOG_ERR(msg) std::cerr << "\033[1;31mERROR: " << LOG_FORMAT << msg << "\033[0m" << std::endl

    /**
     * \def NO_LOGIC_LOGGING
     * \brief If defined, the AI won't output any logic messages
     */
    #ifdef NO_LOGIC_LOGGING
        #define LOG_LOGIC(msg) do {} while(0)
    #else
        /**
         * \def LOG_LOGIC_TO_COUT
         * \brief If defined, all logic logging will be sent directly to std out
         */
        #ifdef LOG_LOGIC_TO_COUT
            #define LOG_LOGIC(msg) std::cout << LOG_FORMAT << msg << std::endl
        #else
            #define LOG_LOGIC(msg) AI::logicLog.addLog(std::string(msg))
        #endif
    #endif
#endif

// vim: set expandtab textwidth=100:
