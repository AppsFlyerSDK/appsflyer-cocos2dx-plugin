#include "HelloWorldScene.h"

#include "platform/CCCommon.h"
#include "ui/CocosGUI.h"
#include "ui/UILayout.h"
#include "ui/UIScrollView.h"

#include <cstdio>
#include <functional>
#include <memory>
#include <vector>

USING_NS_CC;
using namespace ui;

namespace {

constexpr int kMaxLogChars = 12000;

// Sample QA console palette
const Color3B kBgColor{22, 27, 38};
const Color3B kPanelColor{36, 42, 58};
const Color3B kAccentColor{108, 92, 231};
const Color3B kDangerColor{214, 69, 88};
const Color3B kMutedText{160, 168, 188};
const Color3B kHeaderText{245, 247, 252};

Layout* makePanel(const Size& size, const Color3B& fill, uint8_t opacity = 245) {
    auto* panel = Layout::create();
    panel->setContentSize(size);
    panel->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    panel->setBackGroundColor(fill);
    panel->setBackGroundColorOpacity(opacity);
    return panel;
}

Label* makeSectionTitle(const std::string& text, float width) {
    auto* label = Label::createWithSystemFont(text, "Helvetica Neue", 13);
    label->setTextColor(Color4B(kMutedText.r, kMutedText.g, kMutedText.b, 255));
    label->setAnchorPoint(Vec2(0.f, 1.f));
    label->setHorizontalAlignment(TextHAlignment::LEFT);
    return label;
}

Button* makeActionButton(const std::string& title, float width, bool destructive = false) {
    auto* button = Button::create("CloseNormal.png", "CloseSelected.png");
    button->setScale9Enabled(true);
    button->setContentSize(Size(width, 44.f));
    button->setTitleText(title);
    button->setTitleFontSize(16);
    button->setTitleColor(Color3B::WHITE);
    const Color3B fill = destructive ? kDangerColor : kAccentColor;
    if (auto* normal = button->getRendererNormal()) {
        normal->setColor(fill);
    }
    if (auto* selected = button->getRendererClicked()) {
        selected->setColor(Color3B(fill.r * 85 / 100, fill.g * 85 / 100, fill.b * 85 / 100));
    }
    button->setPressedActionEnabled(true);
    button->setZoomScale(-0.04f);
    if (auto* titleLabel = button->getTitleLabel()) {
        titleLabel->setPosition(button->getContentSize() * 0.5f);
    }
    return button;
}

struct QaAction {
    std::string title;
    std::function<void()> run;
    bool destructive;

    QaAction(std::string t, std::function<void()> r, bool d = false)
        : title(std::move(t)), run(std::move(r)), destructive(d) {}
};

void runQaRpc(const char* label, AppsFlyerTask task) {
    std::move(task).whenComplete([label](const AppsFlyerResult& r) { HelloWorld::appendAfResult(label, r); });
}

} // namespace

HelloWorld* HelloWorld::s_activeScene = nullptr;
Label* HelloWorld::s_eventLogLabel = nullptr;
ScrollView* HelloWorld::s_eventScroll = nullptr;
std::string HelloWorld::s_eventText;

void HelloWorld::appendEventLogInternal(const std::string& line) {
  if (line.empty()) {
    return;
  }
  if (!s_eventText.empty()) {
    s_eventText.push_back('\n');
  }
  s_eventText.append(line);
  if (static_cast<int>(s_eventText.size()) > kMaxLogChars) {
    s_eventText.erase(0, s_eventText.size() - kMaxLogChars);
    const auto nl = s_eventText.find('\n');
    if (nl != std::string::npos) {
      s_eventText.erase(0, nl + 1);
    }
  }
  refreshEventLogView();
}

void HelloWorld::appendEventLog(const std::string& line) {
  auto* director = Director::getInstance();
  if (!director) {
    return;
  }
  director->getScheduler()->performFunctionInCocosThread([line]() { appendEventLogInternal(line); });
}

void HelloWorld::refreshEventLogView() {
  if (!s_eventLogLabel || !s_eventScroll) {
    return;
  }
  const float pad = 12.f;
  const float innerW = s_eventScroll->getInnerContainerSize().width;
  const float textW = innerW - pad * 2.f;
  s_eventLogLabel->setDimensions(textW, 0);
  s_eventLogLabel->setString(s_eventText.empty() ? "Waiting for events…" : s_eventText);
  const Size textSize = s_eventLogLabel->getContentSize();
  const float innerH = std::max(textSize.height + pad * 2.f, s_eventScroll->getContentSize().height);
  s_eventScroll->setInnerContainerSize(Size(innerW, innerH));
  s_eventLogLabel->setPosition(Vec2(pad, innerH - pad));
  s_eventScroll->scrollToBottom(0.1f, true);
}

void HelloWorld::appendAfResult(const char* action, const AppsFlyerResult& r) {
  std::string line;
  if (r.success) {
    line = std::string("✓ ") + action;
    if (r.dataJson.empty()) {
      line += "\n(no RPC payload)";
    } else if (r.dataJson == "null") {
      line += "\ncompleted (void RPC)";
    } else {
      line += "\n";
      line += r.dataJson;
    }
  } else if (r.hasError) {
    line = std::string("✗ ") + action + " (" + std::to_string(r.error.code) + "): " + r.error.message;
  } else {
    line = std::string("✗ ") + action + " failed";
  }
  appendEventLog(line);
  CCLOG("[AppsFlyer][QA] %s", line.c_str());
}

void HelloWorld::appendDeepLink(const appsflyer::DeepLinkData& data) {
  switch (data.status) {
  case appsflyer::DeepLinkStatus::NotFound:
    appendEventLog("Deep link — not found");
    return;
  case appsflyer::DeepLinkStatus::Error:
    appendEventLog(std::string("Deep link — error: ") + (data.error.empty() ? "unknown" : data.error));
    return;
  case appsflyer::DeepLinkStatus::Found:
    break;
  }
  std::string block = "Deep link — found";
  for (const auto& entry : data.deepLink) {
    block.append("\n  ");
    block.append(entry.first);
    block.append(": ");
    block.append(entry.second.asString());
  }
  appendEventLog(block);
}

void HelloWorld::appendConversionData(const char* title, const cocos2d::ValueMap& map) {
  std::string block = title;
  for (const auto& entry : map) {
    block.append("\n  ");
    block.append(entry.first);
    block.append(": ");
    block.append(entry.second.asString());
  }
  appendEventLog(block);
}

void HelloWorld::onExit() {
  if (s_activeScene == this) {
    s_activeScene = nullptr;
    s_eventLogLabel = nullptr;
    s_eventScroll = nullptr;
  }
  Scene::onExit();
}

Scene* HelloWorld::createScene() {
  return HelloWorld::create();
}

bool HelloWorld::init() {
  if (!Scene::init()) {
    return false;
  }

  s_activeScene = this;
  s_eventText.clear();

  const auto visible = Director::getInstance()->getVisibleSize();
  const auto origin = Director::getInstance()->getVisibleOrigin();
  const float margin = 14.f;
  const float headerH = 56.f;

  auto* root = makePanel(visible, kBgColor, 255);
  root->setAnchorPoint(Vec2::ZERO);
  root->setPosition(origin);
  addChild(root, 0);

  auto* title = Label::createWithSystemFont("AppsFlyer Cocos2d-x", "Helvetica Neue", 22);
  title->setTextColor(Color4B(kHeaderText.r, kHeaderText.g, kHeaderText.b, 255));
  title->setAnchorPoint(Vec2(0.f, 0.5f));
  title->setPosition(Vec2(margin, visible.height - headerH * 0.55f));
  root->addChild(title, 2);

  auto* subtitle = Label::createWithSystemFont("SDK 7 QA console", "Helvetica Neue", 13);
  subtitle->setTextColor(Color4B(kMutedText.r, kMutedText.g, kMutedText.b, 255));
  subtitle->setAnchorPoint(Vec2(0.f, 0.5f));
  subtitle->setPosition(Vec2(margin, visible.height - headerH * 0.55f - 22.f));
  root->addChild(subtitle, 2);

  const float resultsH = visible.height * 0.38f;
  const float actionsTop = visible.height - headerH - resultsH - margin;
  const float actionsH = actionsTop - margin;
  const float innerW = visible.width - margin * 2.f;
  const float btnW = innerW - 20.f;

  // --- Results zone ---
  auto* resultsPanel = makePanel(Size(innerW, resultsH), kPanelColor);
  resultsPanel->setAnchorPoint(Vec2(0.5f, 1.f));
  resultsPanel->setPosition(Vec2(visible.width * 0.5f, visible.height - headerH));
  root->addChild(resultsPanel, 1);

  auto* resultsTitle = makeSectionTitle("RESULTS & EVENTS", innerW);
  resultsTitle->setPosition(Vec2(12.f, resultsH - 8.f));
  resultsPanel->addChild(resultsTitle);

  const float logH = resultsH - 36.f;
  s_eventScroll = ScrollView::create();
  s_eventScroll->setDirection(ScrollView::Direction::VERTICAL);
  s_eventScroll->setContentSize(Size(innerW - 16.f, logH));
  s_eventScroll->setPosition(Vec2(8.f, 8.f));
  s_eventScroll->setScrollBarEnabled(true);
  s_eventScroll->setBounceEnabled(true);
  resultsPanel->addChild(s_eventScroll);

  s_eventLogLabel = Label::createWithSystemFont("Waiting for events…", "Menlo", 12);
  s_eventLogLabel->setAnchorPoint(Vec2(0.f, 1.f));
  s_eventLogLabel->setTextColor(Color4B(210, 220, 240, 255));
  s_eventLogLabel->setWidth(innerW - 40.f);
  s_eventLogLabel->setOverflow(Label::Overflow::RESIZE_HEIGHT);
  s_eventLogLabel->setHorizontalAlignment(TextHAlignment::LEFT);
  s_eventScroll->addChild(s_eventLogLabel);
  s_eventScroll->setInnerContainerSize(Size(innerW - 16.f, logH));
  s_eventLogLabel->setPosition(Vec2(12.f, logH - 12.f));

  appendEventLogInternal("Ready — use actions below or open a deep link.");

  // --- Actions zone ---
  auto* actionsPanel = makePanel(Size(innerW, actionsH), kPanelColor);
  actionsPanel->setAnchorPoint(Vec2(0.5f, 0.f));
  actionsPanel->setPosition(Vec2(visible.width * 0.5f, margin));
  root->addChild(actionsPanel, 1);

  auto* actionsTitle = makeSectionTitle("ACTIONS", innerW);
  actionsTitle->setPosition(Vec2(12.f, actionsH - 8.f));
  actionsPanel->addChild(actionsTitle);

  const float actionsScrollH = actionsH - 34.f;
  auto* actionScroll = ScrollView::create();
  actionScroll->setDirection(ScrollView::Direction::VERTICAL);
  actionScroll->setContentSize(Size(innerW - 16.f, actionsScrollH));
  actionScroll->setPosition(Vec2(8.f, 8.f));
  actionScroll->setScrollBarEnabled(true);
  actionsPanel->addChild(actionScroll);

  std::vector<QaAction> actions;

  actions.emplace_back(QaAction{
      "Log event",
       []() {
         AppsFlyerX::LogEventParams logParams;
         logParams.eventName = "test_event";
         ValueMap map;
         map["key1"] = "value1";
         map["qa_source"] = "helloworld_button";
         logParams.eventValues = map;
         logParams.awaitResponse = true;
         std::move(AppsFlyerX::logEvent(logParams))
             .whenComplete([logParams](const AppsFlyerResult& r) {
               HelloWorld::appendAfResult("logEvent", r);
               if (r.success) {
                 std::string detail = "  eventName=" + logParams.eventName;
                 detail += ", awaitResponse=true";
                 for (const auto& entry : logParams.eventValues) {
                   detail += "\n  ";
                   detail += entry.first;
                   detail += "=";
                   detail += entry.second.asString();
                 }
                 HelloWorld::appendEventLog(detail);
               }
             });
       }});

  actions.emplace_back(QaAction{
      "Validate IAP",
       []() {
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
         runQaRpc("validateAndLogInAppPurchase",
                  AppsFlyerX::validateAndLogInAppPurchase(purchaseParams));
       }});

  actions.emplace_back(QaAction{"AppsFlyer UID", []() { runQaRpc("fetchAppsFlyerUID", AppsFlyerX::fetchAppsFlyerUID()); }});
  actions.emplace_back(QaAction{"SDK version", []() { runQaRpc("getSdkVersion", AppsFlyerX::getSdkVersion()); }});
  actions.emplace_back(QaAction{"Session ready?", []() { runQaRpc("isSessionReady", AppsFlyerX::isSessionReady()); }});
  actions.emplace_back(
      QaAction{"Set CUID", []() { runQaRpc("setCustomerUserId", AppsFlyerX::setCustomerUserId("cocos2dx-qa-button")); }});
  actions.emplace_back(QaAction{"Additional data", []() {
                                ValueMap map;
                                map["qa_screen"] = "HelloWorld";
                                runQaRpc("setAdditionalData", AppsFlyerX::setAdditionalData(map));
                              }});
  actions.emplace_back(QaAction{"Currency USD", []() { runQaRpc("setCurrencyCode", AppsFlyerX::setCurrencyCode("USD")); }});
  actions.emplace_back(QaAction{"Anonymize user", []() { runQaRpc("anonymizeUser", AppsFlyerX::anonymizeUser(true)); }});
  actions.emplace_back(QaAction{"Log ad revenue", []() {
                       AppsFlyerX::LogAdRevenueParams params;
                       params.monetizationNetwork = "DemoNetwork";
                       params.mediationNetwork = AppsFlyerX::MediationNetwork::ApplovinMax;
                       params.currencyIso4217Code = "USD";
                       params.revenue = 0.01;
                       runQaRpc("logAdRevenue", AppsFlyerX::logAdRevenue(params));
                     }});
  actions.emplace_back(QaAction{"Consent (GDPR)", []() {
                       AppsFlyerX::SetConsentDataParams consent;
                       consent.isUserSubjectToGDPR = true;
                       consent.hasConsentForDataUsage = true;
                       runQaRpc("setConsentData", AppsFlyerX::setConsentData(consent));
                     }});
  actions.emplace_back(
      QaAction{"Enable TCF", []() { runQaRpc("enableTCFDataCollection", AppsFlyerX::enableTCFDataCollection(true)); }});
  actions.emplace_back(QaAction{"Invite link", []() {
                       AppsFlyerX::GenerateInviteLinkParams link;
                       link.awaitResponse = true;
                       link.parameters["channel"] = "qa";
                       runQaRpc("generateInviteLink", AppsFlyerX::generateInviteLink(link));
                     }});
  actions.emplace_back(QaAction{"Log invite", []() {
                       AppsFlyerX::LogInviteParams invite;
                       invite.channel = "qa";
                       runQaRpc("logInvite", AppsFlyerX::logInvite(invite));
                     }});

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
  actions.emplace_back(
      QaAction{"Collect Android ID", []() { runQaRpc("setCollectAndroidID", AppsFlyerX::setCollectAndroidID(true)); }});
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  actions.emplace_back(
      QaAction{"Device language", []() { runQaRpc("setCurrentDeviceLanguage", AppsFlyerX::setCurrentDeviceLanguage("en")); }});
#endif

  actions.emplace_back(QaAction{
      "Unregister deep link",
      []() { runQaRpc("unregisterDeepLinkListener", AppsFlyerX::unregisterDeepLinkListener()); }});
  actions.emplace_back(QaAction{
      "Unregister conversion",
      []() { runQaRpc("unregisterConversionListener", AppsFlyerX::unregisterConversionListener()); }});
  actions.emplace_back(QaAction{
      "Unregister session ready",
      []() {
        runQaRpc("unregisterSessionReadyListener", AppsFlyerX::unregisterSessionReadyListener());
      }});

  actions.emplace_back(QaAction{"Quit", []() { Director::getInstance()->end(); }, true});

  const float rowGap = 10.f;
  const float padTop = 12.f;
  const float contentHeight =
      padTop + static_cast<float>(actions.size()) * (44.f + rowGap) + 8.f;
  const float innerH = std::max(contentHeight, actionsScrollH);
  actionScroll->setInnerContainerSize(Size(btnW, innerH));
  auto* inner = actionScroll->getInnerContainer();
  auto actionsPtr = std::make_shared<std::vector<QaAction>>(std::move(actions));

  for (size_t i = 0; i < actionsPtr->size(); ++i) {
    const float y = innerH - padTop - static_cast<float>(i) * (44.f + rowGap) - 22.f;
    auto* button = makeActionButton((*actionsPtr)[i].title, btnW, (*actionsPtr)[i].destructive);
    button->setAnchorPoint(Vec2(0.5f, 0.5f));
    button->setPosition(Vec2(btnW * 0.5f, y));
    const size_t actionIndex = i;
    button->addTouchEventListener([actionsPtr, actionIndex](Ref*, Widget::TouchEventType type) {
      if (type != Widget::TouchEventType::ENDED) {
        return;
      }
      (*actionsPtr)[actionIndex].run();
    });
    inner->addChild(button);
  }

  return true;
}