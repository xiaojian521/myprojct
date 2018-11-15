/**http://rapidjson.org/zh-cn/classrapidjson_1_1_generic_member_iterator.html
 * rapidjson 使用文档
 * */

/**https://pugixml.org/docs/manual.html
 * pugixml 使用文档
 * */

#ifndef VR_JSONTOXML_H
#define VR_JSONTOXML_H

#include <string>
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "pugixml/pugixml.hpp"

using namespace rapidjson;

class VR_JsonToXml
{
public:
    //json成员变量的迭代器
    typedef GenericMemberIterator<false, UTF8<>, MemoryPoolAllocator<> >::Iterator JsonMemberIter;
    //构造方法传入xml_documet对象
    VR_JsonToXml(pugi::xml_document&  xdoc);
    //为xml对象添加root节点
    bool transferJson2Xml(Document& jsonDoc, const std::string& root);
    //获得xml的对象保存到result里面
    void getReslut(std::string& result);
    //为xml设置根节点的属性
    void setRootAttribute(const std::string &key, const std::string& value);
private:
    //将json转换成xml的过程
    void Json2XmlAddNode(pugi::xml_node value, rapidjson::Value& node);
    void Json2XmlAddArray(pugi::xml_node value, rapidjson::Value& jsondoc);

private:
    pugi::xml_document&  m_xdoc;
};

#endif // VR_JSONTOXML_H

