//
//  OPPtrManager.cpp
//  OverallPlayer
//
//  Created by 王宏鹤 on 2024/6/26.
//

#include "OPPtrManager.hpp"

static std::mutex objMutex;
static std::unordered_set<void *> objSet;


void OPPtrManager::registerObj(void *obj) {
    std::lock_guard<std::mutex> locker(objMutex);
    objSet.insert(obj);
}

void OPPtrManager::unregisterObj(void *obj) {
    std::lock_guard<std::mutex> locker(objMutex);
    objSet.erase(obj);
}

void OPPtrManager::getState(void *obj, std::function<void(bool isReleased)> func) {
    std::lock_guard<std::mutex> locker(objMutex);
    if (func != nullptr) {
        func(objSet.find(obj) == objSet.end());
    }
}

bool OPPtrManager::isReleased(void *obj) {
    std::lock_guard<std::mutex> locker(objMutex);
    return objSet.find(obj) == objSet.end();
}
