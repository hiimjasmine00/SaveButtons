#include "SBModCell.hpp"
#include "../SaveButtons.hpp"
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/loader/Loader.hpp>
#include <Geode/ui/Notification.hpp>
#include <Geode/utils/string.hpp>

using namespace geode::prelude;

SBModCell* SBModCell::create(const ModMetadata& metadata, int index) {
    auto ret = new SBModCell();
    if (ret->init(metadata, index)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SBModCell::init(const ModMetadata& metadata, int index) {
    if (!CCLayer::init()) return false;

    setID(metadata.getID());
    setContentSize({ 380.0f, 30.0f });

    auto background = CCLayerColor::create(index % 2 == 0 ? ccColor4B { 161, 88, 44, 255 } : ccColor4B { 194, 114, 62, 255 }, 380.0f, 30.0f);
    background->setPosition({ 190.0f, 15.0f });
    background->setContentSize({ 380.0f, 30.0f });
    background->ignoreAnchorPointForPosition(false);
    background->setID("background");
    addChild(background);

    auto nameLabel = CCLabelBMFont::create(metadata.getName().c_str(), "bigFont.fnt");
    nameLabel->setPosition({ 5.0f, 22.0f });
    nameLabel->setAnchorPoint({ 0.0f, 0.5f });
    nameLabel->setScale(0.37f);
    if (metadata.checkTargetVersions().isErr()) {
        nameLabel->setColor({ 125, 125, 125 });
    }
    nameLabel->setID("name-label");
    addChild(nameLabel);

    auto buttonMenu = CCMenu::create();
    buttonMenu->setPosition({ 190.0f, 15.0f });
    buttonMenu->setContentSize({ 380.0f, 30.0f });
    buttonMenu->ignoreAnchorPointForPosition(false);
    buttonMenu->setID("button-menu");
    addChild(buttonMenu);

    auto developersLabel = CCLabelBMFont::create(ModMetadata::formatDeveloperDisplayString(metadata.getDevelopers()).c_str(), "goldFont.fnt");
    developersLabel->setScale(0.4f);
    auto developersButton = CCMenuItemExt::createSpriteExtra(developersLabel, [&metadata](auto) {
        auto title = fmt::format("{} Developers", metadata.getName());
        FLAlertLayer::create(
            nullptr,
            title.c_str(),
            string::join(metadata.getDevelopers(), "\n"),
            "OK",
            nullptr,
            getLabelSize(title, "goldFont.fnt").width * 0.9f + 40.0f
        )->show();
    });
    developersButton->setPosition(developersLabel->getScaledContentSize() / 2.0f + CCPoint { 5.0f, 4.0f });
    developersButton->setID("developers-button");
    buttonMenu->addChild(developersButton);

    auto saveSprite = ButtonSprite::create("Save", "goldFont.fnt", "GJ_button_04.png", 0.8f);
    saveSprite->setScale(0.8f);
    auto saveButton = CCMenuItemExt::createSpriteExtra(saveSprite, [&metadata](auto) {
        auto start = std::chrono::steady_clock::now();
        auto [settings, saved] = SaveButtons::save(Loader::get()->getInstalledMod(metadata.getID()));
        auto end = std::chrono::steady_clock::now();

        auto name = metadata.getName();
        auto format = SaveButtons::format(start, end);
        NotificationIcon icon;
        std::string message;
        if (settings && saved) {
            icon = NotificationIcon::Success;
            message = fmt::format("Mod data for {} saved in {}", name, format);
        }
        else if (!settings && !saved) {
            icon = NotificationIcon::Error;
            message = fmt::format("Failed to save mod data for {} in {}", name, format);
        }
        else {
            icon = NotificationIcon::Info;
            message = fmt::format("Mod data for {} saved without {} in {}", name, !settings ? "settings" : "save data", format);
        }

        Notification::create(message, icon)->show();
    });
    saveButton->setPosition({ 380.0f - saveSprite->getScaledContentSize().width / 2.0f - 5.0f, 15.0f });
    saveButton->setID("save-button");
    buttonMenu->addChild(saveButton);

    return true;
}

void SBModCell::draw() {
    ccDrawColor4B(0, 0, 0, 75);
    glLineWidth(2.0f);
    ccDrawLine({ 1.0f, 0.0f }, { 379.0f, 0.0f });
    ccDrawLine({ 1.0f, 30.0f }, { 379.0f, 30.0f });
}
