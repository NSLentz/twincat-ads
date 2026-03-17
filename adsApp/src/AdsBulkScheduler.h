#ifndef ADSBULKSCHEDULER_H_
#define ADSBULKSCHEDULER_H_

#include "AdsDef.h"
#include "IAdsBulkReaderWriter.h"
#include "adsSymbolTable.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

class AdsBulkScheduler
{
public:
    enum class PollingRate
    {
        RATE_00100_mHz,
        RATE_00250_mHz,
        RATE_00333_mHz,
        RATE_00500_mHz,
        RATE_01000_mHz,
        RATE_02500_mHz,
        RATE_03333_mHz,
        RATE_05000_mHz,
        RATE_10000_mHz
    };

    AdsBulkScheduler(const AmsNetId &amsNetId, size_t maxSubCommands = 500);
    ~AdsBulkScheduler();

    AdsBulkScheduler(const AdsBulkScheduler &) = delete;
    AdsBulkScheduler &operator=(const AdsBulkScheduler &) = delete;

    void start();
    void stop();

    void pushCyclicReadSymbolValue(const std::string &symbolName, uint16_t amsServerPort = AMSPORT_R0_PLC_TC3, PollingRate rate = PollingRate::RATE_01000_mHz);

    void pushOneshotReadSymbolInfo(const std::string &symbolName, uint16_t amsServerPort = AMSPORT_R0_PLC_TC3);
    void pushOneshotGetSymbolHandle(const std::string &symbolName, uint16_t amsServerPort = AMSPORT_R0_PLC_TC3);
    void pushOneshotReadSymbolValue(const std::string &symbolName, uint16_t amsServerPort = AMSPORT_R0_PLC_TC3);
    void pushOneshotAddNotification(const std::string &symbolName, uint16_t amsServerPort = AMSPORT_R0_PLC_TC3);
    void pushOneshotDelNotification(const std::string &symbolName, uint16_t amsServerPort = AMSPORT_R0_PLC_TC3);
    void pushOneshotWriteSymbolValue(const std::string &symbolName, uint16_t amsServerPort = AMSPORT_R0_PLC_TC3);

private:
    struct CyclicBuckets
    {
        std::vector<AdsBulkBucket> buckets;
        std::chrono::steady_clock::time_point lastExecutionTime;
        std::chrono::milliseconds interval;
    };

    struct OneshotBuckets
    {
        std::vector<AdsBulkBucket> activeBuckets;
        std::vector<AdsBulkBucket> inactiveBuckets;
        std::mutex bucketMutex;
        std::chrono::steady_clock::time_point lastSwap;
    };

    // Member variables
    size_t m_maxSubCommands;
    std::atomic<bool> m_running;
    std::thread m_schedulerThread;
    SymbolDict adsSymbolDict;

    // Cyclic Map [ams server port -> polling rate -> buckets]
    std::unordered_map<uint16_t, std::unordered_map<PollingRate, CyclicBuckets>> m_cyclicMap;
    std::mutex m_cyclicMutex;

    // Oneshot Map [ams server port -> buckets]
    std::unordered_map<uint16_t, OneshotBuckets> m_oneshotMap;

    // Helper methods
    void schedulerLoop();
    void processCyclic(CyclicBuckets &cyclicBuckets);
    void processOneshot(OneshotBuckets &oneshotBuckets);
    void executeBulkRead(const AdsBulkBucket &bucket);
    void executeBulkWrite(const AdsBulkBucket &bucket);
    std::chrono::milliseconds getInterval(PollingRate rate) const;
    void initializeCyclic();
};

#endif // ADSBULKSCHEDULER_H_