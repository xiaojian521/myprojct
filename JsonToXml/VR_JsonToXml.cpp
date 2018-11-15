#include "VR_JsonToXml.h"
#include <sstream>
#include <iostream>

VR_JsonToXml::VR_JsonToXml(pugi::xml_document&  xdoc)
    : m_xdoc(xdoc)
{
}

bool VR_JsonToXml::transferJson2Xml(Document &jsonDoc, const std::string &root)
{
    //HasParseError() 是否在最后一次解析中发生了解析错误。
    if (jsonDoc.HasParseError()) {
        return false;
    }
    //将m_xdoc初始化为NULL    
    m_xdoc.load_string("");
    //m_xdoc添加root节点
    pugi::xml_node childNode = m_xdoc.append_child(root.c_str());

    Json2XmlAddNode(childNode, jsonDoc);
    return true;
}

void VR_JsonToXml::getReslut(std::string &result)
{
    std::ostringstream oss;
    m_xdoc.print(oss);
    result = oss.str();
}

void VR_JsonToXml::setRootAttribute(const std::string &key, const std::string &value)
{
    if ("" == key) {
        return;
    }
    pugi::xml_node rootNode = m_xdoc.first_child();
    if (rootNode.empty()) {
        return;
    }
    pugi::xml_attribute att = rootNode.attribute(key.c_str()) ;
    if (att.empty()) {
        rootNode.append_attribute(key.c_str()).set_value(value.c_str());
    }
    else {
        att.set_value(value.c_str());
    }
}

void VR_JsonToXml::Json2XmlAddArray(pugi::xml_node value, rapidjson::Value& jsondoc)
{

}
/**
 * 参数 value 带有根节点的xml
 *     jsondoc 完整的json值
 * 
 * */
void VR_JsonToXml::Json2XmlAddNode(pugi::xml_node value, rapidjson::Value& jsondoc)
{
    for (JsonMemberIter node = jsondoc.MemberBegin(); node != jsondoc.MemberEnd(); ++node) {
        std::string nodeName = (node->name).GetString();
        rapidjson::Type nodeType = (node->value).GetType();
        rapidjson::Value _Object;
        if (nodeName.find_first_of("@") == 0 && (node->value).GetType() == kStringType) {
            std::string nodeAttr = (node->value).GetString();
            nodeName = nodeName.replace(0, 1, "");
            value.append_attribute(nodeName.c_str()).set_value(nodeAttr.c_str());
            continue;
        }
        else if (nodeName.compare("#text") == 0 && (node->value).GetType() == kStringType) {
            std::string nodeAttr = (node->value).GetString();
            value.text().set(nodeAttr.c_str());
            continue;
        }

        switch (nodeType) {
        case kNullType:
            value.append_child(nodeName.c_str()).append_child(pugi::xml_node_type::node_null);
            break;
        case kObjectType:
            _Object = (node->value).GetObject();
            Json2XmlAddNode(value.append_child(nodeName.c_str()), _Object);
            break;
        case kArrayType:
            for (auto& iter : ((node->value).GetArray())) {
                if (iter.GetType() == rapidjson::kStringType) {
                    pugi::xml_document tmpDoc;
                    if (tmpDoc.load_string(iter.GetString()).status == pugi::status_ok) {
                        value.append_child(nodeName.c_str()).append_copy(tmpDoc);
                    }
                    else {
                        value.append_child(nodeName.c_str()).append_child(pugi::xml_node_type::node_pcdata).set_value(iter.GetString());
                    }
                }
                else if (iter.GetType() == rapidjson::kFalseType || iter.GetType() == rapidjson::kTrueType) {
                  value.append_child(nodeName.c_str()).append_child(pugi::xml_node_type::node_pcdata).set_value(std::to_string(iter.GetInt()).c_str());
                }
                else if (iter.GetType() == rapidjson::kNumberType) {
                    std::string result = "";
                    if (iter.IsInt()) {
                        result = std::to_string(iter.GetInt());
                    }
                    else if (iter.IsUint()) {
                        result =  std::to_string(iter.GetUint());
                    }
                    else if (iter.IsInt64()) {
                        result = std::to_string(iter.GetInt64());
                    }
                    else if (iter.IsUint64()) {
                        result = std::to_string(iter.GetUint64());
                    }
                    else if (iter.IsDouble()) {
                        result = std::to_string(iter.GetDouble());
                    }
                    value.append_child(nodeName.c_str()).append_child(pugi::xml_node_type::node_pcdata).set_value(result.c_str());
                }
                else if (iter.GetType() == rapidjson::kObjectType) {
                    _Object = iter.GetObject();
                    Json2XmlAddNode(value.append_child(nodeName.c_str()), _Object);
                }
            }
            break;
        case kTrueType:
        case kFalseType:
            if ((node->value).GetBool()) {
                value.append_child(nodeName.c_str()).append_child(pugi::xml_node_type::node_pcdata).set_value("true");
            }
            else {
                value.append_child(nodeName.c_str()).append_child(pugi::xml_node_type::node_pcdata).set_value("false");
            }
            break;
        case kStringType:
        {
            pugi::xml_document tmpDoc;
            if (tmpDoc.load_string((node->value).GetString()).status == pugi::status_ok) {
                value.append_child(nodeName.c_str()).append_copy(tmpDoc.first_child());
            }
            else {
                value.append_child(nodeName.c_str()).append_child(pugi::xml_node_type::node_pcdata).set_value((node->value).GetString());
            }
        }
            break;
        case kNumberType:
        {
            std::string result = "";
            if (node->value.IsInt()) {
                result = std::to_string(node->value.GetInt());
            }
            else if (node->value.IsUint()) {
                result =  std::to_string(node->value.GetUint());
            }
            else if (node->value.IsInt64()) {
                result = std::to_string(node->value.GetInt64());
            }
            else if (node->value.IsUint64()) {
                result = std::to_string(node->value.GetUint64());
            }
            else if (node->value.IsDouble()) {
                result = std::to_string(node->value.GetDouble());
            }
            value.append_child(nodeName.c_str()).append_child(pugi::xml_node_type::node_pcdata).set_value(result.c_str());
        }
            break;
        default:
            break;
        }
    }
}
