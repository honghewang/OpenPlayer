//
//  OPFilterShowingFilter.hpp
//  OpenPlayer
//
//  Created by 王宏鹤 on 2024/8/25.
//

#ifndef OPFilterShowingFilter_hpp
#define OPFilterShowingFilter_hpp

#include <stdio.h>
#include "OPFilterInOut.hpp"

class OPFilterShowingFilter : public OPFilterInOut {
protected:
    void render() override;
public:
    std::function<void(int texture)> refreshFunc;
};

#endif /* OPFilterShowingFilter_hpp */
