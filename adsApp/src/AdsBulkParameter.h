#ifndef ADSBULKPARAMETER_H_
#define ADSBULKPARAMETER_H_

#include "AdsDef.h"
#include <functional>
#include <vector>

#pragma pack(push, 1)
struct AdsReadWriteSubCommandInfo
{
    uint32_t indexGroup = 0;
    uint32_t indexOffset = 0;
    uint32_t readSizeInBytes = 0;
    uint32_t writeSizeInBytes = 0;
};

struct AdsReadSubCommandInfo
{
    uint32_t indexGroup = 0;
    uint32_t indexOffset = 0;
    uint32_t readSizeInBytes = 0;
};

struct AdsWriteSubCommandInfo
{
    uint32_t indexGroup = 0;
    uint32_t indexOffset = 0;
    uint32_t writeSizeInBytes = 0;
};

struct AdsAddNotificationInfo
{
    uint32_t indexGroup = 0;
    uint32_t indexOffset = 0;
    uint32_t writeSizeInBytes = 0;
};
#pragma pack(pop)

struct AdsBulkParameter
{
    AdsBulkParameter(const AmsAddr& amsServerAddr);
    AmsAddr amsServerAddr;
    uint32_t iGroup = 0;
    uint32_t iOffset = 0;
    uint32_t directSymbolHandle = 0;
    uint32_t size = 0;
    uint32_t dataType = 0;
    uint32_t flags = 0;
    std::string symbolName;
    std::string datatypeName;
    std::string symbolComment;
    std::vector<uint8_t> value;
    uint32_t lastAdsReturnCode = 0;
    uint32_t notificationHandle = 0;
};

#endif // ADSBULKPARAMETER_H_