#include "AdsBulkBucket.h"
#include "doctest.h"
#include <cstring>

TEST_SUITE("AdsBulkBucket")
{
    TEST_CASE("bool push(const AdsBulkParameter &parameter, AdsSubCommandRequestType subCommandRequestType)\n"
              "AdsBulkRequestType getBulkRequestType() const")
    {
        SUBCASE("Push one undefined request parameter.\n"
                "Expect return value is false and bulk request type does not update.")
        {
            AdsBulkBucket bucket(10);
            AdsBulkParameter parameter0;
            auto result = bucket.push(parameter0, AdsSubCommandRequestType::UNDEFINED);
            CHECK_FALSE(result);
            auto bulkRequestTypeResult0 = bucket.getBulkRequestType();
            CHECK(bulkRequestTypeResult0 == AdsBulkRequestType::UNDEFINED);
        }
        SUBCASE("Push one valid request parameter.\n"
                "Expect return value is true and bulk request type updates.")
        {
            AdsBulkBucket bucket(1);
            AdsBulkParameter parameter0;
            auto result = bucket.push(parameter0, AdsSubCommandRequestType::GET_DYNAMIC_HANDLE_FOR_INDIVIDUAL_SYMBOL_BY_NAME);
            CHECK(result);
            auto bulkRequestTypeResult0 = bucket.getBulkRequestType();
            CHECK(bulkRequestTypeResult0 == AdsBulkRequestType::DO_ACTIONS_SPECIFIED_BY_SUB_COMMAND_GROUPS);
        }
        SUBCASE("Exceed max parameter number.\n"
                "Expect return value is false.")
        {
            AdsBulkBucket bucket(1);
            AdsBulkParameter parameter0;
            auto result = bucket.push(parameter0, AdsSubCommandRequestType::GET_DYNAMIC_HANDLE_FOR_INDIVIDUAL_SYMBOL_BY_NAME);
            REQUIRE(result);

            AdsBulkParameter parameter1;
            result = bucket.push(parameter1, AdsSubCommandRequestType::GET_DYNAMIC_HANDLE_FOR_INDIVIDUAL_SYMBOL_BY_NAME);
            CHECK_FALSE(result);
        }
        SUBCASE("Try to put incompatible request in.\n"
                "Expect return value is false.")
        {
            AdsBulkBucket bucket(2);
            AdsBulkParameter parameter0;
            auto result = bucket.push(parameter0, AdsSubCommandRequestType::GET_DYNAMIC_HANDLE_FOR_INDIVIDUAL_SYMBOL_BY_NAME);
            REQUIRE(result);

            AdsBulkParameter parameter1;
            result = bucket.push(parameter1, AdsSubCommandRequestType::READ_SYMBOL_VALUE);
            CHECK_FALSE(result);
        }
        SUBCASE("Try to put compatible request in.\n"
                "Expect return value is true.")
        {
            AdsBulkBucket bucket(2);
            AdsBulkParameter parameter0;
            auto result = bucket.push(parameter0, AdsSubCommandRequestType::GET_DYNAMIC_HANDLE_FOR_INDIVIDUAL_SYMBOL_BY_NAME);
            REQUIRE(result);

            AdsBulkParameter parameter1;
            result = bucket.push(parameter1, AdsSubCommandRequestType::READ_INFO_OF_INDIVIDUAL_SYMBOL_BY_NAME);
            CHECK(result);
        }
    }
    TEST_CASE("AdsBulkParameter &getParameter(size_t i)\n"
              "AdsSubCommandRequestType getSubCommandRequestType(size_t i) const\n"
              "size_t getNumSubCommands() const\n"
              "size_t getSubCommandInfoSize() const\n"
              "size_t getSubCommandInfoSizeInBytes() const")
    {
        SUBCASE("Push multiple valid parameters with compatible requests.\n"
                "Expect return value is true each time and expect referenced get functions return correct values.")
        {
            AdsBulkBucket bucket(3);
            AdsBulkParameter parameter0;
            auto result = bucket.push(parameter0, AdsSubCommandRequestType::GET_DYNAMIC_HANDLE_FOR_INDIVIDUAL_SYMBOL_BY_NAME);
            REQUIRE(result);
            auto &parameterResult0 = bucket.getParameter(0);
            CHECK(&parameterResult0 == &parameter0);
            auto subCommandResult0 = bucket.getSubCommandRequestType(0);
            CHECK(subCommandResult0 == AdsSubCommandRequestType::GET_DYNAMIC_HANDLE_FOR_INDIVIDUAL_SYMBOL_BY_NAME);
            auto numSubCommandsResult0 = bucket.getNumSubCommands();
            CHECK(numSubCommandsResult0 == 1);
            auto subCommandInfoSizeResult0 = bucket.getSubCommandInfoSize();
            CHECK(subCommandInfoSizeResult0 == 4);
            auto subCommandInfoSizeInBytesResult0 = bucket.getSubCommandInfoSizeInBytes();
            CHECK(subCommandInfoSizeInBytesResult0 == 16);

            AdsBulkParameter parameter1;
            result = bucket.push(parameter1, AdsSubCommandRequestType::READ_INFO_OF_INDIVIDUAL_SYMBOL_BY_NAME);
            REQUIRE(result);
            auto &parameterResult1 = bucket.getParameter(1);
            CHECK(&parameterResult1 == &parameter1);
            auto subCommandResult1 = bucket.getSubCommandRequestType(1);
            CHECK(subCommandResult1 == AdsSubCommandRequestType::READ_INFO_OF_INDIVIDUAL_SYMBOL_BY_NAME);
            auto numSubCommandsResult1 = bucket.getNumSubCommands();
            CHECK(numSubCommandsResult1 == 2);
            auto subCommandInfoSizeResult1 = bucket.getSubCommandInfoSize();
            CHECK(subCommandInfoSizeResult1 == 8);
            auto subCommandInfoSizeInBytesResult1 = bucket.getSubCommandInfoSizeInBytes();
            CHECK(subCommandInfoSizeInBytesResult1 == 32);

            AdsBulkParameter parameter2;
            result = bucket.push(parameter2, AdsSubCommandRequestType::GET_DYNAMIC_HANDLE_FOR_INDIVIDUAL_SYMBOL_BY_NAME);
            REQUIRE(result);
            auto &parameterResult2 = bucket.getParameter(2);
            CHECK(&parameterResult2 == &parameter2);
            auto subCommandResult2 = bucket.getSubCommandRequestType(2);
            CHECK(subCommandResult2 == AdsSubCommandRequestType::GET_DYNAMIC_HANDLE_FOR_INDIVIDUAL_SYMBOL_BY_NAME);
            auto numSubCommandsResult2 = bucket.getNumSubCommands();
            CHECK(numSubCommandsResult2 == 3);
            auto subCommandInfoSizeResult2 = bucket.getSubCommandInfoSize();
            CHECK(subCommandInfoSizeResult2 == 12);
            auto subCommandInfoSizeInBytesResult2 = bucket.getSubCommandInfoSizeInBytes();
            CHECK(subCommandInfoSizeInBytesResult2 == 48);
        }
        SUBCASE("Bucket size is 0.\n"
                "Expect all push attempts fail, getParameter and getSubCommandRequestType throw exceptions,\n"
                "and getNumSubCommands always returns 0.")
        {
            AdsBulkBucket bucket(0);
            auto numSubCommandsInitial = bucket.getNumSubCommands();
            CHECK(numSubCommandsInitial == 0);

            AdsBulkParameter parameter0;
            auto result0 = bucket.push(parameter0, AdsSubCommandRequestType::GET_DYNAMIC_HANDLE_FOR_INDIVIDUAL_SYMBOL_BY_NAME);
            CHECK_FALSE(result0);
            CHECK(bucket.getNumSubCommands() == 0);
            CHECK_THROWS(bucket.getParameter(0));
            CHECK_THROWS(bucket.getSubCommandRequestType(0));

            AdsBulkParameter parameter1;
            auto result1 = bucket.push(parameter1, AdsSubCommandRequestType::READ_INFO_OF_INDIVIDUAL_SYMBOL_BY_NAME);
            CHECK_FALSE(result1);
            CHECK(bucket.getNumSubCommands() == 0);
            CHECK_THROWS(bucket.getParameter(1));
            CHECK_THROWS(bucket.getSubCommandRequestType(1));

            AdsBulkParameter parameter2;
            auto result2 = bucket.push(parameter2, AdsSubCommandRequestType::GET_DYNAMIC_HANDLE_FOR_INDIVIDUAL_SYMBOL_BY_NAME);
            CHECK_FALSE(result2);
            CHECK(bucket.getNumSubCommands() == 0);
            CHECK_THROWS(bucket.getParameter(2));
            CHECK_THROWS(bucket.getSubCommandRequestType(2));
        }
    }
    TEST_CASE("uint8_t *getRawBytesRead()\n"
              "uint32_t getNumRawBytesRead() const\n"
              "uint32_t getNumRawBytesWrite() const\n"
              "uint8_t* getRawBytesWriteValues() const\n"
              "uint32_t getAdsReturnCodeForSubCommand(size_t subCommandIndex) const)\n"
              "void resizeRawBytesWriteForBulkWrite()")
    {
        SUBCASE("Push some valid parameters with sub commands to do a bulk write.\n"
                "Mock the return statuses for the writes and check getting the return codes produces the expected result.\n"
                "Require all push attempts succeed. Check getting the return codes produces the expected result")
        {
            AdsBulkBucket bucket(3);

            auto numRawBytesRead = bucket.getNumRawBytesRead();
            CHECK(numRawBytesRead == 0);

            AdsBulkParameter parameter0;
            parameter0.value.resize(sizeof(uint32_t));
            auto result = bucket.push(parameter0, AdsSubCommandRequestType::WRITE_SYMBOL_VALUE);
            REQUIRE(result);
            AdsBulkParameter parameter1;
            parameter1.value.resize(sizeof(uint32_t));
            result = bucket.push(parameter1, AdsSubCommandRequestType::WRITE_SYMBOL_VALUE);
            REQUIRE(result);
            AdsBulkParameter parameter2;
            parameter2.value.resize(sizeof(uint32_t));
            result = bucket.push(parameter2, AdsSubCommandRequestType::WRITE_SYMBOL_VALUE);
            REQUIRE(result);

            auto numRawBytesWrite = bucket.getNumRawBytesWrite();
            CHECK(numRawBytesWrite == 36);

            auto pointerToRawBytesWrite = bucket.resizeRawBytesWriteForBulkWrite();
            numRawBytesWrite = bucket.getNumRawBytesWrite();
            CHECK(numRawBytesWrite == 48);

            std::vector<uint32_t> values;
            values.push_back(132);
            values.push_back(476);
            values.push_back(1);
            for (size_t i = 0; i < bucket.getNumSubCommands(); i++)
            {
                auto &parameter = bucket.getParameter(i);
                memcpy(parameter.value.data(), &values[i], parameter.value.size());
                memcpy(pointerToRawBytesWrite, parameter.value.data(), parameter.value.size());
                pointerToRawBytesWrite += parameter.value.size();
            }

            numRawBytesRead = bucket.getNumRawBytesRead();
            REQUIRE(numRawBytesRead == 12);

            uint32_t returnCode0 = 0;
            uint32_t returnCode1 = 176;
            uint32_t returnCode2 = 132;

            auto rawBytesRead = bucket.getRawBytesRead();
            memcpy(&rawBytesRead[0], &returnCode0, sizeof(uint32_t));
            memcpy(&rawBytesRead[4], &returnCode1, sizeof(uint32_t));
            memcpy(&rawBytesRead[8], &returnCode2, sizeof(uint32_t));

            auto resultCode = bucket.getAdsReturnCodeForSubCommand(0);
            CHECK(resultCode == returnCode0);
            resultCode = bucket.getAdsReturnCodeForSubCommand(1);
            CHECK(resultCode == returnCode1);
            resultCode = bucket.getAdsReturnCodeForSubCommand(2);
            CHECK(resultCode == returnCode2);

            auto rawBytesWriteValues = bucket.getRawBytesWriteValues();
            uint32_t value;
            memcpy(&value, &rawBytesWriteValues[0], sizeof(uint32_t));
            CHECK(value == values[0]);
            memcpy(&value, &rawBytesWriteValues[4], sizeof(uint32_t));
            CHECK(value == values[1]);
            memcpy(&value, &rawBytesWriteValues[8], sizeof(uint32_t));
            CHECK(value == values[2]);
        }
    }
}