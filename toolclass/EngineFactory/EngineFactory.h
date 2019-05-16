
#ifndef VR_ENGINEFACTORY_H
#define VR_ENGINEFACTORY_H

#ifndef __cplusplus
# error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include "VR_Def.h"

class VR_EngineIF;
class VR_EngineListenerIF;

/**
 * @brief The VR_EngineFactory class
 *
 * class declaration
 */
class VR_EngineFactory
{
public:
    static bool LoadEngineLibrary(const std::string& strEngineType);
    static VR_EngineIF* CreateEngineInstance(const std::string& strEngineType, VR_EngineListenerIF* listener,
        const std::string& strPathConfig = "", const std::string& strPathInput = "", const std::string& strLogLevel = "");
    static bool DestroyEngineInstance(const std::string& strEngineType, VR_EngineIF* engine=NULL);
};

#endif // VR_ENGINEFACTORY_H
/* EOF */
