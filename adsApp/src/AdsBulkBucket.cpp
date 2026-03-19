#include "AdsBulkBucket.h"
#include "doctest.h"
#include <cstring>
#include <iostream>
#include <memory>

bool operator==(const AmsNetId &lhs, const AmsNetId &rhs)
{
    return std::equal(std::begin(lhs.b), std::end(lhs.b), std::begin(rhs.b));
}

bool operator!=(const AmsNetId &lhs, const AmsNetId &rhs)
{
    return !(lhs == rhs);
}

bool operator==(const AmsAddr &lhs, const AmsAddr &rhs)
{
    return lhs.netId == rhs.netId && lhs.port == rhs.port;
}

bool operator!=(const AmsAddr &lhs, const AmsAddr &rhs)
{
    return !(lhs == rhs);
}

AdsBulkBucket::AdsBulkBucket(size_t maxNumParametersInBucket)
    : m_bulkRequestType(AdsBulkRequestType::UNDEFINED),
      m_numRawBytesWriteValues(0),
      m_maxNumParametersInBucket(maxNumParametersInBucket)
{
    // Ensure the pointer to data() is valid.
    m_rawBytesRead.reserve(1);
    m_rawBytesWrite.reserve(1);
    m_parameters.reserve(maxNumParametersInBucket);
    m_subCommandRequestTypes.reserve(maxNumParametersInBucket);
}

const std::shared_ptr<AmsAddr> AdsBulkBucket::getAmsAddr() const
{
    return m_amsServerAddr;
}

bool AdsBulkBucket::amsAddrIsSet() const
{
    return bool(m_amsServerAddr);
}

bool AdsBulkBucket::requestTypeIsSet() const
{
    return m_bulkRequestType != AdsBulkRequestType::UNDEFINED;
}

uint8_t *AdsBulkBucket::getRawBytesRead()
{
    return m_rawBytesRead.data();
}

uint32_t AdsBulkBucket::getNumRawBytesRead() const
{
    return m_rawBytesRead.size();
}

uint8_t *AdsBulkBucket::getRawBytesWrite()
{
    return m_rawBytesWrite.data();
}

uint32_t AdsBulkBucket::getNumRawBytesWrite() const
{
    return m_rawBytesWrite.size();
}

uint32_t *AdsBulkBucket::getRawBytesSubCommandInfo()
{
    return m_rawBytesWriteSubCommandInfo.data();
}

size_t AdsBulkBucket::getSubCommandInfoSize() const
{
    return m_rawBytesWriteSubCommandInfo.size();
}

size_t AdsBulkBucket::getNumSubCommands() const
{
    return m_parameters.size();
}

uint32_t AdsBulkBucket::getAdsReturnCodeForSubCommand(size_t subCommandIndex) const
{
    auto adsReturnCode = (uint32_t *)m_rawBytesRead.data();
    adsReturnCode += subCommandIndex;
    return *adsReturnCode;
}

void AdsBulkBucket::makeAmsAddr(const AmsAddr &amsAddr)
{
    m_amsServerAddr = std::make_shared<AmsAddr>(amsAddr);
}

void AdsBulkBucket::makeAmsAddrIfNotSet(const AmsAddr &amsAddr)
{
    if (!amsAddrIsSet())
        makeAmsAddr(amsAddr);
}

void AdsBulkBucket::setBulkRequestTypeFromSubCommandRequestTypeIfNotSet(AdsSubCommandRequestType subCommandRequestType)
{
    if (!requestTypeIsSet())
        m_bulkRequestType = getBulkRequestTypeFromSubCommandRequestType(subCommandRequestType);
}

bool AdsBulkBucket::parameterCanGoInThisBucket(const AdsBulkParameter &parameter, AdsSubCommandRequestType subCommandRequestType) const
{
    if (!amsAddrIsSet() || !requestTypeIsSet())
        return false;
    return *m_amsServerAddr == parameter.amsServerAddr && m_bulkRequestType == getBulkRequestTypeFromSubCommandRequestType(subCommandRequestType);
}

AdsBulkParameter &AdsBulkBucket::getParameter(size_t i)
{
    return *m_parameters.at(i);
}

AdsSubCommandRequestType AdsBulkBucket::getSubCommandRequestType(size_t i) const
{
    return m_subCommandRequestTypes.at(i);
}

size_t AdsBulkBucket::getSubCommandInfoSizeInBytes() const
{
    return m_rawBytesWriteSubCommandInfo.size() * sizeof(uint32_t);
}

bool AdsBulkBucket::push(AdsBulkParameter &parameter, AdsSubCommandRequestType subCommandRequestType)
{
    if (m_parameters.size() >= m_maxNumParametersInBucket)
    {
        return false;
    }
    makeAmsAddrIfNotSet(parameter.amsServerAddr);
    setBulkRequestTypeFromSubCommandRequestTypeIfNotSet(subCommandRequestType);
    if (!parameterCanGoInThisBucket(parameter, subCommandRequestType))
        return false;
    switch (subCommandRequestType)
    {
    case AdsSubCommandRequestType::READ_INFO_OF_INDIVIDUAL_SYMBOL_BY_NAME:
    {
        m_rawBytesWriteSubCommandInfo.push_back(ADSIGRP_SYM_INFOBYNAMEEX);                           // IG = sub command identifier.
        m_rawBytesWriteSubCommandInfo.push_back(0);                                                  // IO = not used for getting symbol info by name.
        m_rawBytesWriteSubCommandInfo.push_back(READ_BYTES_ALLOCATED_PER_SYMBOL_INFO_ENTRY_REQUEST); // Read Len = the size of the buffer allocated to get the symbol info for this symbol.
        m_rawBytesWriteSubCommandInfo.push_back(parameter.symbolName.length());                      // Write Len = number of bytes in the symbol name, excluding null terminator.

        auto rawBytesSubCommandInfo = getRawBytesSubCommandInfo() + getSubCommandInfoSize() - 4;
        m_rawBytesRead.resize(m_rawBytesRead.size() + READ_BYTES_ALLOCATED_PER_SYMBOL_INFO_ENTRY_REQUEST);
        m_rawBytesWrite.resize(m_rawBytesWrite.size() + WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_BULK_READ_WRITE_REQUEST);
        auto rawBytesWrite = getRawBytesWrite() + getNumRawBytesWrite() - WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_BULK_READ_WRITE_REQUEST;
        memcpy(rawBytesWrite, rawBytesSubCommandInfo, WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_BULK_READ_WRITE_REQUEST);
        break;
    }
    case AdsSubCommandRequestType::GET_DYNAMIC_HANDLE_FOR_INDIVIDUAL_SYMBOL_BY_NAME:
    {
        m_rawBytesWriteSubCommandInfo.push_back(ADSIGRP_SYM_HNDBYNAME);                          // IG = sub command identifier.
        m_rawBytesWriteSubCommandInfo.push_back(0);                                              // IO = not used for getting symbol dynamic handle by name.
        m_rawBytesWriteSubCommandInfo.push_back(READ_BYTES_ALLOCATED_PER_SYMBOL_HANDLE_REQUEST); // Read Len = number of bytes in one handle.
        m_rawBytesWriteSubCommandInfo.push_back(parameter.symbolName.length());                  // Write Len = number of bytes in the symbol name, excluding null terminator.

        auto rawBytesSubCommandInfo = getRawBytesSubCommandInfo() + getSubCommandInfoSize() - 4;
        m_rawBytesRead.resize(m_rawBytesRead.size() + READ_BYTES_ALLOCATED_PER_SYMBOL_HANDLE_REQUEST);
        m_rawBytesWrite.resize(m_rawBytesWrite.size() + WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_BULK_READ_WRITE_REQUEST);
        auto rawBytesWrite = getRawBytesWrite() + getNumRawBytesWrite() - WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_BULK_READ_WRITE_REQUEST;
        memcpy(rawBytesWrite, rawBytesSubCommandInfo, WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_BULK_READ_WRITE_REQUEST);
        break;
    }
    case AdsSubCommandRequestType::READ_SYMBOL_VALUE:
    {
        if (parameter.directSymbolHandle)
        {
            // read using a dynamic handle
            m_rawBytesWriteSubCommandInfo.push_back(ADSIGRP_SYM_VALBYHND);         // IG = sub command to instruct the ams server to use the supplied index offset as a handle.
            m_rawBytesWriteSubCommandInfo.push_back(parameter.directSymbolHandle); // IO = handle identifier.
            m_rawBytesWriteSubCommandInfo.push_back(parameter.size);               // Len = number of bytes to read.
        }
        else
        {
            // read using index group and offset
            m_rawBytesWriteSubCommandInfo.push_back(parameter.iGroup);  // IG = actual ads index group address for the symbol.
            m_rawBytesWriteSubCommandInfo.push_back(parameter.iOffset); // IO = actual index offset address for the symbol.
            m_rawBytesWriteSubCommandInfo.push_back(parameter.size);    // Len = number of bytes to read.
        }
        auto rawBytesSubCommandInfo = getRawBytesSubCommandInfo() + getSubCommandInfoSize() - 3;
        m_rawBytesRead.resize(m_rawBytesRead.size() + READ_BYTES_ALLOCATED_FOR_ADS_RETURN_CODE_PER_SUB_COMMAND_OF_REQUEST + parameter.size);
        m_rawBytesWrite.resize(m_rawBytesWrite.size() + WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_BULK_READ_REQUEST);
        auto rawBytesWrite = getRawBytesWrite() + getNumRawBytesWrite() - WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_BULK_READ_REQUEST;
        memcpy(rawBytesWrite, rawBytesSubCommandInfo, WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_BULK_READ_REQUEST);
        break;
    }
    case AdsSubCommandRequestType::WRITE_SYMBOL_VALUE:
    {
        if (parameter.directSymbolHandle)
        {
            // write using a dynamic handle
            m_rawBytesWriteSubCommandInfo.push_back(ADSIGRP_SYM_VALBYHND);         // IG = sub command to instruct the ams server to use the supplied index offset as a handle.
            m_rawBytesWriteSubCommandInfo.push_back(parameter.directSymbolHandle); // IO = handle identifier.
            m_rawBytesWriteSubCommandInfo.push_back(parameter.size);               // Len = number of bytes to write.
        }
        else
        {
            // write using index group and offset
            m_rawBytesWriteSubCommandInfo.push_back(parameter.iGroup);  // IG = actual ads index group address for the symbol.
            m_rawBytesWriteSubCommandInfo.push_back(parameter.iOffset); // IO = actual index offset address for the symbol.
            m_rawBytesWriteSubCommandInfo.push_back(parameter.size);    // Len = number of bytes to write.
        }
        auto rawBytesSubCommandInfo = getRawBytesSubCommandInfo() + getSubCommandInfoSize() - 3;
        m_rawBytesRead.resize(m_rawBytesRead.size() + READ_BYTES_ALLOCATED_FOR_ADS_RETURN_CODE_PER_SUB_COMMAND_OF_REQUEST);
        m_rawBytesWrite.resize(m_rawBytesWrite.size() + WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_BULK_WRITE_REQUEST);
        auto rawBytesWrite = getRawBytesWrite() + getNumRawBytesWrite() - WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_BULK_WRITE_REQUEST;
        memcpy(rawBytesWrite, rawBytesSubCommandInfo, WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_BULK_WRITE_REQUEST);
        // Save the write values separately so they can be appended on when it comes time to make a bulk write.
        m_numRawBytesWriteValues += parameter.value.size();
        break;
    }
    case AdsSubCommandRequestType::ADD_SYMBOL_NOTIFICATION:
    {
        if (parameter.directSymbolHandle)
        {
            // write using a dynamic handle
            m_rawBytesWriteSubCommandInfo.push_back(ADSIGRP_SYM_VALBYHND);         // IG = sub command to instruct the ams server to use the supplied index offset as a handle.
            m_rawBytesWriteSubCommandInfo.push_back(parameter.directSymbolHandle); // IO = handle identifier.
            m_rawBytesWriteSubCommandInfo.push_back(parameter.size);               // Len = number of bytes to write.
        }
        else
        {
            // write using index group and offset
            m_rawBytesWriteSubCommandInfo.push_back(parameter.iGroup);  // IG = actual ads index group address for the symbol.
            m_rawBytesWriteSubCommandInfo.push_back(parameter.iOffset); // IO = actual index offset address for the symbol.
            m_rawBytesWriteSubCommandInfo.push_back(parameter.size);    // Len = number of bytes to write.
        }
        auto rawBytesSubCommandInfo = getRawBytesSubCommandInfo() + getSubCommandInfoSize() - 3;
        m_rawBytesRead.resize(m_rawBytesRead.size() + READ_BYTES_ALLOCATED_FOR_ADS_RETURN_CODE_PER_SUB_COMMAND_OF_REQUEST + READ_BYTES_ALLOCATED_PER_HANDLE);
        m_rawBytesWrite.resize(m_rawBytesWrite.size() + WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_ADD_NOTIFICATION_REQUEST);
        auto rawBytesWrite = getRawBytesWrite() + getNumRawBytesWrite() - WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_ADD_NOTIFICATION_REQUEST;
        memcpy(rawBytesWrite, rawBytesSubCommandInfo, WRITE_BYTES_ALLOCATED_PER_SUB_COMMAND_OF_ADD_NOTIFICATION_REQUEST);
        break;
    }
    case AdsSubCommandRequestType::DEL_SYMBOL_NOTIFICATION:
    {
        m_rawBytesWriteSubCommandInfo.push_back(parameter.notificationHandle); // handle identifier.
        auto rawBytesSubCommandInfo = getRawBytesSubCommandInfo() + getSubCommandInfoSize() - 1;
        m_rawBytesRead.resize(m_rawBytesRead.size() + READ_BYTES_ALLOCATED_FOR_ADS_RETURN_CODE_PER_SUB_COMMAND_OF_REQUEST);
        m_rawBytesWrite.resize(m_rawBytesWrite.size() + WRITE_BYTES_ALLOCATED_PER_HANDLE);
        auto rawBytesWrite = getRawBytesWrite() + getNumRawBytesWrite() - WRITE_BYTES_ALLOCATED_PER_HANDLE;
        memcpy(rawBytesWrite, rawBytesSubCommandInfo, WRITE_BYTES_ALLOCATED_PER_HANDLE);
        break;
    }
    default:
    {
        return false;
    }
    }
    m_subCommandRequestTypes.push_back(subCommandRequestType);
    m_parameters.push_back(&parameter);
    return true;
}

AdsBulkRequestType AdsBulkBucket::getBulkRequestType() const
{
    return m_bulkRequestType;
}

void AdsBulkBucket::resizeRawBytesWriteForBulkWrite()
{
    m_rawBytesWrite.resize(getSubCommandInfoSizeInBytes() + m_numRawBytesWriteValues);
}