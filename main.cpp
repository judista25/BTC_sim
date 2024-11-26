#include "main.hpp"

// Base process

void BaseProcess::Behavior()
{
    // Print the current simulation time
    Seize(hash_ready_facility);
    Release(hashing_facility);
    factory.hashingProcess();
}

BaseProcess::BaseProcess(Facility &hash_ready_facility, Facility &hashing_facility, ProccessFactory &factory) : hash_ready_facility(hash_ready_facility), hashing_facility(hashing_facility), factory(factory) {}

// gambling process

void GamblingProcess::Behavior()
{
    if (Uniform(0, diff) == 69)
    {
        // win
        factory.winProccess();
    }
    else
    {
        // lose
        Release(hash_ready);
        factory.baseProcess();
    }
}

GamblingProcess::GamblingProcess(int diff, Facility &gambling_facility, Facility &hash_ready, ProccessFactory &factory) : diff(diff), gambling_facility(gambling_facility), hash_ready(hash_ready), factory(factory) {}

// Winner process

void WinnerProcess::Behavior()
{

    Seize(win_facility_me);
    if (!hashing_opponent.Busy())
    {
        Seize(hashing_opponent, ServicePriority_t(1));
        Release(reward_facility);
        factory.rewardProccess();
    }
    else if (!opponent_base.Busy())
    {
        Seize(opponent_base, ServicePriority_t(1));
        Release(reward_facility);
        factory.rewardProccess();
    }
}

WinnerProcess::WinnerProcess(Facility &opponent_base, Facility &hashing_opponent, Facility &win_facility_me, Facility &reward_facility, ProccessFactory &factory) : opponent_base(opponent_base), hashing_opponent(hashing_opponent), win_facility_me(win_facility_me), reward_facility(reward_facility), factory(factory) {}

// Wait process

void HashingProcess::Behavior()
{
    Wait(1 / hashRate);
    Seize(hashing_facility);
    Release(gamble_facility);
    factory.gambleProccess();
}

HashingProcess::HashingProcess(int hashRate, Facility &hashing_facility, Facility &gamble_facility, ProccessFactory &factory) : hashRate(hashRate), hashing_facility(hashing_facility), gamble_facility(gamble_facility), factory(factory) {}

// Get reward

void RewardProcess::Behavior()
{
    Seize(reward_facility);
}
RewardProcess::RewardProcess(Facility &reward_facility) : reward_facility(reward_facility) {}


ProccessFactory::ProccessFactory(int hash_rate, int diff, Facility &hash_ready_facility_me, Facility &hash_ready_facility_opponent, Facility &hashing_facility_me, Facility &hashing_facility_opponent, Facility &gamble_facility_me, Facility &gamble_facility_opponent, Facility &win_facility_me, Facility &win_facility_opponent, Facility &reward_facility_me, Facility &reward_facility_opponent) : hash_rate(hash_rate), diff(diff), hash_ready_facility_me(hash_ready_facility_me), hash_ready_facility_opponent(hash_ready_facility_opponent), hashing_facility_me(hashing_facility_me), hashing_facility_opponent(hashing_facility_opponent), gamble_facility_me(gamble_facility_me), gamble_facility_opponent(gamble_facility_opponent), win_facility_me(win_facility_me), win_facility_opponent(win_facility_opponent), reward_facility_me(reward_facility_me), reward_facility_opponent(reward_facility_opponent) {}

void ProccessFactory::baseProcess()
{
    (new BaseProcess(hash_ready_facility_me, hashing_facility_me, *this))->Activate();
}
void ProccessFactory::hashingProcess()
{
    (new HashingProcess(hash_rate, hashing_facility_me, gamble_facility_me, *this))->Activate();
}
void ProccessFactory::gambleProccess()
{
    (new GamblingProcess(hash_rate, gamble_facility_me, hash_ready_facility_me, *this))->Activate();
}
void ProccessFactory::winProccess()
{
    std::cout << "Win " << win_facility_me.Name() << std::endl;
    (new WinnerProcess(hash_ready_facility_opponent, hashing_facility_opponent, win_facility_me, reward_facility_me, *this))->Activate();
}
void ProccessFactory::rewardProccess()
{
    (new RewardProcess(reward_facility_me))->Activate();
}

void MineProcess::Behavior()
{
    int diff = 100;

    int hash_rate_miner = 10;
    int hash_rate_network = 28;

    Facility hash_ready_facility_miner("hash_ready_facility_miner");
    Facility hash_ready_facility_network("hash_ready_facility_network");

    Facility hashing_facility_miner("hashing_facility_miner");
    Facility hashing_facility_network("hashing_facility_network");

    Facility gamble_facility_miner("gamble_facility_miner");
    Facility gamble_facility_network("gamble_facility_network");

    Facility win_facility_miner("win_facility_miner");
    Facility win_facility_network("win_facility_network");

    Facility reward_facility_miner("reward_facility_miner");
    Facility reward_facility_network("reward_facility_network");

    Facility network_base("network_base");
    Facility miner_base("miner_base");

    ProccessFactory miner_factory(hash_rate_miner, diff, hash_ready_facility_miner, hash_ready_facility_network, hashing_facility_miner, hashing_facility_network, gamble_facility_miner, gamble_facility_network, win_facility_miner, win_facility_network, reward_facility_miner, reward_facility_network);
    ProccessFactory network_factory(hash_rate_network, diff, hash_ready_facility_network, hash_ready_facility_miner, hashing_facility_network, hashing_facility_miner, gamble_facility_network, gamble_facility_miner, win_facility_network, win_facility_miner, reward_facility_network, reward_facility_miner);

    miner_factory.baseProcess();
    network_factory.baseProcess();
}

int main()
{

    Init(0, 10000); // Initialize the simulation time from 0 to 100

    // Base proces

    Run();
    (new MineProcess())->Activate();

    // Run the simulation

    // Print simulation results
    SIMLIB_statistics.Output();

    return 0;
}