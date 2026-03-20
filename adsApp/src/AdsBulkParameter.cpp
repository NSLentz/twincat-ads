#include "AdsBulkParameter.h"

AdsBulkParameter::AdsBulkParameter(const AmsAddr &amsServerAddr)
{
    this->amsServerAddr.netId.b[0] = amsServerAddr.netId.b[0];
    this->amsServerAddr.netId.b[1] = amsServerAddr.netId.b[1];
    this->amsServerAddr.netId.b[2] = amsServerAddr.netId.b[2];
    this->amsServerAddr.netId.b[3] = amsServerAddr.netId.b[3];
    this->amsServerAddr.netId.b[4] = amsServerAddr.netId.b[4];
    this->amsServerAddr.netId.b[5] = amsServerAddr.netId.b[5];
    this->amsServerAddr.port = amsServerAddr.port;
}