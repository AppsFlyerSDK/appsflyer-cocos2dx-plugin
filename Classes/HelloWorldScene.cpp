#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

#include <iostream>
#include <cstdlib>
#include "ui/CocosGUI.h"
#include "AppsFlyer/AppsFlyerX.h"
#include "../cocos2d/cocos/ui/UIWidget.h"
#include "../cocos2d/cocos/base/CCValue.h"

using namespace ui;

Scene* HelloWorld::createScene() {
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init()) {
        return false;
    }


    auto size = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto logo = ImageView::create("logo.png");
    
    // auto label = Label::createWithTTF("AppsFlyer", "fonts/Marker Felt.ttf", 24);
    logo->setPosition(Vec2(origin.x + size.width/2, origin.y + size.height - logo->getContentSize().height));
    
    addChild(logo, 1);
    
    auto button = Button::create("CloseNormal.png", "CloseSelected.png", "");
    button->setTitleText("Track event");
    button->setPosition(Vec2(origin.x + 40, origin.y + size.height - 100));
    // todo: change positioning logic
    button->getTitleLabel()->setPosition(45, 10);
    button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
        switch (type) {
            case Widget::TouchEventType::ENDED: {
                std::cout << "Track event raised" << std::endl;

                AppsFlyerX::trackEvent(AFEventPurchase, {
                        { AFEventParamContentId, Value({Value("12344"), Value("98844"), Value("39944")})},
                        { AFEventParamCurrency, Value({Value(20), Value(11), Value(61)})},
                        { AFEventParamPrice, Value({Value(25), Value(50), Value(10)})},
                        { AFEventParamContentType, Value("ELECTRONIC")},
                        { AFEventParamCurrency, Value("USD")},
                        {AFEventParamRevenue, cocos2d::Value("10.67")}
                });

//                ValueMap map;
//                map["key1"] = "value1";
//                map["key2"] = 1;
//                map["key3"] = true;
//                map["key4"] = 3.0;
//                AppsFlyerX::trackEvent("test_event", map);

                AppsFlyerX::setCustomerUserID("CustomId");

//                ValueMap test_map;
//                test_map["key1"] = "value1";
//                test_map["key2"] = "eee";
//                test_map["key3"] = "sssss";
//                AppsFlyerX::validateAndTrackInAppPurchase("public", "sig", "pd", "price", "curr", test_map);
                break;
            }
            default:
                break;
        }
    });
    auto exitButton = Button::create("CloseNormal.png", "CloseSelected.png", "");
    exitButton->setTitleText("Quit");
    exitButton->setPosition(Vec2(origin.x + 40, origin.y + size.height - 120));
    // todo: change positioning logic
    exitButton->getTitleLabel()->setPosition(30, 10);
    exitButton->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type){
        switch (type) {
            case Widget::TouchEventType::ENDED:
                Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                exit(0);
#endif
                break;
            default:
                break;
        }
    });
    auto validatePurchase = Button::create("CloseNormal.png", "CloseSelected.png", "");
    validatePurchase->setTitleText("Validate");
    validatePurchase->setPosition(Vec2(origin.x + 40, origin.y + size.height - 140));
    // todo: change positioning logic
    validatePurchase->getTitleLabel()->setPosition(40, 10);
    
    validatePurchase->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
        switch (type) {
            case Widget::TouchEventType::ENDED: {
                std::cout << "Track event raised" << std::endl;
                
                ValueMap params;
                AppsFlyerX::validateAndTrackInAppPurchase("1", "2", "USD", "3", params, [&](cocos2d::ValueMap result) {
                    for (auto& t : result)
                        std::cout << t.first << " "
                                  << t.second.asString() << "\n";
                }, [&](cocos2d::ValueMap error) {
                    for (auto& t : error)
                        std::cout << t.first << " "
                                  << t.second.asString() << "\n";
                });
                break;
            }
            default:
                break;
        }
    });
    addChild(validatePurchase, 1);
    addChild(exitButton, 1);
    addChild(button, 1);
    return true;
}
