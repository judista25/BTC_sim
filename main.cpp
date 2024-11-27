#include "main.hpp"
int max = 12;
int counter = 0;
void EvaluateBlock::Behavior()
{
    Facility *hasher_miner = new Facility("Hasher_miner");
    Facility *hasher_network = new Facility("Hasher_network");
    (new MineProcess(hasher_miner, hasher_network, this))->Activate();
    (new MineProcess(hasher_network, hasher_miner, this))->Activate();
    Passivate();
    
    // mining network
}
MineProcess::MineProcess(Facility *miner_me, Facility *miner_opponent, EvaluateBlock *mainProc) : miner_me(miner_me), miner_opponent(miner_opponent), mainProc(mainProc) {}
// MineProcess
void MineProcess::Behavior()
{
    int hash_rate = 1000;
    int diff = 100;
    while (true)
    {
        // std::cerr << miner_me->Name() << " mining" << std::endl;

        Seize(*miner_me);
        Wait(1 / hash_rate);
        // gamble
        if (int(Uniform(1, diff)) == 6)
        {
            std::cerr << miner_me->Name() << " wins" << std::endl;
            Release(*miner_me);
            Seize(*miner_opponent, ServicePriority_t(1));
            (new WinProcess(mainProc, miner_me->Name()))->Activate();
            return;
        }
        else
        {
            // std::cerr << miner_me->Name() << " not found" << std::endl;
            Release(*miner_me);
        }
    }
}
WinProcess::WinProcess(EvaluateBlock *mainProc, std::string name) : mainProc(mainProc), name(name) {}
void WinProcess::Behavior()
{
    // win
    // TOOD tie
    if (counter++ < max)
    {
        mainProc->Activate();
    }
}

int main()
{

    Init(0, 10000); // Initialize the simulation time from 0 to 100

    // Base proces

    (new EvaluateBlock())->Activate();
    Run();

    // Run the simulation

    // Print simulation results
    SIMLIB_statistics.Output();

    return 0;
}