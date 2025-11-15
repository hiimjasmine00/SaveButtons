#include "../classes/SBSavePopup.hpp"
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>

using namespace geode::prelude;

class $modify(SBMenuLayer, MenuLayer) {
    bool init() override {
        if (!MenuLayer::init()) return false;

        if (auto bottomMenu = getChildByID("bottom-menu")) {
            auto saveSprite = CircleButtonSprite::createWithSpriteFrameName(
                "geode.loader/save.png", 1.0f, CircleBaseColor::Green, CircleBaseSize::MediumAlt);
            auto saveButton = CCMenuItemSpriteExtra::create(saveSprite, this, menu_selector(SBMenuLayer::onSaveButtons));
            saveButton->setID("save-button"_spr);
            bottomMenu->addChild(saveButton);
            bottomMenu->updateLayout();
        }

        return true;
    }

    void onSaveButtons(CCObject*) {
        SBSavePopup::create()->show();
    }
};
