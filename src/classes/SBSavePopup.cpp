#include "SBSavePopup.hpp"
#include "SBModCell.hpp"
#include "../SaveButtons.hpp"
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/LocalLevelManager.hpp>
#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include <Geode/external/fts/fts_fuzzy_match.h>
#include <Geode/loader/Mod.hpp>
#include <Geode/ui/Notification.hpp>
#include <Geode/ui/Scrollbar.hpp>
#include <Geode/ui/TextInput.hpp>

using namespace geode::prelude;

static std::string saveQuery;

SBSavePopup* SBSavePopup::create() {
    auto ret = new SBSavePopup();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SBSavePopup::init() {
    if (!Popup::init(420.0f, 290.0f)) return false;

    setID("SBSavePopup");
    setTitle("Save Buttons");
    m_title->setID("save-buttons-title");
    m_mainLayer->setID("main-layer");
    m_buttonMenu->setID("button-menu");
    m_bgSprite->setID("background");
    m_closeBtn->setID("close-button");

    m_mods = Loader::get()->getAllMods();
    for (auto it = m_mods.begin(); it != m_mods.end();) {
        if ((*it)->getID().view().starts_with("geode_invalid.")) it = m_mods.erase(it);
        else ++it;
    }

    auto saveMenu = CCMenu::create();
    saveMenu->setPosition({ 210.0f, 235.0f });
    saveMenu->setContentSize({ 420.0f, 50.0f });
    saveMenu->setLayout(RowLayout::create()->setGap(5.0f));
    saveMenu->setID("save-menu");

    auto gameDataSprite = ButtonSprite::create("Game Data", "goldFont.fnt", "GJ_button_01.png", 0.8f);
    auto gameDataButton = CCMenuItemExt::createSpriteExtra(gameDataSprite, [](auto) {
        auto timer = asp::Instant::now();
        GameManager::get()->save();
        auto elapsed = timer.elapsed();
        Notification::create(fmt::format("Game data saved in {}", SaveButtons::format(elapsed)), NotificationIcon::Success)->show();
    });
    gameDataButton->setID("game-data-button");
    saveMenu->addChild(gameDataButton);

    auto localLevelsSprite = ButtonSprite::create("Local Levels", "goldFont.fnt", "GJ_button_02.png", 0.8f);
    auto localLevelsButton = CCMenuItemExt::createSpriteExtra(localLevelsSprite, [](auto) {
        auto timer = asp::Instant::now();
        LocalLevelManager::get()->save();
        auto elapsed = timer.elapsed();
        Notification::create(fmt::format("Local levels saved in {}", SaveButtons::format(elapsed)), NotificationIcon::Success)->show();
    });
    localLevelsButton->setID("local-levels-button");
    saveMenu->addChild(localLevelsButton);

    auto modDataSprite = ButtonSprite::create("Mod Data", "goldFont.fnt", "GJ_button_03.png", 0.8f);
    auto modDataButton = CCMenuItemExt::createSpriteExtra(modDataSprite, [this](auto) {
        auto settings = 0;
        auto saved = 0;
        auto timer = asp::Instant::now();
        for (auto mod : m_mods) {
            auto pair = SaveButtons::save(mod);
            if (pair.first) settings++;
            if (pair.second) saved++;
        }
        auto elapsed = timer.elapsed();

        auto size = m_mods.size();
        auto icon = NotificationIcon::Info;
        if (settings >= size && saved >= size) icon = NotificationIcon::Success;
        else if (settings == 0 && saved == 0) icon = NotificationIcon::Error;

        Notification::create(fmt::format("Mod data saved in {} (settings {}/{}, save data {}/{})",
            SaveButtons::format(elapsed), settings, size, saved, size), icon)->show();
    });
    modDataButton->setID("mod-data-button");
    saveMenu->addChild(modDataButton);

    saveMenu->updateLayout();
    m_mainLayer->addChild(saveMenu);

    auto textInput = TextInput::create(400.0f, "Search Mods...");
    textInput->setPosition({ 210.0f, 200.0f });
    textInput->setString(saveQuery);
    textInput->setCallback([this](const std::string& str) {
        saveQuery = str;
        updateMods(str);
    });
    textInput->setID("search-input");
    m_mainLayer->addChild(textInput);

    auto scrollBackground = CCLayerColor::create({ 0, 0, 0, 100 }, 390.0f, 170.0f);
    scrollBackground->setPosition({ 204.5f, 95.0f });
    scrollBackground->ignoreAnchorPointForPosition(false);
    scrollBackground->setID("scroll-background");
    m_mainLayer->addChild(scrollBackground);

    m_scrollLayer = ScrollLayer::create({ 380.0f, 160.0f });
    m_scrollLayer->setPosition({ 204.5f, 95.0f });
    m_scrollLayer->ignoreAnchorPointForPosition(false);
    m_scrollLayer->m_contentLayer->setLayout(ColumnLayout::create()
        ->setAxisReverse(true)
        ->setAxisAlignment(AxisAlignment::End)
        ->setAutoGrowAxis(190.0f)
        ->setGap(0.0f));
    m_scrollLayer->setID("scroll-layer");
    m_mainLayer->addChild(m_scrollLayer);

    auto scrollbar = Scrollbar::create(m_scrollLayer);
    scrollbar->setPosition({ 406.5f, 95.0f });
    scrollbar->setScaleY(1.05f);
    scrollbar->setID("scrollbar");
    m_mainLayer->addChild(scrollbar);

    updateMods(saveQuery);

    return true;
}

void weightedFuzzyMatch(std::string_view str, std::string_view query, double weight, double& current) {
    int score;
    if (fts::fuzzy_match(query.data(), str.data(), score)) {
        auto finalScore = score * weight;
        if (finalScore > current) current = finalScore;
    }
}

void SBSavePopup::updateMods(std::string_view query) {
    m_scrollLayer->m_contentLayer->removeAllChildren();

    std::vector<std::pair<Mod*, double>> filteredMods;

    for (auto mod : m_mods) {
        if (query.empty()) {
            filteredMods.emplace_back(mod, mod->isInternal() ? 5.0 : 0.0);
            continue;
        }

        auto& metadata = mod->getMetadata();
        auto weighted = 0.0;
        weightedFuzzyMatch(metadata.getName(), query, 1.0, weighted);
        weightedFuzzyMatch(metadata.getID(), query, 0.5, weighted);
        for (auto& dev : metadata.getDevelopers()) {
            weightedFuzzyMatch(dev, query, 0.25, weighted);
        }
        if (auto details = metadata.getDetails()) {
            weightedFuzzyMatch(details.value(), query, 0.005, weighted);
        }
        if (auto desc = metadata.getDescription()) {
            weightedFuzzyMatch(desc.value(), query, 0.02, weighted);
        }
        if (weighted >= 2.0) {
            if (mod->isInternal()) weighted += 5.0;
            filteredMods.emplace_back(mod, weighted);
        }
    }

    std::ranges::sort(filteredMods, [](const std::pair<Mod*, double>& a, const std::pair<Mod*, double>& b) {
        if (a.second != b.second) return a.second > b.second;

        auto& aMeta = a.first->getMetadata();
        auto& bMeta = b.first->getMetadata();

        auto aOutdated = aMeta.checkTargetVersions().isErr();
        auto bOutdated = bMeta.checkTargetVersions().isErr();
        if (aOutdated != bOutdated) return aOutdated < bOutdated;

        return string::caseInsensitiveCompare(aMeta.getName(), bMeta.getName()) == std::strong_ordering::less;
    });

    for (size_t i = 0; i < filteredMods.size(); i++) {
        m_scrollLayer->m_contentLayer->addChild(SBModCell::create(filteredMods[i].first->getMetadata(), i));
    }

    m_scrollLayer->m_contentLayer->updateLayout();
    m_scrollLayer->scrollToTop();
}
