#include "HelloWorldScene.h"

#include "AppsFlyer/AppsFlyerX.h"
#include "platform/CCCommon.h"
#include "ui/CocosGUI.h"
#include "ui/UIScrollView.h"

#include <cstdio>
#include <functional>
#include <memory>
#include <vector>

USING_NS_CC;
using namespace ui;

namespace {

void showAfDialog(const char* title, const char* message) {
    cocos2d::ccMessageBox(message, title);
}

void showAfResult(const char* action, const AppsFlyerResult& r) {
    char message[512];
    if (r.success) {
        if (!r.dataJson.empty()) {
            std::snprintf(message, sizeof(message), "%s\nsuccess\n%s", action, r.dataJson.c_str());
        } else {
            std::snprintf(message, sizeof(message), "%s\nsuccess", action);
        }
    } else if (r.hasError) {
        std::snprintf(message, sizeof(message), "%s\nfailed (%d): %s", action, r.error.code,
                      r.error.message.c_str());
    } else {
        std::snprintf(message, sizeof(message), "%s\nfailed", action);
    }
    showAfDialog("AppsFlyer", message);
    CCLOG("[AppsFlyer][QA] %s", message);
}

void runRpc(const char* label, AppsFlyerTask task) {
    std::move(task).whenComplete([label](const AppsFlyerResult& r) { showAfResult(label, r); });
}

Button* makeQaButton(const std::string& title, const Vec2& position) {
    auto button = Button::create("CloseNormal.png", "CloseSelected.png");
    button->setTitleText(title);
    button->setTitleFontSize(14);
    button->setScale9Enabled(true);
    button->setContentSize(Size(220, 36));
    button->setPosition(position);
    if (auto* label = button->getTitleLabel()) {
        label->setPosition(button->getContentSize() / 2);
    }
    return button;
}

struct QaAction {
    std::string title;
    std::function<void()> run;
};

} // namespace

Scene* HelloWorld::createScene() {
    return HelloWorld::create();
}

bool HelloWorld::init() {
    if (!Scene::init()) {
        return false;
    }

    const auto size = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto logo = ImageView::create("logo.png");
    if (logo) {
        logo->setPosition(Vec2(origin.x + size.width / 2,
                               origin.y + size.height - logo->getContentSize().height * 0.6f));
        addChild(logo, 1);
    }

    auto hint = Label::createWithSystemFont(
        "SDK 7 QA panel (sample app — not exhaustive)", "Arial", 11);
    hint->setPosition(Vec2(origin.x + size.width / 2, origin.y + size.height - 52));
    hint->setTextColor(Color4B(140, 140, 140, 255));
    addChild(hint, 1);

    const float buttonWidth = 220.f;
    const float rowHeight = 44.f;
    const float padTop = 72.f;

    std::vector<QaAction> actions;

    actions.push_back({"Log event", []() {
                           AppsFlyerX::LogEventParams logParams;
                           logParams.eventName = "test_event";
                           ValueMap map;
                           map["key1"] = "value1";
                           map["key2"] = 1;
                           map["qa_source"] = "helloworld_button";
                           logParams.eventValues = map;
                           logParams.awaitResponse = true;
                           runRpc("logEvent", AppsFlyerX::logEvent(logParams));
                       }});

    actions.push_back({"Validate IAP", []() {
                           AppsFlyerX::ValidateAndLogInAppPurchaseParams purchaseParams;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
                           purchaseParams.purchase.purchaseType = AppsFlyerX::PurchaseType::OneTimePurchase;
                           purchaseParams.purchase.productId = "demo_product";
                           purchaseParams.purchase.purchaseToken = "demo_token";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                           purchaseParams.purchase.purchaseType = AppsFlyerX::PurchaseType::OneTimePurchase;
                           purchaseParams.purchase.productId = "demo_product";
                           purchaseParams.purchase.transactionId = "demo_txn";
#endif
                           runRpc("validateAndLogInAppPurchase",
                                  AppsFlyerX::validateAndLogInAppPurchase(purchaseParams));
                       }});

    actions.push_back({"AppsFlyer UID", []() {
                           runRpc("fetchAppsFlyerUID", AppsFlyerX::fetchAppsFlyerUID());
                       }});

    actions.push_back({"SDK version", []() {
                           runRpc("getSdkVersion", AppsFlyerX::getSdkVersion());
                       }});

    actions.push_back({"Session ready?", []() {
                           runRpc("isSessionReady", AppsFlyerX::isSessionReady());
                       }});

    actions.push_back({"Set CUID", []() {
                           runRpc("setCustomerUserId",
                                  AppsFlyerX::setCustomerUserId("cocos2dx-qa-button"));
                       }});

    actions.push_back({"Additional data", []() {
                           ValueMap map;
                           map["qa_screen"] = "HelloWorld";
                           map["sample"] = true;
                           runRpc("setAdditionalData", AppsFlyerX::setAdditionalData(map));
                       }});

    actions.push_back({"Currency USD", []() {
                           runRpc("setCurrencyCode", AppsFlyerX::setCurrencyCode("USD"));
                       }});

    actions.push_back({"Anonymize user", []() {
                           runRpc("anonymizeUser", AppsFlyerX::anonymizeUser(true));
                       }});

    actions.push_back({"Log ad revenue", []() {
                           AppsFlyerX::LogAdRevenueParams params;
                           params.monetizationNetwork = "DemoNetwork";
                           params.mediationNetwork = AppsFlyerX::MediationNetwork::ApplovinMax;
                           params.currencyIso4217Code = "USD";
                           params.revenue = 0.01;
                           runRpc("logAdRevenue", AppsFlyerX::logAdRevenue(params));
                       }});

    actions.push_back({"Consent (GDPR)", []() {
                           AppsFlyerX::SetConsentDataParams consent;
                           consent.isUserSubjectToGDPR = true;
                           consent.hasConsentForDataUsage = true;
                           consent.hasConsentForAdsPersonalization = true;
                           consent.hasConsentForAdStorage = true;
                           runRpc("setConsentData", AppsFlyerX::setConsentData(consent));
                       }});

    actions.push_back({"Enable TCF", []() {
                           runRpc("enableTCFDataCollection",
                                  AppsFlyerX::enableTCFDataCollection(true));
                       }});

    actions.push_back({"Invite link", []() {
                           AppsFlyerX::GenerateInviteLinkParams link;
                           link.awaitResponse = true;
                           link.parameters["channel"] = "qa";
                           link.parameters["campaign"] = "cocos_sample";
                           runRpc("generateInviteLink", AppsFlyerX::generateInviteLink(link));
                       }});

    actions.push_back({"Log invite", []() {
                           AppsFlyerX::LogInviteParams invite;
                           invite.channel = "qa";
                           invite.eventParameters["campaign"] = "cocos_sample";
                           runRpc("logInvite", AppsFlyerX::logInvite(invite));
                       }});

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    actions.push_back({"Collect Android ID", []() {
                           runRpc("setCollectAndroidID", AppsFlyerX::setCollectAndroidID(true));
                       }});
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    actions.push_back({"Device language", []() {
                           runRpc("setCurrentDeviceLanguage",
                                  AppsFlyerX::setCurrentDeviceLanguage("en"));
                       }});
#endif

    actions.push_back({"Quit", []() {
                           Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                           exit(0);
#endif
                       }});

    const float contentHeight = padTop + static_cast<float>(actions.size()) * rowHeight + 16.f;
    const float viewHeight = size.height - 64.f;

    auto scroll = ScrollView::create();
    scroll->setDirection(ScrollView::Direction::VERTICAL);
    scroll->setContentSize(Size(size.width, viewHeight));
    scroll->setPosition(Vec2(origin.x, origin.y));
    scroll->setInnerContainerSize(Size(size.width, std::max(contentHeight, viewHeight)));
    addChild(scroll, 1);

    auto inner = scroll->getInnerContainer();
    auto actionsPtr = std::make_shared<std::vector<QaAction>>(std::move(actions));

    for (size_t i = 0; i < actionsPtr->size(); ++i) {
        const float y = contentHeight - padTop - static_cast<float>(i) * rowHeight;
        auto button = makeQaButton((*actionsPtr)[i].title, Vec2(size.width * 0.5f, y));
        const size_t actionIndex = i;
        button->addTouchEventListener(
            [actionsPtr, actionIndex](Ref*, Widget::TouchEventType type) {
                if (type != Widget::TouchEventType::ENDED) {
                    return;
                }
                (*actionsPtr)[actionIndex].run();
            });
        inner->addChild(button);
    }

    return true;
}