//
//  OPFilterShaderHeader.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/20.
//

#ifndef OPFilterShaderHeader_hpp
#define OPFilterShaderHeader_hpp

#include <stdio.h>
#include <string>
#include <map>

extern std::string OPFilterVertexShaderString;
extern std::string OPFilterPointVertexShaderString;
extern std::string OPFilterPointFragmentShaderString;
extern std::string OPFilterPassthroughFragmentShaderString;

extern std::map<std::string, std::string> OPVertexShaderMap;

extern std::map<std::string, std::string> OPFragmentShaderMap;

#endif /* OPFilterShaderHeader_hpp */
