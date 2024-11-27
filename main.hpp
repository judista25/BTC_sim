#ifndef MAIN_HPP
#define MAIN_HPP

#include "./include/simlib.h"
#include <iostream>

class EvaluateBlock : public Event
{
    void Behavior();
};

class MineProcess : public Process
{
    Facility *miner_me;
    Facility *miner_opponent;
    EvaluateBlock *mainProc;
    void Behavior();
    public:
        MineProcess(Facility *miner_me, Facility *miner_opponent, EvaluateBlock * mainProc);
};

class WinProcess : public Process
{
    EvaluateBlock *mainProc;
    std::string name;
    void Behavior();
    public:
        WinProcess(EvaluateBlock * mainProc, std::string name);
};

#endif // MAIN_HPP