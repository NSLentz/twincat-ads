#include "AdsRequestType.h"

std::string adsBulkRequestTypeToString(AdsBulkRequestType req)
{
    switch (req)
    {
        case AdsBulkRequestType::UNDEFINED:
            return "UNDEFINED";
        case AdsBulkRequestType::DO_ACTIONS_SPECIFIED_BY_SUB_COMMAND_GROUPS:
            return "DO_ACTIONS_SPECIFIED_BY_SUB_COMMAND_GROUPS";
        case AdsBulkRequestType::READ_MULTIPLE_SYMBOL_VALUES:
            return "READ_MULTIPLE_SYMBOL_VALUES";
        case AdsBulkRequestType::WRITE_MULTIPLE_SYMBOL_VALUES:
            return "WRITE_MULTIPLE_SYMBOL_VALUES";
        case AdsBulkRequestType::ADD_MULTIPLE_SYMBOL_NOTIFICATIONS:
            return "ADD_MULTIPLE_SYMBOL_NOTIFICATIONS";
        case AdsBulkRequestType::DEL_MULTIPLE_SYMBOL_NOTIFICATIONS:
            return "DEL_MULTIPLE_SYMBOL_NOTIFICATIONS";
        default:
            return "UNDEFINED";
    }
}

std::string adsSubCommandRequestTypeToString(AdsSubCommandRequestType req)
{
    switch (req)
    {
        case AdsSubCommandRequestType::UNDEFINED:
            return "UNDEFINED";
        case AdsSubCommandRequestType::GET_DYNAMIC_HANDLE_FOR_INDIVIDUAL_SYMBOL_BY_NAME:
            return "GET_DYNAMIC_HANDLE_FOR_INDIVIDUAL_SYMBOL_BY_NAME";
        case AdsSubCommandRequestType::READ_INFO_OF_INDIVIDUAL_SYMBOL_BY_NAME:
            return "READ_INFO_OF_INDIVIDUAL_SYMBOL_BY_NAME";
        case AdsSubCommandRequestType::READ_SYMBOL_VALUE:
            return "READ_SYMBOL_VALUE";
        case AdsSubCommandRequestType::WRITE_SYMBOL_VALUE:
            return "WRITE_SYMBOL_VALUE";
        case AdsSubCommandRequestType::ADD_SYMBOL_NOTIFICATION:
            return "ADD_SYMBOL_NOTIFICATION";
        case AdsSubCommandRequestType::DEL_SYMBOL_NOTIFICATION:
            return "DEL_SYMBOL_NOTIFICATION";
        default:
            return "UNDEFINED";
    }
}