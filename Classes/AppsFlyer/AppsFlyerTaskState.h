#ifndef AppsFlyerTaskState_h
#define AppsFlyerTaskState_h

#include "AppsFlyerResult.h"

#include <mutex>
#include <utility>
#include <vector>

struct AppsFlyerTaskState {
    std::mutex mutex;
    bool done = false;
    AppsFlyerResult result;
    std::vector<AppsFlyerCompletion> pending;

    void addCompletion(AppsFlyerCompletion callback);
    void finish(const AppsFlyerResult& finalResult);
};

#endif
