#ifndef IADSBULKWRITER_H_
#define IADSBULKWRITER_H_

#include "AdsBulkBucket.h"

enum AdsBulkRequestType : uint32_t
{
    UNDEFINED = 0,
    DO_ACTIONS_SPECIFIED_BY_SUB_COMMAND_GROUPS = ADSIGRP_SUMUP_READWRITE,
    READ_MULTIPLE_SYMBOL_VALUES = ADSIGRP_SUMUP_READ,
    WRITE_MULTIPLE_SYMBOL_VALUES = ADSIGRP_SUMUP_WRITE,
    ADD_MULTIPLE_SYMBOL_NOTIFICATIONS = ADSIGRP_SUMUP_ADDDEVNOTE,
    DEL_MULTIPLE_SYMBOL_NOTIFICATIONS = ADSIGRP_SUMUP_DELDEVNOTE
};

enum AdsSubCommandRequestType : uint32_t
{
    UNDEFINED = 0,
    GET_DYNAMIC_HANDLE_FOR_INDIVIDUAL_SYMBOL_BY_NAME = ADSIGRP_SYM_HNDBYNAME,
    THE_INDEX_OFFSET_SUPPLIED_IS_A_DYNAMIC_HANDLE = ADSIGRP_SYM_VALBYHND,
    READ_INFO_OF_INDIVIDUAL_SYMBOL_BY_NAME = ADSIGRP_SYM_INFOBYNAMEEX,
    READ_SYMBOL_VALUE,
    WRITE_SYMBOL_VALUE,
    ADD_SYMBOL_NOTIFICATION,
    DEL_SYMBOL_NOTIFICATION
};

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