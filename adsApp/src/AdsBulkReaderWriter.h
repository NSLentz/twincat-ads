#ifndef ADSBULKREADERWRITER_H_
#define ADSBULKREADERWRITER_H_

#include "IAdsBulkReaderWriter.h"

class AdsBulkReaderWriter : public IAdsBulkReaderWriter
{
public:
    uint32_t doSubCommandsInBulk(uint16_t amsClientPort, AdsBulkBucket &bucket) override;
    uint32_t readInBulk(uint16_t amsClientPort, AdsBulkBucket &bucket) override;
    uint32_t writeInBulk(uint16_t amsClientPort, AdsBulkBucket &bucket) override;
    uint32_t addNotificationsInBulk(uint16_t amsClientPort, AdsBulkBucket &bucket) override;
    uint32_t delNotificationsInBulk(uint16_t amsClientPort, AdsBulkBucket &bucket) override;
    uint32_t readWriteInBulk(uint16_t amsClientPort, AdsBulkBucket &bucket, uint32_t adsSumUpCommand) override;
    uint32_t readWriteInBulkRouter(uint16_t amsClientPort, AdsBulkBucket &bucket) override;
};

#endif // ADSBULKREADERWRITER_H_