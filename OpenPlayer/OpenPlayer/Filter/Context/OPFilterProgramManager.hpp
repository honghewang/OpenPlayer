//
//  OPFilterProgramManager.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/7/10.
//

#ifndef OPFilterProgramManager_hpp
#define OPFilterProgramManager_hpp

#include <stdio.h>
#include <map>
#include <string>
#include "OPFilterProgram.hpp"
#include "OPFilterShaderHeader.hpp"

class OPFilterProgramManager {
private:
    std::map<std::string, std::shared_ptr<OPFilterProgram>> programMap;
public:
    OPFilterProgramManager();
    ~OPFilterProgramManager();
    
    std::shared_ptr<OPFilterProgram> programFromCache(std::string vShaderKey, std::string fShaderKey);
    
};

#endif /* OPFilterProgramManager_hpp */
