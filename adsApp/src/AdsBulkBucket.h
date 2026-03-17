#ifndef ADSBULKBUCKET_H_
#define ADSBULKBUCKET_H_

#include "AdsBulkParameter.h"
#include "IAdsBulkReaderWriter.h"
#include <memory>

bool operator==(const AmsNetId &lhs, const AmsNetId &rhs);
bool operator!=(const AmsNetId &lhs, const AmsNetId &rhs);
bool operator==(const AmsAddr &lhs, const AmsAddr &rhs);
bool operator!=(const AmsAddr &lhs, const AmsAddr &rhs);

class AdsBulkBucket
{
public:
    static constexpr size_t READ_BYTES_ALLOCATED_PER_SYMBOL_INFO_ENTRY_REQUEST = 1000;
    static constexpr size_t READ_BYTES_ALLOCATED_PER_SYMBOL_HANDLE_REQUEST = sizeof(uint32_t);
    static constexpr size_t READ_BYTES_ALLOCATED_FOR_ADS_RETURN_CODE_PER_SUB_COMMAND_OF_REQUEST = sizeof(uint32_t);
    static constexpr size_t READ_BYTES_ALLOCATED_PER_HANDLE = sizeof(uint32_t);
    static constexpr size_t WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_BULK_READ_WRITE_REQUEST = sizeof(AdsReadWriteSubCommandInfo);
    static constexpr size_t WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_BULK_READ_REQUEST = sizeof(AdsReadSubCommandInfo);
    static constexpr size_t WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_BULK_WRITE_REQUEST = sizeof(AdsWriteSubCommandInfo);
    static constexpr size_t WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_ADD_NOTIFICATION_REQUEST = sizeof(AdsAddNotificationInfo);
    static constexpr size_t WRITE_BYTES_ALLOCATED_PER_HANDLE = sizeof(uint32_t);

    const std::shared_ptr<AmsAddr> getAmsAddr() const;
    bool amsAddrIsSet() const;
    bool requestTypeIsSet() const;
    uint8_t *getRawBytesRead();
    uint32_t getNumRawBytesRead() const;
    uint8_t *getRawBytesWrite();
    uint32_t getNumRawBytesWrite() const;
    uint32_t *getRawBytesSubCommandInfo();
    size_t getSubCommandInfoSize() const;
    size_t getSubCommandInfoSizeInBytes() const;
    size_t getNumSubCommands() const;
    uint32_t getAdsReturnCodeForSubCommand(size_t subCommandIndex) const;
    bool parameterCanGoInThisBucket(const AdsBulkParameter &parameter, AdsSubCommandRequestType subCommandRequestType) const;
    AdsBulkParameter &getParameter(size_t i);
    AdsSubCommandRequestType getSubCommandRequestType(size_t i) const;
    bool push(const AdsBulkParameter &parameter, AdsSubCommandRequestType subCommandRequestType);
    AdsBulkRequestType getBulkRequestType() const;
    void resizeRawBytesWriteForBulkWrite();
    static AdsBulkRequestType getBulkRequestTypeFromSubCommandRequestType(AdsSubCommandRequestType subCommandRequestType)
    {
        switch (subCommandRequestType)
        {
        case AdsSubCommandRequestType::ADD_SYMBOL_NOTIFICATION:
            return AdsBulkRequestType::ADD_MULTIPLE_SYMBOL_NOTIFICATIONS;

        case AdsSubCommandRequestType::DEL_SYMBOL_NOTIFICATION:
            return AdsBulkRequestType::DEL_MULTIPLE_SYMBOL_NOTIFICATIONS;

        case AdsSubCommandRequestType::GET_DYNAMIC_HANDLE_FOR_INDIVIDUAL_SYMBOL_BY_NAME:
        case AdsSubCommandRequestType::READ_INFO_OF_INDIVIDUAL_SYMBOL_BY_NAME:
            return AdsBulkRequestType::DO_ACTIONS_SPECIFIED_BY_SUB_COMMAND_GROUPS;

        case AdsSubCommandRequestType::READ_SYMBOL_VALUE:
            return AdsBulkRequestType::READ_MULTIPLE_SYMBOL_VALUES;

        case AdsSubCommandRequestType::WRITE_SYMBOL_VALUE:
            return AdsBulkRequestType::WRITE_MULTIPLE_SYMBOL_VALUES;

        default:
            return AdsBulkRequestType::UNDEFINED;
        }
    }

protected:
    AdsBulkRequestType m_bulkRequestType;
    void makeAmsAddr(const AmsAddr &amsAddr);
    void makeAmsAddrIfNotSet(const AmsAddr &amsAddr);
    void setBulkRequestTypeFromSubCommandRequestTypeIfNotSet(AdsSubCommandRequestType subCommandRequestType);
    std::shared_ptr<AmsAddr> m_amsServerAddr;
    std::vector<uint8_t> m_rawBytesRead;
    std::vector<uint8_t> m_rawBytesWrite;
    std::vector<uint32_t> m_rawBytesWriteSubCommandInfo;
    size_t m_numRawBytesWriteValues = 0;
    std::vector<AdsBulkParameter> m_parameters;
    std::vector<AdsSubCommandRequestType> m_subCommandRequestTypes;
};

#endif // ADSBULKBUCKET_H_