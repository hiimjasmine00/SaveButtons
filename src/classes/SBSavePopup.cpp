#include "SBSavePopup.hpp"
#include "SBModCell.hpp"
#include "../SaveButtons.hpp"
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/LocalLevelManager.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/ModSettingsManager.hpp>
#include <Geode/ui/Notification.hpp>
#include <Geode/ui/Scrollbar.hpp>
#include <Geode/ui/ScrollLayer.hpp>

using namespace geode::prelude;

SBSavePopup* SBSavePopup::create() {
    auto ret = new SBSavePopup();
    if (ret->initAnchored(420.0f, 290.0f)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SBSavePopup::setup() {
    setID("SBSavePopup");
    setTitle("Save Buttons");
    m_title->setID("save-buttons-title");
    m_mainLayer->setID("main-layer");
    m_buttonMenu->setID("button-menu");
    m_bgSprite->setID("background");
    m_closeBtn->setID("close-button");

    m_mods = Loader::get()->getAllMods();
    std::ranges::sort(m_mods, [](Mod* a, Mod* b) {
        if (a->isInternal()) return true;
        if (b->isInternal()) return false;
        return string::caseInsensitiveCompare(a->getName(), b->getName()) == std::strong_ordering::less;
    });

    auto saveMenu = CCMenu::create();
    saveMenu->setPosition({ 210.0f, 235.0f });
    saveMenu->setContentSize({ 420.0f, 50.0f });
    saveMenu->setLayout(RowLayout::create()->setGap(5.0f));
    saveMenu->setID("save-menu");

    auto gameDataSprite = ButtonSprite::create("Game Data", "goldFont.fnt", "GJ_button_01.png", 0.8f);
    auto gameDataButton = CCMenuItemExt::createSpriteExtra(gameDataSprite, [](auto) {
        auto start = std::chrono::steady_clock::now();
        GameManager::get()->save();
        auto end = std::chrono::steady_clock::now();
        Notification::create(fmt::format("Game data saved in {}", SaveButtons::format(start, end)), NotificationIcon::Success)->show();
    });
    gameDataButton->setID("game-data-button");
    saveMenu->addChild(gameDataButton);

    auto localLevelsSprite = ButtonSprite::create("Local Levels", "goldFont.fnt", "GJ_button_02.png", 0.8f);
    auto localLevelsButton = CCMenuItemExt::createSpriteExtra(localLevelsSprite, [](auto) {
        auto start = std::chrono::steady_clock::now();
        LocalLevelManager::get()->save();
        auto end = std::chrono::steady_clock::now();
        Notification::create(fmt::format("Local levels saved in {}", SaveButtons::format(start, end)), NotificationIcon::Success)->show();
    });
    localLevelsButton->setID("local-levels-button");
    saveMenu->addChild(localLevelsButton);

    auto modDataSprite = ButtonSprite::create("Mod Data", "goldFont.fnt", "GJ_button_03.png", 0.8f);
    auto modDataButton = CCMenuItemExt::createSpriteExtra(modDataSprite, [this](auto) {
        auto settings = 0;
        auto saved = 0;
        auto start = std::chrono::steady_clock::now();
        for (auto mod : m_mods) {
            auto pair = SaveButtons::save(mod);
            if (pair.first) settings++;
            if (pair.second) saved++;
        }
        auto end = std::chrono::steady_clock::now();

        auto size = m_mods.size();
        auto icon = NotificationIcon::Info;
        if (settings >= size && saved >= size) icon = NotificationIcon::Success;
        else if (settings == 0 && saved == 0) icon = NotificationIcon::Error;

        Notification::create(fmt::format("Mod data saved in {} (settings {}/{}, save data {}/{})",
            SaveButtons::format(start, end), settings, size, saved, size), icon)->show();
    });
    modDataButton->setID("mod-data-button");
    saveMenu->addChild(modDataButton);

    saveMenu->updateLayout();
    m_mainLayer->addChild(saveMenu);

    auto scrollBackground = CCLayerColor::create({ 0, 0, 0, 100 }, 390.0f, 200.0f);
    scrollBackground->setPosition({ 204.5f, 110.0f });
    scrollBackground->ignoreAnchorPointForPosition(false);
    scrollBackground->setID("scroll-background");
    m_mainLayer->addChild(scrollBackground);

    auto scrollLayer = ScrollLayer::create({ 380.0f, 190.0f });
    scrollLayer->setPosition({ 204.5f, 110.0f });
    scrollLayer->ignoreAnchorPointForPosition(false);
    scrollLayer->m_contentLayer->setLayout(ColumnLayout::create()
        ->setAxisReverse(true)
        ->setAxisAlignment(AxisAlignment::End)
        ->setAutoGrowAxis(190.0f)
        ->setGap(0.0f));
    scrollLayer->setID("scroll-layer");

    for (size_t i = 0; i < m_mods.size(); i++) {
        scrollLayer->m_contentLayer->addChild(SBModCell::create(m_mods[i], i));
    }

    scrollLayer->m_contentLayer->updateLayout();
    scrollLayer->scrollToTop();
    m_mainLayer->addChild(scrollLayer);

    auto scrollbar = Scrollbar::create(scrollLayer);
    scrollbar->setPosition({ 406.5f, 110.0f });
    scrollbar->setScaleY(1.05f);
    scrollbar->setID("scrollbar");
    m_mainLayer->addChild(scrollbar);

    return true;
}
