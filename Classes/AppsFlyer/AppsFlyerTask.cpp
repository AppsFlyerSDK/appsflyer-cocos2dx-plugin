#include "AppsFlyerTask.h"

#include "AppsFlyerTaskState.h"

void AppsFlyerTaskState::addCompletion(AppsFlyerCompletion callback) {
    if (!callback) {
        return;
    }
    AppsFlyerResult snapshot;
    bool alreadyDone = false;
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (done) {
            alreadyDone = true;
            snapshot = result;
        } else {
            pending.push_back(std::move(callback));
        }
    }
    if (alreadyDone) {
        callback(snapshot);
    }
}

void AppsFlyerTaskState::finish(const AppsFlyerResult& finalResult) {
    std::vector<AppsFlyerCompletion> callbacks;
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (done) {
            return;
        }
        done = true;
        result = finalResult;
        callbacks = std::move(pending);
    }
    for (auto& cb : callbacks) {
        if (cb) {
            cb(finalResult);
        }
    }
}

AppsFlyerTask::AppsFlyerTask() : state_(std::make_shared<AppsFlyerTaskState>()) {}

AppsFlyerTask::AppsFlyerTask(std::shared_ptr<AppsFlyerTaskState> state)
    : state_(state ? std::move(state) : std::make_shared<AppsFlyerTaskState>()) {}

void AppsFlyerTask::whenComplete(AppsFlyerCompletion callback) {
    if (state_) {
        state_->addCompletion(std::move(callback));
    }
}

bool AppsFlyerTask::isDone() const {
    if (!state_) {
        return true;
    }
    std::lock_guard<std::mutex> lock(state_->mutex);
    return state_->done;
}
