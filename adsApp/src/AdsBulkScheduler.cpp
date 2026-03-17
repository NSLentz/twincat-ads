#include "AdsBulkScheduler.h"
#include <algorithm>

AdsBulkScheduler::AdsBulkScheduler(size_t maxSubCommands)
    : m_maxSubCommands(maxSubCommands)
    , m_running(false)
{
    initializeCyclicQueues();
}

AdsBulkScheduler::~AdsBulkScheduler() {
    stop();
}

void AdsBulkScheduler::start() {
    if (m_running.exchange(true)) {
        return; // Already running
    }

    m_schedulerThread = std::thread(&AdsBulkScheduler::schedulerLoop, this);
}

void AdsBulkScheduler::stop() {
    if (!m_running.exchange(false)) {
        return; // Already stopped
    }

    if (m_schedulerThread.joinable()) {
        m_schedulerThread.join();
    }
}

void AdsBulkScheduler::pushCyclicRead(const AdsBulkParameter& param, PollingRate rate) {
    std::lock_guard<std::mutex> lock(m_cyclicMutex);
    
    auto it = m_cyclicQueues.find(rate);
    if (it != m_cyclicQueues.end()) {
        it->second.parameters.push_back(param);
    }
}

void AdsBulkScheduler::pushOneshotRead(const AdsBulkParameter& param) {
    std::lock_guard<std::mutex> lock(m_oneshotReads.bufferMutex);
    m_oneshotReads.inactiveBuffer.push_back(param);
}

void AdsBulkScheduler::pushOneshotWrite(const AdsBulkParameter& param) {
    std::lock_guard<std::mutex> lock(m_oneshotWrites.bufferMutex);
    m_oneshotWrites.inactiveBuffer.push_back(param);
}

void AdsBulkScheduler::schedulerLoop() {
    auto lastOneshotProcess = std::chrono::steady_clock::now();
    
    while (m_running) {
        auto now = std::chrono::steady_clock::now();
        
        // Process cyclic queues
        processCyclicQueues();
        
        // Process oneshot buffers every 100ms
        auto timeSinceLastOneshot = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastOneshotProcess);
        
        if (timeSinceLastOneshot.count() >= 100) {
            processOneshotBuffers();
            lastOneshotProcess = now;
        }
        
        // Sleep for a short period to prevent busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void AdsBulkScheduler::processCyclicQueues() {
    std::lock_guard<std::mutex> lock(m_cyclicMutex);
    auto now = std::chrono::steady_clock::now();
    
    for (auto& pair : m_cyclicQueues) {
        CyclicQueue& queue = pair.second;
        
        if (queue.parameters.empty()) {
            continue;
        }
        
        auto timeSinceLastExecution = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - queue.lastExecution);
        
        if (timeSinceLastExecution >= queue.interval) {
            // Calculate how many parameters to read in this cycle
            size_t remaining = queue.parameters.size() - queue.currentIndex;
            size_t batchSize = std::min(m_maxSubCommands, remaining);
            
            if (batchSize > 0) {
                // Extract batch
                std::vector<AdsBulkParameter> batch(
                    queue.parameters.begin() + queue.currentIndex,
                    queue.parameters.begin() + queue.currentIndex + batchSize
                );
                
                // Execute bulk read
                executeBulkRead(batch);
                
                // Update index
                queue.currentIndex += batchSize;
                
                // Reset index if we've completed the full cycle
                if (queue.currentIndex >= queue.parameters.size()) {
                    queue.currentIndex = 0;
                    queue.lastExecution = now;
                }
            }
        }
    }
}

void AdsBulkScheduler::processOneshotBuffers() {
    // Process oneshot reads
    {
        std::lock_guard<std::mutex> lock(m_oneshotReads.bufferMutex);
        
        // Swap buffers
        std::swap(m_oneshotReads.activeBuffer, m_oneshotReads.inactiveBuffer);
        m_oneshotReads.lastSwap = std::chrono::steady_clock::now();
    }
    
    // Process active buffer in chunks
    while (!m_oneshotReads.activeBuffer.empty()) {
        size_t batchSize = std::min(m_maxSubCommands, m_oneshotReads.activeBuffer.size());
        
        std::vector<AdsBulkParameter> batch(
            m_oneshotReads.activeBuffer.begin(),
            m_oneshotReads.activeBuffer.begin() + batchSize
        );
        
        executeBulkRead(batch);
        
        m_oneshotReads.activeBuffer.erase(
            m_oneshotReads.activeBuffer.begin(),
            m_oneshotReads.activeBuffer.begin() + batchSize
        );
    }
    
    // Process oneshot writes
    {
        std::lock_guard<std::mutex> lock(m_oneshotWrites.bufferMutex);
        
        // Swap buffers
        std::swap(m_oneshotWrites.activeBuffer, m_oneshotWrites.inactiveBuffer);
        m_oneshotWrites.lastSwap = std::chrono::steady_clock::now();
    }
    
    // Process active buffer in chunks
    while (!m_oneshotWrites.activeBuffer.empty()) {
        size_t batchSize = std::min(m_maxSubCommands, m_oneshotWrites.activeBuffer.size());
        
        std::vector<AdsBulkParameter> batch(
            m_oneshotWrites.activeBuffer.begin(),
            m_oneshotWrites.activeBuffer.begin() + batchSize
        );
        
        executeBulkWrite(batch);
        
        m_oneshotWrites.activeBuffer.erase(
            m_oneshotWrites.activeBuffer.begin(),
            m_oneshotWrites.activeBuffer.begin() + batchSize
        );
    }
}

void AdsBulkScheduler::executeBulkRead(const std::vector<AdsBulkParameter>& params) {
    // TODO: Implement actual ADS bulk read command
    // This is a placeholder that shows the structure
    
    // Example structure:
    // 1. Build ADS read request with multiple sub-commands
    // 2. Send to PLC
    // 3. Parse response
    // 4. Call callbacks with data
    
    for (const auto& param : params) {
        // Simulate read data
        std::vector<uint8_t> readData(param.size, 0);
        
        // Call callback if provided
        if (param.callback) {
            param.callback(readData);
        }
    }
}

void AdsBulkScheduler::executeBulkWrite(const std::vector<AdsBulkParameter>& params) {
    // TODO: Implement actual ADS bulk write command
    // This is a placeholder that shows the structure
    
    // Example structure:
    // 1. Build ADS write request with multiple sub-commands
    // 2. Send to PLC
    // 3. Parse response
    // 4. Call write callbacks with success status
    
    for (const auto& param : params) {
        // Simulate write success
        if (param.writeCallback) {
            param.writeCallback(true);
        }
    }
}

std::chrono::milliseconds AdsBulkScheduler::getInterval(PollingRate rate) const {
    switch (rate) {
        case PollingRate::RATE_0_1_HZ:  return std::chrono::milliseconds(10000);
        case PollingRate::RATE_0_33_HZ: return std::chrono::milliseconds(3000);
        case PollingRate::RATE_0_5_HZ:  return std::chrono::milliseconds(2000);
        case PollingRate::RATE_1_HZ:    return std::chrono::milliseconds(1000);
        case PollingRate::RATE_2_HZ:    return std::chrono::milliseconds(500);
        case PollingRate::RATE_3_HZ:    return std::chrono::milliseconds(333);
        case PollingRate::RATE_5_HZ:    return std::chrono::milliseconds(200);
        case PollingRate::RATE_10_HZ:   return std::chrono::milliseconds(100);
        default:                         return std::chrono::milliseconds(1000);
    }
}

void AdsBulkScheduler::initializeCyclicQueues() {
    auto now = std::chrono::steady_clock::now();
    
    for (int i = static_cast<int>(PollingRate::RATE_0_1_HZ); 
         i <= static_cast<int>(PollingRate::RATE_10_HZ); 
         ++i) {
        PollingRate rate = static_cast<PollingRate>(i);
        CyclicQueue queue;
        queue.lastExecution = now;
        queue.interval = getInterval(rate);
        m_cyclicQueues[rate] = queue;
    }
}
