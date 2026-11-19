#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "AppsFlyer/AppsFlyerDeepLinkData.h"
#include "AppsFlyer/AppsFlyerX.h"
#include "cocos2d.h"
#include "ui/UIScrollView.h"

class HelloWorld : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();

    virtual bool init() override;
    virtual void onExit() override;

    /// Append a line to the on-screen results panel (safe from AppsFlyer callbacks).
    static void appendEventLog(const std::string& line);
    static void appendAfResult(const char* action, const AppsFlyerResult& result);
    static void appendDeepLink(const appsflyer::DeepLinkData& data);
    static void appendConversionData(const char* title, const cocos2d::ValueMap& map);

    CREATE_FUNC(HelloWorld);

private:
    static HelloWorld* s_activeScene;
    static cocos2d::Label* s_eventLogLabel;
    static cocos2d::ui::ScrollView* s_eventScroll;
    static std::string s_eventText;

    static void refreshEventLogView();
    static void appendEventLogInternal(const std::string& line);
};

#endif // __HELLOWORLD_SCENE_H__