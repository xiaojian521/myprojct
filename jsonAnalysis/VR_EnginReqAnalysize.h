#ifndef VR_ENGINREQANALYSIZE_H
#define VR_ENGINREQANALYSIZE_H

#include <string>
#include "rapidjson/document.h"
#include "rapidjson/encodings.h"
#include "VR_Engin/VR_AlexaInterfaceDef.h"
#include "VR_Engin/VR_EnginReqDataType.h"

class VR_EnginReqAnalysize
{
public:
    VR_EnginReqAnalysize(const std::string &orgJson);
    ~VR_EnginReqAnalysize();
    std::string requstType();
    VR_EnginReqDataType *requestParams();

private:
    void parseData(const std::string &orgJson);
    void parseReqSendFile(const rapidjson::Value &obj);
    void parseReqSetMute(const rapidjson::Value &obj);
    void parseReqAdjVolume(const rapidjson::Value &obj);
    void parseReqChangeSetting(const rapidjson::Value &obj);

    VR_EnginReqDataHead     m_head;
    VR_EnginReqDataType    *m_data;

};

#endif // VR_ENGINREQANALYSIZE_H
