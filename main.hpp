#ifndef MAIN_HPP
#define MAIN_HPP

#include "./include/simlib.h"
#include <iostream>

// declaration of ProccessFactory
class ProccessFactory
{
    int hash_rate;
    int diff;

    Facility &hash_ready_facility_me;
    Facility &hash_ready_facility_opponent;

    Facility &hashing_facility_me;
    Facility &hashing_facility_opponent;

    Facility &gamble_facility_me;
    Facility &gamble_facility_opponent;

    Facility &win_facility_me;
    Facility &win_facility_opponent;

    Facility &reward_facility_me;
    Facility &reward_facility_opponent;

public:
    ProccessFactory(int hash_rate, int diff, Facility &hash_ready_facility_me, Facility &hash_ready_facility_opponent, Facility &hashing_facility_me, Facility &hashing_facility_opponent, Facility &gamble_facility_me, Facility &gamble_facility_opponent, Facility &win_facility_me, Facility &win_facility_opponent, Facility &reward_facility_me, Facility &reward_facility_opponent);

    void baseProcess();
    void hashingProcess();
    void gambleProccess();
    void winProccess();
    void rewardProccess();
};

// Base process
class BaseProcess : public Process
{
    Facility &hash_ready_facility;
    Facility &hashing_facility;
    ProccessFactory &factory;
    void Behavior();

public:
    BaseProcess(Facility &hash_ready_facility, Facility &hashing_facility, ProccessFactory &factory);
};

// Gambling process
class GamblingProcess : public Process
{
    int diff;
    Facility &gambling_facility;
    Facility &hash_ready;
    ProccessFactory &factory;
    void Behavior();

public:
    GamblingProcess(int diff, Facility &gambling_facility, Facility &hash_ready, ProccessFactory &factory);
};

// Winner process
class WinnerProcess : public Process
{
    Facility &opponent_base;
    Facility &hashing_opponent;
    Facility &win_facility_me;
    Facility &reward_facility;
    ProccessFactory &factory;
    void Behavior();

public:
    WinnerProcess(Facility &opponent_base, Facility &hashing_opponent, Facility &win_facility_me, Facility &reward_facility, ProccessFactory &factory);
};

// Hashing process
class HashingProcess : public Process
{
    int hashRate;
    Facility &hashing_facility;
    Facility &gamble_facility;
    ProccessFactory &factory;
    void Behavior();

public:
    HashingProcess(int hashRate, Facility &hashing_facility, Facility &gamble_facility, ProccessFactory &factory);
};

// Reward process
class RewardProcess : public Process
{
    Facility &reward_facility;
    void Behavior();

public:
    RewardProcess(Facility &reward_facility);
};

// Process factory

// Mine process
class MineProcess : public Process
{
    void Behavior();
};

#endif // MAIN_HPP