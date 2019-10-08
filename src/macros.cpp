/**
 * \file macros.cpp
 * \author Kobus van Schoor
 */

#include "../include/macros.h"
#include <mutex>

using namespace AI;

LogicLog logicLog;
std::vector<std::string> LogicLog::log;
int LogicLog::lastPos = 0;
std::mutex lock;

void LogicLog::addLog(std::string msg)
{
    std::lock_guard<std::mutex> l(lock);

    log.push_back(msg);
}

std::vector<std::string> LogicLog::readFromLast()
{
    std::lock_guard<std::mutex> l(lock);

    auto ret = std::vector<std::string>(log.begin() + lastPos, log.end());
    lastPos = log.size();
    return ret;
}

std::vector<std::string> LogicLog::readFull()
{
    std::lock_guard<std::mutex> l(lock);

    return log;
}

// vim: set expandtab textwidth=100:
