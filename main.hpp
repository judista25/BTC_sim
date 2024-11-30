#ifndef MAIN_HPP
#define MAIN_HPP

#include "./include/simlib.h"
#include <iostream>
#include <ctime> // for time()
#include <vector>
#include <map>
#include <fstream>
#include "nlohmann/json.hpp"

class EvaluateBlock : public Event
{
    double hash_rate_miner;
    double hash_rate_network;
    double diff;
    long long StartTime;
    long long EndTime;
    std::map<long long, double> hash_rates_history;
    std::map<long long, double> difficulty_history;
    std::map<long long, double> BTC_price_history;
    std::vector<double> reward_history;

    void Behavior();

public:
    EvaluateBlock(double hash_rate_miner, long long StartTime, long long EndTime, std::map<long long, double> hash_rates_history, std::map<long long, double> difficulty_history, std::map<long long, double> BTC_price_history, std::vector<double> reward_history);
};

class MineProcess : public Process
{
    int block_hash;
    double hash_rate;
    double diff;
    Facility *miner_me;
    Facility *miner_opponent;
    EvaluateBlock *mainProc;
    void Behavior();

public:
    MineProcess(int block_hash, double hahs_rate, double diff, Facility *miner_me, Facility *miner_opponent, EvaluateBlock *mainProc);
};

class WinProcess : public Process
{
    EvaluateBlock *mainProc;
    std::string name;
    void Behavior();

public:
    WinProcess(EvaluateBlock *mainProc, std::string name);
};

#endif // MAIN_HPP