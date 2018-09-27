#include <iostream>
#include <cstdio>



#include "rapidjson/prettywriter.h"
#include "VR_Engin/VR_EnginReqAnalysize.h"
#include  "VR_Engin/VR_AlexaInterfaceDef.h"

VR_EnginReqAnalysize::VR_EnginReqAnalysize(const std::__cxx11::string &orgJson)
    : m_head("")
    , m_data(nullptr)
{
    parseData(orgJson);
}

VR_EnginReqAnalysize::~VR_EnginReqAnalysize()
{
    if (nullptr != m_data) {
        delete m_data;
        m_data = nullptr;
    }
}

std::__cxx11::string VR_EnginReqAnalysize::requstType()
{
    return m_head.getRequestType();
}

VR_EnginReqDataType *VR_EnginReqAnalysize::requestParams()
{
    return m_data;
}

void VR_EnginReqAnalysize::parseData(const std::__cxx11::string &orgJson)
{
    rapidjson::Document orgDoc;
    if (orgDoc.Parse<0>(orgJson.c_str()).HasParseError()) {
        return;
    }
    if (orgDoc.HasMember("requestType")) {
        if (orgDoc["requestType"].IsString()) {
            m_head.setRequestType(orgDoc["requestType"].GetString());
        }
    }
    if (orgDoc.HasMember("requestParams")) {
        if (orgDoc["requestParams"].IsObject()) {
            std::string strType = m_head.getRequestType();
            if ("changeSetting" == strType) {
                parseReqChangeSetting(orgDoc["requestParams"]);
            }
            else if ("adjustVolume" == strType) {
                parseReqAdjVolume(orgDoc["requestParams"]);
            }
            else if ("setMute" == strType) {
                parseReqSetMute(orgDoc["requestParams"]);
            }
            else if ("sendFile" == strType) {
                parseReqSendFile(orgDoc["requestParams"]);
            }
        }
    }
}

void VR_EnginReqAnalysize::parseReqSendFile(const rapidjson::Value &obj)
{
    if (nullptr != m_data) {
        delete m_data;
        m_data = nullptr;
    }
    if (obj.HasMember("file")) {
        if (obj["file"].IsString()) {
            m_data = new VR_EnginReqDataParSendFile(obj["file"].GetString());
        }
    }
}

void VR_EnginReqAnalysize::parseReqSetMute(const rapidjson::Value &obj)
{
    if (nullptr != m_data) {
        delete m_data;
        m_data = nullptr;
    }
    if (obj.HasMember("type") && obj.HasMember("mute")) {
        if (obj["type"].IsInt() && obj["mute"].IsBool()) {
            m_data = new VR_EnginReqDataParSetMute(obj["type"].GetInt(), obj["mute"].GetBool());
        }
    }
}

void VR_EnginReqAnalysize::parseReqAdjVolume(const rapidjson::Value &obj)
{
    if (nullptr != m_data) {
        delete m_data;
        m_data = nullptr;
    }
    if (obj.HasMember("type") && obj.HasMember("delta")) {
        if (obj["type"].IsInt() && obj["delta"].IsInt()) {
            m_data = new VR_EnginReqDataParAdjVol(obj["type"].GetInt(), obj["delta"].GetInt());
        }
    }
}

void VR_EnginReqAnalysize::parseReqChangeSetting(const rapidjson::Value &obj)
{
    if (nullptr != m_data) {
        delete m_data;
        m_data = nullptr;
    }
    if (obj.HasMember("key") && obj.HasMember("value")) {
        if (obj["key"].IsString() && obj["value"].IsString()) {
            m_data = new VR_EnginReqDataParChangeSetting(obj["key"].GetString(), obj["value"].GetString());
        }
    }
}

