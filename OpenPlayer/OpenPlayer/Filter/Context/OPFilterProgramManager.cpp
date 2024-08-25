//
//  OPFilterProgramManager.cpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/10.
//

#include "OPFilterProgramManager.hpp"
#include "OPFilterShaderHeader.hpp"


OPFilterProgramManager::OPFilterProgramManager() {
    
}

OPFilterProgramManager::~OPFilterProgramManager() {
    
}

std::shared_ptr<OPFilterProgram> OPFilterProgramManager::programFromCache(std::string vShaderKey, std::string fShaderKey) {
    std::string key = vShaderKey + fShaderKey;
    if (programMap.find(key) == programMap.end()) {
        std::string vShader = OPVertexShaderMap[vShaderKey];
        std::string fShader = OPFragmentShaderMap[fShaderKey];
        if (vShader.size() > 0 && fShader.size() > 0) {
            programMap[key] = std::make_shared<OPFilterProgram>(vShader, fShader);
        }
    }
    return programMap[key];
}
