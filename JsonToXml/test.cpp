#include<iostream>
#include<string>
#include"VR_JsonToXml.h"
#include "rapidjson/include/rapidjson/document.h"
#include "rapidjson/include/rapidjson/istreamwrapper.h"
#include "rapidjson/include/rapidjson/stringbuffer.h"
#include "rapidjson/include/rapidjson/writer.h"

#include "pugixml/pugixml.hpp"
#include "pugixml/pugiconfig.hpp"

int main(int argc, char const *argv[])
{
    
    std::string readdate = "{\"project\":\"rapidjson\",\"stars\":10}";
    std::cout<<"1"<<std::endl;
    rapidjson::Document document; 
    std::cout<<"2"<<std::endl; 
    std::string result = "";
    std::cout<<"3"<<std::endl;
    document.Parse<0>(readdate.c_str()); 
    std::cout<<"4"<<std::endl;
    pugi::xml_document m_xdoc;
    std::cout<<"5"<<std::endl;
    VR_JsonToXml m_jsonToxml(m_xdoc);
    std::cout<<"6"<<std::endl;
    m_jsonToxml.transferJson2Xml(document,"xiaojian");
    std::cout<<"7"<<std::endl;
    m_jsonToxml.getReslut(result);
    std::cout<<"8"<<std::endl;
    std::cout<<result<<std::endl;

    return 0;
}


