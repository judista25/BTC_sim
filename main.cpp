#include "main.hpp"

Stat STAT_WIN_MINER("Win miner");
Stat STAT_MINER_HASH_COUNT("Miner mining");
Stat proccessTime(" end time");
Stat STAT_WIN_NETWORK("Win network");
Stat STAT_NETWORK_HASH_COUNT("Network mining");
Stat STAT_proportion(" hash rate /difficulty");
Stat STAT_proportion_hash(" hash rate ");
Stat STAT_proportion_diff(" difficulty");

Stat STAT_HASH_ATTEMPTS_MINER("Hash attempts miner");
Stat STAT_HASH_ATTEMPTS_NETWORK("Hash attempts network");

Stat STAT_REWARD_NETWORK("Rewards network");
Stat STAT_REWARD_MINER("Rewards miner");

double scale = 1e10;
const double SIM_TIME_M = (86400) / scale;

#define DAY 86400
#define YEAR (DAY * 365)

const long long DIFF_TIME_DATA = 345600000;
const long long StartData = 1231545600000;
const long long YEAR_MS = static_cast<long long>(YEAR) * 1000;
int counter = 0;
double reward_miner = 0;

EvaluateBlock::EvaluateBlock(bool pool_mining, double hash_rate_miner, long long StartTime, long long EndTime, std::map<long long, double> hash_rates_history, std::map<long long, double> difficulty_history, std::map<long long, double> BTC_price_history, std::vector<double> reward_history, Facility *hasher_miner, Facility *hasher_network) : pool_mining(pool_mining), hash_rate_miner(hash_rate_miner), StartTime(StartTime), EndTime(EndTime), hash_rates_history(hash_rates_history), difficulty_history(difficulty_history), BTC_price_history(BTC_price_history), reward_history(reward_history), hasher_miner(hasher_miner), hasher_network(hasher_network) {}

void EvaluateBlock::Behavior()
{

    long long realTime = (static_cast<long long>(Time * scale) * 1000);
    realTime += StartTime;
    auto index = (realTime - ((realTime - StartData) % DIFF_TIME_DATA));

    auto pool_hash_rate = (hash_rates_history[index] * 1e12) / scale * hash_rate_miner;
    // std::cout << "Real time: " << realTime << " : " << ((realTime - StartData) % DIFF_TIME_DATA) << std::endl;
    auto rewardIndex = StartTime / YEAR_MS + 1970 - 2009;
    auto reward = reward_history[rewardIndex];
    auto hash_rate_network = (((hash_rates_history[index] * 1e12) / scale) - static_cast<long long>(pool_mining ? pool_hash_rate : hash_rate_miner));
    auto diff = difficulty_history[index] / scale;
    // double diff = 600 /hash_rate_network;
    STAT_proportion(hash_rate_network / diff);

    // std::cout << "hash rate network: " << hash_rate_network << std::endl;
    // std::cout << "diff: " << diff << std::endl;
    // std::cout << "Index: " << index << std::endl;
    // std::cout << "Reward index: " << rewardIndex << std::endl;
    // std::cout << "Reward: " << reward << std::endl;
    // std::cout << "Hash rate history size: " << hash_rates_history.size() << std::endl;
    // std::cout << "Hash rate network: " << hash_rate_network << std::endl;
    // std::cout << "Difficulty: " << diff << std::endl;
    // std::cout << "TIME: " << Time * scale << std::endl;
    // std::cout << (EndTime / 1000 - StartTime / 1000) << std::endl;

    double hash_rate_network_test = (4.4e20 / scale) - pool_mining ? pool_hash_rate : hash_rate_miner; // Real-world network hash rate (440 EH/s)
    double difficulty_test = 6.1e13 / scale;                                                           // Current Bitcoin difficulty (61 trillion)
    STAT_proportion_hash(hash_rate_network_test / hash_rate_network);
    STAT_proportion_diff(difficulty_test / diff);

    long modulo = static_cast<long>(diff);
    long block_hash = static_cast<int>(Uniform(0, modulo));
    // long block_hash = Random() * diff;

    (new MineProcess(block_hash, hash_rate_network, diff, hasher_network, reward, hasher_miner, this))->Activate();
    (new MineProcess(block_hash, pool_mining ? pool_hash_rate : hash_rate_miner, diff, hasher_miner, reward, hasher_network, this))->Activate();

    // Passivate();
    // this->Cancel();

    // mining network
}
// MineProcess
MineProcess::MineProcess(int block_hash, double hash_rate, double diff, Facility *miner_me, int reward, Facility *miner_opponent, EvaluateBlock *mainProc) : block_hash(block_hash), hash_rate(hash_rate), diff(diff), miner_me(miner_me), reward(reward), miner_opponent(miner_opponent), mainProc(mainProc) {}
void MineProcess::Behavior()
{

    int count = 0;
    RandomSeed(time(NULL));
    long modulo = static_cast<long>(diff);
    // long start_hash = static_cast<long>(Uniform(0, modulo));
    // long start_hash = Random() * diff;
    double wait_time = 1.0 / (hash_rate);

    long block_hash = static_cast<long>(Uniform(0, modulo));

    // std::cout << "Start mining: " << miner_me->Name() << " " << SIM_TIME_M << " " << wait_time << " " << Time << std::endl;
    // std::cout << "Hash rate: " << miner_me->Name() << " :" << hash_rate << std::endl;

    double StartTimeProcess = Time;

    // std::cout << "seizing " << miner_me->Name() << std::endl;
    Seize(*miner_me); // Exclusive access to hasher
    while (true)
    {
        if (miner_me->Name() == "Hasher_miner")
        {
            STAT_HASH_ATTEMPTS_MINER(1);
        }
        else
        {
            STAT_HASH_ATTEMPTS_NETWORK(1);
        }

        Wait(wait_time); // Adjusted mining rate
        count++;

        // Probabilistic block-finding
        if (static_cast<long>(Uniform(0, modulo)) == block_hash)
        {
            if (miner_me->Name() == "Hasher_miner")
            {
                STAT_WIN_MINER(count);
                STAT_REWARD_MINER(reward);
                reward_miner += reward;
            }
            else
            {
                STAT_WIN_NETWORK(count);
                STAT_REWARD_NETWORK(reward);
            }

            if (++counter % 10000 == 0)
                std::cout << "Win " + miner_me->Name() << " block count: " << counter << " Time: " << Time * scale << std::endl;

            if (miner_opponent->In())
            {
                // std::cout << "Opponent " << miner_opponent->Name() << " canceled" << std::endl;
                auto opponent = miner_opponent->In();
                Seize(*miner_opponent, 1);
                opponent->Cancel();
                Release(*miner_opponent);
                miner_opponent->Clear();

                // std::cout << "ocupancy of opponent: " << miner_opponent->Name() << " " << (miner_opponent->Busy() ? "true" : "false") << std::endl;
            }
            else
            {
                std::cout << "No opponent" << std::endl;
                Seize(*miner_opponent, 1);
                exit(1);
            }

            proccessTime((Time - StartTimeProcess) * scale);

            // std::cout << "releasing " << miner_me->Name() << std::endl;
            Release(*miner_me);
            (new WinProcess(mainProc, miner_me->Name()))->Activate();
            this->Cancel();
        }
        else
        {
            // std::cout << "Miss" << std::endl;
            if (static_cast<long>(diff) < 0)
            {
                std::cout << "Invalid diff" << std::endl;
                exit(1);
            }
            // start_hash = (start_hash + 1) % (modulo);
            // if (start_hash < 0 || start_hash > diff)
            // {
            //     std::cout << "Invalid start hash" << std::endl;
            //     exit(1);
            // }
            // modulo--;
        }
    }
}

WinProcess::WinProcess(EvaluateBlock *mainProc, std::string name) : mainProc(mainProc), name(name) {}
void WinProcess::Behavior()
{
    // mainProc->Activate(); // Reactivate the main process to continue mining
    (new EvaluateBlock(mainProc->pool_mining, mainProc->hash_rate_miner, mainProc->StartTime, mainProc->EndTime, mainProc->hash_rates_history, mainProc->difficulty_history, mainProc->BTC_price_history, mainProc->reward_history, mainProc->hasher_miner, mainProc->hasher_network))->Activate();
    mainProc->Cancel();
}

int main()
{
    long long endDate = 1732320000000;
    long long startDate = endDate - ((DAY * 1000LL) * 365LL);
    std::cout << "Start date: " << startDate << std::endl;
    Init(0, (((endDate - startDate) / 1000)) / scale);

    /// Load data
    std::map<long long, double> hash_rates_history;
    std::map<long long, double> difficulty_history;
    std::map<long long, double> BTC_price_history;
    std::vector<double> reward_history = {50, 50, 50, 50, 25, 25, 25, 25, 12.5, 12.5, 12.5, 12.5, 6.25, 6.25, 6.25, 6.25, 3.125, 3.125, 3.125, 3.125};

    std::string hash_rates_history_file = "./data/hash-rate.json";
    std::ifstream hash_rates_history_file_stream(hash_rates_history_file);

    std::string difficulty_history_file = "./data/difficulty.json";
    std::ifstream difficulty_history_file_stream(difficulty_history_file);

    std::string BTC_price_history_file = "./data/market-price.json";
    std::ifstream BTC_price_history_file_stream(BTC_price_history_file);

    nlohmann::json hash_rates_history_json;
    nlohmann::json difficulty_history_json;
    nlohmann::json BTC_price_history_json;

    if (!hash_rates_history_file_stream.is_open())
    {
        std::cerr << "Error opening file: " << hash_rates_history_file << std::endl;
        return 1;
    }
    if (!difficulty_history_file_stream.is_open())
    {
        std::cerr << "Error opening file: " << difficulty_history_file << std::endl;
        return 1;
    }
    if (!BTC_price_history_file_stream.is_open())
    {
        std::cerr << "Error opening file: " << BTC_price_history_file << std::endl;
        return 1;
    }

    hash_rates_history_file_stream >> hash_rates_history_json;
    difficulty_history_file_stream >> difficulty_history_json;
    BTC_price_history_file_stream >> BTC_price_history_json;

    for (auto &el : hash_rates_history_json["hash-rate"])
    {
        hash_rates_history[el["x"]] = el["y"];
    }
    for (auto &el : difficulty_history_json["difficulty"])
    {
        difficulty_history[el["x"]] = el["y"];
    }

    for (auto &el : BTC_price_history_json["market-price"])
    {
        BTC_price_history[el["x"]] = el["y"];
    }
    ////////////////////////////////////////////////////////////////////////////

    /*
     * Antminer S21 XP
     * 270 TH/s
     * Energy consumption: 3645 W
     * Power efficiency: 13.5 J/TH
     * Price: $9 369
     */
    double Antminer_S21_XP = 270 * 1e12; // 270 TH/s

    /*
     * MicroBT WhatsMiner M56S
     * 212 TH/s
     * Energy consumption: 5 550 W
     */
    double whatsminer_m56s = 212 * 1e12; // 212 TH/s

    /*
     * GeForce RTX 4090
     * 1.4 MH/s
     * Energy consumption: 280 W
     * Price: $2 709
     */
    double rtx_4090 = 1.4 * 1e6; // 1.4 MH/s

    // Pools
    double foundrydigital = 0.3; // 250.46 EH/s
    double antpool = 0.2;        // 172 EH/s
    double binance = 0.062;      // 52.89 EH/s

    bool pool_mining = false;
    double hash_rate_miner = Antminer_S21_XP / (pool_mining ? 1 : scale);

    // for (size_t i = 0; i < 4; i++)
    // {
        Facility *hasher_miner = new Facility("Hasher_miner");
        Facility *hasher_network = new Facility("Hasher_network");

        std::cout << "Simulation started" << std::endl;
        (new EvaluateBlock(pool_mining, hash_rate_miner, startDate, endDate, hash_rates_history, difficulty_history, BTC_price_history, reward_history, hasher_miner, hasher_network))->Activate();
        std::cout << "Simulation ended" << std::endl;
    // }
    Run();

    std::cout << "Miner reward: " << reward_miner << std::endl;
    STAT_WIN_MINER.Output();
    STAT_WIN_NETWORK.Output();
    proccessTime.Output();
    STAT_HASH_ATTEMPTS_MINER.Output();
    STAT_HASH_ATTEMPTS_NETWORK.Output();
    STAT_REWARD_MINER.Output();
    STAT_REWARD_NETWORK.Output();
    // STAT_proportion.Output();
    // STAT_proportion_diff.Output();
    // STAT_proportion_hash.Output();

    SIMLIB_statistics.Output();

    return 0;
}