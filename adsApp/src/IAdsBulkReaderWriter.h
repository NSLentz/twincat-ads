#ifndef IADSBULKWRITER_H_
#define IADSBULKWRITER_H_

#include "AdsBulkBucket.h"

#pragma pack(push, 1)
struct AdsReadWriteReturnInfo
{
    uint32_t adsReturnCode = 0;
    uint32_t adsReturnLength = 0;
};
#pragma pack(pop)

class IAdsBulkReaderWriter
{
public:
    virtual ~IAdsBulkReaderWriter() = default;
    virtual uint32_t doSubCommandsInBulk(uint16_t amsClientPort, AdsBulkBucket &bucket);
    virtual uint32_t readInBulk(uint16_t amsClientPort, AdsBulkBucket &bucket);
    virtual uint32_t writeInBulk(uint16_t amsClientPort, AdsBulkBucket &bucket);
    virtual uint32_t addNotificationsInBulk(uint16_t amsClientPort, AdsBulkBucket &bucket);
    virtual uint32_t delNotificationsInBulk(uint16_t amsClientPort, AdsBulkBucket &bucket);
    virtual uint32_t readWriteInBulk(uint16_t amsClientPort, AdsBulkBucket &bucket);
    virtual uint32_t readWriteInBulkRouter(uint16_t amsClientPort, AdsBulkBucket &bucket);
};

#endif // IADSBULKWRITER_H_