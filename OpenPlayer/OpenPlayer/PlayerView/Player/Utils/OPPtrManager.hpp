//
//  OPPtrManager.hpp
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/26.
//

#ifndef OPPtrManager_hpp
#define OPPtrManager_hpp

#include <stdio.h>
#include <thread>
#include <unordered_set>
#include <functional>

class OPPtrManager {
private:
    OPPtrManager() {}
    ~OPPtrManager() {}
    
public:
    static void registerObj(void *obj);
    static void unregisterObj(void *obj);
    // 使用过程中先锁住，防止过程中被释放
    static void getState(void *obj, std::function<void(bool isReleased)> fun);
    // 两个版本
    static bool isReleased(void *obj);
};

#endif /* OPPtrManager_hpp */
