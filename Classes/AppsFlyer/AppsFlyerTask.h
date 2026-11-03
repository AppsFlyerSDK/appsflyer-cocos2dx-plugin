#ifndef AppsFlyerTask_h
#define AppsFlyerTask_h

#include "AppsFlyerResult.h"

#include <memory>

#include "AppsFlyerTaskState.h"

class AppsFlyerTask {
public:
    AppsFlyerTask();
    explicit AppsFlyerTask(std::shared_ptr<AppsFlyerTaskState> state);

    void whenComplete(AppsFlyerCompletion callback);
    bool isDone() const;

    std::shared_ptr<AppsFlyerTaskState> stateForInternalUse() const { return state_; }

private:
    std::shared_ptr<AppsFlyerTaskState> state_;
};

#endif
