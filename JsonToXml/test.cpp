#include<iostream>
#include<string>
#include"VR_JsonToXml.h"
#include "rapidjson/include/rapidjson/document.h"
#include "rapidjson/include/rapidjson/istreamwrapper.h"
#include "rapidjson/include/rapidjson/stringbuffer.h"
#include "rapidjson/include/rapidjson/writer.h"

#include "pugixml/pugixml.hpp"
#include "pugixml/pugiconfig.hpp"
#include "pugixml/pugixml.cpp"
//还不能运行
int main(int argc, char const *argv[])
{
    
    std::string readdate = "{\"project\":\"rapidjson\",\"stars\":10}";
    rapidjson::Document document;
    std::string result = "";
    document.Parse<0>(readdate.c_str()); 
    pugi::xml_document m_xdoc;
    VR_JsonToXml* m_jsonToxml = new VR_JsonToXml(m_xdoc);
    m_jsonToxml->transferJson2Xml(document,"xiaojian");
    m_jsonToxml->getReslut(result);
    std::cout<<result<<std::endl;
    delete m_jsonToxml;

    return 0;
}


