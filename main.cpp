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

double scale = 1e10;
const double SIM_TIME_M = (86400) / scale;

#define DAY 86400
#define YEAR (DAY * 365)

const long long DIFF_TIME_DATA = 345600000;
const long long StartData = 1231545600000;
int max = 144;
int counter = 0;
const long long YEAR_MS = static_cast<long long>(YEAR) * 1000;

EvaluateBlock::EvaluateBlock(double hash_rate_miner, long long StartTime, long long EndTime, std::map<long long, double> hash_rates_history, std::map<long long, double> difficulty_history, std::map<long long, double> BTC_price_history, std::vector<double> reward_history) : hash_rate_miner(hash_rate_miner), StartTime(StartTime), EndTime(EndTime), hash_rates_history(hash_rates_history), difficulty_history(difficulty_history), BTC_price_history(BTC_price_history), reward_history(reward_history) {}

void EvaluateBlock::Behavior()
{
    // std::cout << "Run time: " << Time << std::endl;
    // std::cout << "Start time: " << StartTime << std::endl;
    // std::cout << "End time: " << EndTime << std::endl;
    // if (static_cast<long long>(Time * scale) + StartTime > EndTime)
    // {
    //     // should end either way
    //     std::cout << "End time: " << EndTime << " Start time: " << StartTime << " result " << ((static_cast<long long>(Time)) + StartTime > EndTime) << std::endl;
    //     Abort();
    // }
    long long realTime = (static_cast<long long>(Time * scale));
    // std::cout << "test time: " << realTime << std::endl;
    realTime += StartTime;

    auto index = realTime - ((realTime - StartData) % DIFF_TIME_DATA);
    auto rewardIndex = StartTime / YEAR_MS + 1970 - 2009;
    auto reward = reward_history[rewardIndex];
    auto hash_rate_network = (((hash_rates_history[index] * 1e12) / scale) - static_cast<long long>(hash_rate_miner)) * 4.6154;
    auto diff = difficulty_history[index] / scale * 3.68515;
    STAT_proportion(hash_rate_network / diff);

    std::cout << "hash rate network: " << hash_rate_network << std::endl;
    std::cout << "diff: " << diff << std::endl;
    std::cout << "Index: " << index << std::endl;
    std::cout << "Reward: " << reward << std::endl;
    std::cout << "Hash rate history size: " << hash_rates_history.size() << std::endl;
    std::cout << "Hash rate network: " << hash_rate_network << std::endl;
    std::cout << "Difficulty: " << diff << std::endl;
    std::cout << "TIME: " << Time * scale << std::endl;
    std::cout << (EndTime / 1000 - StartTime / 1000) << std::endl;

    double hash_rate_network_test = (4.4e20 / scale) - hash_rate_miner; // Real-world network hash rate (440 EH/s)
    double difficulty_test = 6.1e13 / scale;                            // Current Bitcoin difficulty (61 trillion)
    STAT_proportion_hash(hash_rate_network_test / hash_rate_network);
    STAT_proportion_diff(difficulty_test / diff);

    int block_hash = int(Uniform(0, diff));
    Facility *hasher_miner = new Facility("Hasher_miner");
    Facility *hasher_network = new Facility("Hasher_network");
    (new MineProcess(block_hash, hash_rate_miner, diff, hasher_miner, hasher_network, this))->Activate();
    (new MineProcess(block_hash, hash_rate_network, diff, hasher_network, hasher_miner, this))->Activate();
    Passivate();

    // mining network
}
MineProcess::MineProcess(int block_hash, double hash_rate, double diff, Facility *miner_me, Facility *miner_opponent, EvaluateBlock *mainProc) : block_hash(block_hash), hash_rate(hash_rate), diff(diff), miner_me(miner_me), miner_opponent(miner_opponent), mainProc(mainProc) {}
// MineProcess
void MineProcess::Behavior()
{
    int count = 0;
    long long start_hash = static_cast<long long>(Uniform(0, diff));
    double wait_time = 1.0 / (hash_rate);

    // std::cout << "Start mining: " << miner_me->Name() << " " << SIM_TIME_M << " " << wait_time << " " << Time << std::endl;
    // std::cout << "Hash rate: " << miner_me->Name() << " :" << hash_rate << std::endl;

    double StartTimeProcess = Time;

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

        Seize(*miner_me); // Exclusive access to hasher
        Wait(wait_time);  // Adjusted mining rate
        // std::cout << "Wait " << wait_time << std::endl;
        // std::cout << "Prewait " << Time << std::endl;
        // Activate(Time + wait_time);
        // std::cout << "After " << Time << std::endl;

        // Probabilistic block-finding
        if (start_hash == block_hash)
        {
            if (miner_me->Name() == "Hasher_miner")
            {
                STAT_WIN_MINER(count);
            }
            else
            {
                STAT_WIN_NETWORK(count);
            }
            std::cout << "Win " + miner_me->Name() << " wirh hash: " << start_hash << std::endl;
            Release(*miner_me);
            Seize(*miner_opponent, ServicePriority_t(1));
            proccessTime((Time - StartTimeProcess) * scale);

            (new WinProcess(mainProc, miner_me->Name()))->Activate();
            return;
        }
        else
        {
            // std::cout << "Miss" << std::endl;
            start_hash = (start_hash + 1) % int(diff);
            // std::cout << "diff " << diff << std::endl;
            // std::cout << "Miss " << start_hash << std::endl;
            // start_hash = static_cast<long long>(Uniform(0, diff));
            count++;
            Release(*miner_me);
        }
    }
}

WinProcess::WinProcess(EvaluateBlock *mainProc, std::string name) : mainProc(mainProc), name(name) {}
void WinProcess::Behavior()
{
    // win
    // TODO tie
    counter++;

    mainProc->Activate(); // Reactivate the main process to continue mining
}

int main()
{
    long long endDate = 1732320000000;
    long long startDate = 1617926400000;
    Init(0, (((endDate - StartData) / 1000 - (startDate - StartData) / 1000)) / scale);
    RandomSeed(std::time(NULL));

    double hash_rate_miner_s9 = 14e12;       // 14 TH/s (Antminer S9)
    double hash_rate_miner_s19_pro = 110e12; // 110 TH/s (Antminer S19 Pro)
    double hash_rate_miner_s19_xp = 140e15;  // 140 TH/s (Antminer S19 XP)

    // Base proces
    double hash_rate_miner = hash_rate_miner_s19_xp / scale;

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
        // std::cout << el["x"] << " " << el["y"] << ", ";
    }
    std::cout << std::endl;
    for (auto &el : difficulty_history_json["difficulty"])
    {
        difficulty_history[el["x"]] = el["y"];
    }

    for (auto &el : BTC_price_history_json["market-price"])
    {
        BTC_price_history[el["x"]] = el["y"];
    }

    (new EvaluateBlock(hash_rate_miner, startDate, endDate, hash_rates_history, difficulty_history, BTC_price_history, reward_history))->Activate();
    Run();
    STAT_WIN_MINER.Output();
    STAT_WIN_NETWORK.Output();
    proccessTime.Output();
    STAT_HASH_ATTEMPTS_MINER.Output();
    STAT_HASH_ATTEMPTS_NETWORK.Output();
    STAT_proportion.Output();
    STAT_proportion_diff.Output();
    STAT_proportion_hash.Output();

    SIMLIB_statistics.Output();

    return 0;
}