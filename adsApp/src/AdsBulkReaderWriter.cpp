#include "AdsBulkReaderWriter.h"
#include "AdsLib.h"
#include "adsSymbolTable.h"
#include <cstring>
#include <iostream>

uint32_t AdsBulkReaderWriter::doSubCommandsInBulk(uint16_t amsClientPort, AdsBulkBucket &bucket)
{
    auto adsResult = readWriteInBulk(amsClientPort, bucket);
    auto adsReadWriteReturnInfo = reinterpret_cast<AdsReadWriteReturnInfo *>(bucket.getRawBytesRead());
    auto adsReadData = bucket.getRawBytesRead() + bucket.getSubCommandInfoSizeInBytes();
    for (size_t i = 0; i < bucket.getNumSubCommands(); i++)
    {
        auto &parameter = bucket.getParameter(i);
        auto subCommandRequestType = bucket.getSubCommandRequestType(i);
        // The return comes back with sets of 8 bytes.
        // The first four bytes are the ads return code (indicates error or success)
        parameter.lastAdsReturnCode = adsReadWriteReturnInfo->adsReturnCode;

        // The second 4 bytes are the length of the read data for this sub command.
        auto readLengthInBytesForThisParameter = adsReadWriteReturnInfo->adsReturnLength;

        // Advance the read write info pointer to the next parameter.
        adsReadWriteReturnInfo++;

        // If there was an ads error, skip the rest of the operations.
        if (parameter.lastAdsReturnCode)
        {
            // Save the error to be processed elsewhere and advance the read data
            // pointer by the read length to get to the next parameter.
            adsReadData += readLengthInBytesForThisParameter;
            continue;
        }

        switch (subCommandRequestType)
        {
        case AdsSubCommandRequestType::GET_DYNAMIC_HANDLE_FOR_INDIVIDUAL_SYMBOL_BY_NAME:
        {
            parameter.directSymbolHandle = *reinterpret_cast<uint32_t *>(adsReadData);
            adsReadData += readLengthInBytesForThisParameter;
            continue;
        }
        case AdsSubCommandRequestType::READ_INFO_OF_INDIVIDUAL_SYMBOL_BY_NAME:
        {
            AdsSymbolEntryWire symbolEntryInfo;
            // If the info read succeeded, we are guaranteed at least the first 30 bytes of the symbol entry.
            // So we can copy those bytes into our info structure to make them more friendly to access.
            memcpy(&symbolEntryInfo, adsReadData, sizeof(AdsSymbolEntryWire));

            // We get an additional, dynamic number of bytes which give the symbol name, datatype name, and comment string
            // in the form of characters that are terminated by the null terminator.
            // The symbol name starts at the 31st byte.
            auto pointerToSymbolNameStart = (char *)(adsReadData + sizeof(AdsSymbolEntryWire));

            // The name length value does not include the null terminator, so the start of the datatype name
            // is the length of the symbol name plus 1 to advance to the first character beyond the null terminator.
            auto pointerToDatatypeNameStart = pointerToSymbolNameStart + symbolEntryInfo.nameLength + 1;

            // Repeat for the comment string.
            auto pointerToSymbolCommentStart = pointerToDatatypeNameStart + symbolEntryInfo.typeLength + 1;

            // The std::string operator= function will copy all the characters in the data until it hits a null terminator.
            parameter.symbolName = pointerToSymbolNameStart;
            parameter.datatypeName = pointerToDatatypeNameStart;
            parameter.symbolComment = pointerToSymbolCommentStart;

            // Copy the rest of the parameter information we want to store.
            parameter.iGroup = symbolEntryInfo.iGroup;
            parameter.iOffset = symbolEntryInfo.iOffs;
            parameter.size = symbolEntryInfo.size;
            parameter.dataType = symbolEntryInfo.dataType;
            parameter.flags = symbolEntryInfo.flags;

            // We advance by the read length of the entry to be at the next unread byte for the next parameter.
            adsReadData += readLengthInBytesForThisParameter;
            continue;
        }
        default:
        {
            adsReadData += readLengthInBytesForThisParameter;
            std::cerr << "Ads sub command [" << parameter.iGroup << "] is not supported by this ads bulk reader/writer." << std::endl;
            continue;
        }
        }
    }
    return adsResult;
}

uint32_t AdsBulkReaderWriter::readInBulk(uint16_t amsClientPort, AdsBulkBucket &bucket)
{
    auto adsResult = readWriteInBulk(amsClientPort, bucket);
    auto adsReturnCode = reinterpret_cast<uint32_t *>(bucket.getRawBytesRead());
    auto adsReadData = bucket.getRawBytesRead() + bucket.getSubCommandInfoSizeInBytes();
    for (size_t i = 0; i < bucket.getNumSubCommands(); i++)
    {
        auto &parameter = bucket.getParameter(i);
        auto subCommandRequestType = bucket.getSubCommandRequestType(i);

        parameter.lastAdsReturnCode = *adsReturnCode;
        adsReturnCode++;

        memcpy(parameter.value.data(), adsReadData, parameter.value.size());
        adsReadData += parameter.value.size();
    }
    return adsResult;
}

uint32_t AdsBulkReaderWriter::writeInBulk(uint16_t amsClientPort, AdsBulkBucket &bucket)
{
    bucket.resizeRawBytesWriteForBulkWrite();
    auto pointerToRawBytesWrite = bucket.getRawBytesWrite() + bucket.getSubCommandInfoSizeInBytes();
    for (size_t i = 0; i < bucket.getNumSubCommands(); i++)
    {
        auto &parameter = bucket.getParameter(i);
        memcpy(pointerToRawBytesWrite, parameter.value.data(), parameter.value.size());
    }
    auto adsResult = readWriteInBulk(amsClientPort, bucket);
    auto adsReturnCode = reinterpret_cast<uint32_t *>(bucket.getRawBytesRead());
    for (size_t i = 0; i < bucket.getNumSubCommands(); i++)
    {
        auto &parameter = bucket.getParameter(i);

        parameter.lastAdsReturnCode = *adsReturnCode;
        adsReturnCode++;
    }
    return adsResult;
}

uint32_t AdsBulkReaderWriter::addNotificationsInBulk(uint16_t amsClientPort, AdsBulkBucket &bucket)
{
    auto adsResult = readWriteInBulk(amsClientPort, bucket);
    auto adsReturnCode = reinterpret_cast<uint32_t *>(bucket.getRawBytesRead());
    auto adsReadData = bucket.getRawBytesRead() + bucket.getSubCommandInfoSizeInBytes();
    for (size_t i = 0; i < bucket.getNumSubCommands(); i++)
    {
        auto &parameter = bucket.getParameter(i);

        parameter.lastAdsReturnCode = *adsReturnCode;
        adsReturnCode++;

        parameter.directSymbolHandle = *reinterpret_cast<uint32_t *>(adsReadData);
        adsReadData += sizeof(uint32_t);
    }
    return adsResult;
}

uint32_t AdsBulkReaderWriter::delNotificationsInBulk(uint16_t amsClientPort, AdsBulkBucket &bucket)
{
    auto adsResult = readWriteInBulk(amsClientPort, bucket);
    auto adsReturnCode = reinterpret_cast<uint32_t *>(bucket.getRawBytesRead());
    for (size_t i = 0; i < bucket.getNumSubCommands(); i++)
    {
        auto &parameter = bucket.getParameter(i);

        parameter.lastAdsReturnCode = *adsReturnCode;
        adsReturnCode++;
    }
    return adsResult;
}

uint32_t AdsBulkReaderWriter::readWriteInBulk(uint16_t amsClientPort, AdsBulkBucket &bucket)
{
    uint32_t bytesRead = 0;
    auto adsResult = AdsSyncReadWriteReqEx2(
        amsClientPort, bucket.getAmsAddr().get(),
        bucket.getBulkRequestType(), bucket.getNumSubCommands(),
        bucket.getNumRawBytesRead(), bucket.getRawBytesRead(),
        bucket.getNumRawBytesWrite(), bucket.getRawBytesWrite(),
        &bytesRead);
    if (bytesRead != bucket.getNumRawBytesRead())
    {
        std::cerr << "The number of bytes read did not match the number of bytes expected." << std::endl;
    }
    return adsResult;
}

uint32_t AdsBulkReaderWriter::readWriteInBulkRouter(uint16_t amsClientPort, AdsBulkBucket &bucket)
{
    uint32_t adsResult = 0;
    switch (bucket.getBulkRequestType())
    {
    case AdsBulkRequestType::ADD_MULTIPLE_SYMBOL_NOTIFICATIONS:
        adsResult = addNotificationsInBulk(amsClientPort, bucket);
        break;
    case AdsBulkRequestType::DEL_MULTIPLE_SYMBOL_NOTIFICATIONS:
        adsResult = delNotificationsInBulk(amsClientPort, bucket);
        break;
    case AdsBulkRequestType::READ_MULTIPLE_SYMBOL_VALUES:
        adsResult = readInBulk(amsClientPort, bucket);
        break;
    case AdsBulkRequestType::WRITE_MULTIPLE_SYMBOL_VALUES:
        adsResult = writeInBulk(amsClientPort, bucket);
        break;
    case AdsBulkRequestType::DO_ACTIONS_SPECIFIED_BY_SUB_COMMAND_GROUPS:
        adsResult = doSubCommandsInBulk(amsClientPort, bucket);
        break;
    default:
        std::cerr << "Ads bulk request [" << bucket.getBulkRequestType() << "] is not supported by this ads reader/writer class." << std::endl;
        break;
    }
    return adsResult;
}