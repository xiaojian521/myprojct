
#include "stdafx.h"
#include <dlfcn.h>
#include <string>
#include <fstream>
#include <dirent.h>

#include "VR_EngineFactory.h"

#ifndef VR_ENGINEIF_H
#    include "VR_EnginIF.h"
#endif

#ifndef VR_LOG_H
#   include "VR_Log.h"
#endif

#ifndef VR_CONFIGUREIF_H
#    include "VR_ConfigureIF.h"
#endif

#include "MEM_map.h"

#define VR_CREATE_ENGINE    "createVRAlexaEng"     // "VR_CreateEngine"
#define VR_DESTROY_ENGINE   "destoryVRAlexaEng"    // "VR_DestroyEngine"
#define VR_CREATE_BAIDU_POC_ENGINE    "createVRBaiduPocEng"     // "VR_CreateEngine"
#define VR_DESTROY_BAIDU_POC_ENGINE   "destoryVRBaiduPocEng"    // "VR_DestroyEngine"
typedef VR_EngineIF* (*fnCreateAlexaEngine)(const char*, const char*, const char*, VR_EngineListenerIF*);
typedef VR_EngineIF* (*fnCreateBaiduPocEngine)(VR_EngineListenerIF*);
typedef VR_EngineIF* (*fnCreateOtherEngine)(VR_EngineListenerIF*);
typedef void (*fnDestroyEngine)();
typedef void (*fnDestroyBaiduPocEngine)(VR_EngineIF*);

const VoiceMap<std::string, std::string>::type::value_type init_value[] =
{
    VoiceMap<std::string, std::string>::type::value_type("Alexa", "libVREnginAvs.so"),
    VoiceMap<std::string, std::string>::type::value_type("Baidu", "libvr_engine_Baidu.so"),
    VoiceMap<std::string, std::string>::type::value_type("Google", "libvr_engine_Google.so"),
    VoiceMap<std::string, std::string>::type::value_type("Baidu_POC", "libvr_engine_BaiduPoc.so")
};

const static VoiceMap<std::string, std::string>::type VR_EngineLibraryNameMap(init_value, init_value + 4);
static VoiceMap<std::string, void*>::type VR_EngineLibraryHandleMap;

static void* VR_LoadLibrary(const std::string& strLibraryName)
{
    VR_LOGD_FUNC();

    if (strLibraryName.empty()) {
        {
            static bool errorLogMark = false;
            if (!errorLogMark) {
                errorLogMark = true;
                VR_ERROR(VOICE_VR_ERR_INVALID_PARAM, VR_DE_OPTIONDATA_NONE, "Load Engine lib failed, strLibraryName is NULL\n");
            }
        }
        return NULL;
    }
    void* handle = NULL;
    handle = dlopen(strLibraryName.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    VR_LOGI("dlopen engine library %s successfully !", strLibraryName.c_str());
    if (!handle) {
        VR_LOGI("Load engine library failed, strLibraryName=%s, errmsg=%s\n", strLibraryName.c_str(), dlerror());
        return NULL;
    }
    VR_LOGI("Load engine library %s successfully !", strLibraryName.c_str());
    return handle;
}

static VR_EngineIF* VR_LoadEngineInstance(const std::string& strEngineType, VR_EngineListenerIF* listener,
        const std::string& strPathConfig, const std::string& strPathInput, const std::string& strLogLevel)
{
    VR_LOGD_FUNC();

    VoiceMap<std::string, void *>::iterator it;
    it = VR_EngineLibraryHandleMap.find(strEngineType);
    if (it == VR_EngineLibraryHandleMap.end()) {
        {
            static bool errorLogMark = false;
            if (!errorLogMark) {
                errorLogMark = true;
                VR_ERROR(VOICE_VR_ERR_OUT_RANGE, VR_DE_OPTIONDATA_NONE, "can not create engine: %s", strEngineType.c_str());
            }
        }
        VR_LOGI("VR_EngineLibraryHandleMap can't find");
        return NULL;
    }

    void* handle = it->second;
    if (!handle) {
        VR_LOGI("handle is NULL, need dlopen() first !");
    }

    VR_EngineIF* pEngineIF = NULL;
    VR_LOGI("choose engine, type=%s",strEngineType.c_str());
    if ("Alexa" == strEngineType) {
        fnCreateAlexaEngine funcAlexa = (fnCreateAlexaEngine)dlsym(handle, VR_CREATE_ENGINE);
        if (!funcAlexa) {
            {
                static bool errorLogMark = false;
                if (!errorLogMark) {
                    errorLogMark = true;
                    VR_ERROR(VOICE_VR_ERR_INVALID_OP, VR_DE_OPTIONDATA_NONE, "dlsym failed, funcAlexa = %s", VR_CREATE_ENGINE);
                }
            }
            return NULL;
        }

        pEngineIF = (*funcAlexa)(strPathConfig.c_str(), strPathInput.c_str(), strLogLevel.c_str(), listener);
    }
    else if ("Baidu_POC" == strEngineType) {
        VR_LOGI("Baidu_POC start create");
        fnCreateBaiduPocEngine funcBaiduPoc = (fnCreateBaiduPocEngine)dlsym(handle, VR_CREATE_BAIDU_POC_ENGINE);
        if (!funcBaiduPoc) {
            {
                static bool errorLogMark = false;
                if (!errorLogMark) {
                    errorLogMark = true;
                    VR_ERROR(VOICE_VR_ERR_INVALID_OP, VR_DE_OPTIONDATA_NONE, "dlsym failed, funcBaiduPoc = %s", VR_CREATE_BAIDU_POC_ENGINE);
                }
            }
            VR_LOGI("Baidu_POC create fail");
            return NULL;
        }

        pEngineIF = (*funcBaiduPoc)(listener);
    }
    else {
        VR_LOGI("undefine engine enter");
        fnCreateOtherEngine funcOther = (fnCreateOtherEngine)dlsym(handle, VR_CREATE_ENGINE);
        if (!funcOther) {
            {
                static bool errorLogMark = false;
                if (!errorLogMark) {
                    errorLogMark = true;
                    VR_ERROR(VOICE_VR_ERR_INVALID_OP, VR_DE_OPTIONDATA_NONE, "dlsym failed, funcOther = %s", VR_CREATE_ENGINE);
                }
            }
            return NULL;
        }

        pEngineIF = (*funcOther)(listener);
    }

    if (!pEngineIF) {
        {
            static bool errorLogMark = false;
            if (!errorLogMark) {
                errorLogMark = true;
                VR_ERROR(VOICE_VR_ERR_INVALID_OP, VR_DE_OPTIONDATA_NONE, "create engine instance failed, func = %s", VR_CREATE_ENGINE);
            }
        }
        VR_LOGI("create engine fail");
        return NULL;
    }

    VR_LOGI("####################### create engine %s ok ############################", strEngineType.c_str());
    return pEngineIF;
}

bool VR_EngineFactory::LoadEngineLibrary(const std::string& strEngineType)
{
    VR_LOGD_FUNC();

    if (strEngineType.empty()) {
        return false;
    }

    VoiceMap<std::string, std::string>::const_iterator it;
    it = VR_EngineLibraryNameMap.find(strEngineType);
    if (it == VR_EngineLibraryNameMap.cend()) {
        {
            static bool errorLogMark = false;
            if (!errorLogMark) {
                errorLogMark = true;
                VR_ERROR(VOICE_VR_ERR_OUT_RANGE, VR_DE_OPTIONDATA_NONE, "can not find engine: %s so", strEngineType.c_str());
            }
        }
        return false;
    }

    std::string strLibraryName = it->second;
    void* handle = VR_LoadLibrary(strLibraryName);
    if (NULL == handle) {
        return false;
    }

    VR_EngineLibraryHandleMap[it->first] = handle;
    return true;
}
//返回创建Engine的对象
VR_EngineIF* VR_EngineFactory::CreateEngineInstance(const std::string& strEngineType, VR_EngineListenerIF* listener,
        const std::string& strPathConfig, const std::string& strPathInput, const std::string& strLogLevel)
{
    VR_LOGD_FUNC();

    if (strEngineType.empty()) {
        return NULL;
    }

    VR_EngineIF* pEngineIF = VR_LoadEngineInstance(strEngineType, listener, strPathConfig, strPathInput, strLogLevel);
    if (NULL == pEngineIF) {
        VR_LOGE("Create engine %s failed !", strEngineType.c_str());
    }
    return pEngineIF;
}

bool VR_EngineFactory::DestroyEngineInstance(const std::string& strEngineType, VR_EngineIF* engine)
{
    VR_LOGD_FUNC();

    if (strEngineType.empty()) {
        return false;
    }

    VoiceMap<std::string, void *>::iterator it;
    it = VR_EngineLibraryHandleMap.find(strEngineType);
    if (it == VR_EngineLibraryHandleMap.end()) {
        {
            static bool errorLogMark = false;
            if (!errorLogMark) {
                errorLogMark = true;
                VR_ERROR(VOICE_VR_ERR_OUT_RANGE, VR_DE_OPTIONDATA_NONE, "can not destroy engine: %s", strEngineType.c_str());
            }
        }
        return false;
    }

    void* handle = it->second;
    if (!handle) {
        VR_LOGI("handle is NULL, need dlopen() first !");
    }

    if ("Baidu_POC" == strEngineType) {
        fnDestroyBaiduPocEngine func = (fnDestroyBaiduPocEngine)dlsym(handle, VR_DESTROY_BAIDU_POC_ENGINE);
        if (!func) {
            {
                static bool errorLogMark = false;
                if (!errorLogMark) {
                    errorLogMark = true;
                    VR_ERROR(VOICE_VR_ERR_INVALID_OP, VR_DE_OPTIONDATA_NONE, "dlsym failed, func = %s", VR_DESTROY_BAIDU_POC_ENGINE);
                }
            }
            return false;
        }

        (*func)(engine);
    } else {
        fnDestroyEngine func = (fnDestroyEngine)dlsym(handle, VR_DESTROY_ENGINE);
        if (!func) {
            {
                static bool errorLogMark = false;
                if (!errorLogMark) {
                    errorLogMark = true;
                    VR_ERROR(VOICE_VR_ERR_INVALID_OP, VR_DE_OPTIONDATA_NONE, "dlsym failed, func = %s", VR_DESTROY_ENGINE);
                }
            }
            return false;
        }

        (*func)();
    }

    dlclose(it->second);
    VR_EngineLibraryHandleMap.erase(it);

    VR_LOGI("destroy engine %s ok", strEngineType.c_str());
    return true;
}

/* EOF */
