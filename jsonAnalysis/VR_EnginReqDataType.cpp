#include "VR_Engin/VR_EnginReqDataType.h"

VR_EnginReqDataHead::VR_EnginReqDataHead(const std::__cxx11::string &requestType)
    : m_requestType(requestType)
{
}


std::__cxx11::string VR_EnginReqDataHead::getRequestType()
{
    return m_requestType;
}

void VR_EnginReqDataHead::setRequestType(const std::__cxx11::string &type)
{
    m_requestType = type;
}

VR_EnginReqDataParChangeSetting::VR_EnginReqDataParChangeSetting(const std::__cxx11::string &key,
                                                                 const std::__cxx11::string &value)
    : m_key(key)
    , m_value(value)
{
}

void VR_EnginReqDataParChangeSetting::getData(std::__cxx11::string &key, std::__cxx11::string &value)
{
   key = m_key;
   value = m_value;
}

VR_EnginReqDataParAdjVol::VR_EnginReqDataParAdjVol(int type,
                                                   char delta)
    : m_type(type)
    , m_delta(delta)
{

}

void VR_EnginReqDataParAdjVol::getData(int &type, char &delta)
{
    type = m_type;
    delta = m_delta;
}

VR_EnginReqDataParSetMute::VR_EnginReqDataParSetMute(int type,
                                                 bool mute)
    : m_type(type)
    , m_mute(mute)
{
}

void VR_EnginReqDataParSetMute::getData(int &type, bool &mute)
{
    type = m_type;
    mute = m_mute;
}

VR_EnginReqDataParSendFile::VR_EnginReqDataParSendFile(const std::__cxx11::string &file)
    : m_file(file)
{
}

std::__cxx11::string VR_EnginReqDataParSendFile::getData()
{
    return m_file;
}
