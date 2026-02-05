#include "../SaveButtons.hpp"
#include "../classes/SBSavePopup.hpp"
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <jasmine/hook.hpp>
#include <jasmine/setting.hpp>

using namespace geode::prelude;

class $modify(SBMenuLayer, MenuLayer) {
    static void onModify(ModifyBase<ModifyDerive<SBMenuLayer, MenuLayer>>& self) {
        if (auto customKeybinds = Loader::get()->getInstalledMod("geode.custom-keybinds")) {
            auto hook = jasmine::hook::get(self.m_hooks, "MenuLayer::init", true);
            if (customKeybinds->isEnabled()) {
                hook->setAutoEnable(!jasmine::setting::getValue<bool>("hide-main-menu-button"));
                keybindsLoaded(hook);
            }
            else {
                ModStateEvent(ModEventType::Loaded, customKeybinds).listen([hook] {
                    jasmine::hook::toggle(hook, !jasmine::setting::getValue<bool>("hide-main-menu-button"));
                    keybindsLoaded(hook);
                }).leak();
            }
        }
    }

    static void keybindsLoaded(Hook* hook) {
        SaveButtons::registerKeybind();
        SettingChangedEventV3(GEODE_MOD_ID, "hide-main-menu-button").listen([hook](std::shared_ptr<SettingV3> setting) {
            jasmine::hook::toggle(hook, !std::static_pointer_cast<BoolSettingV3>(std::move(setting))->getValue());
        }).leak();
    }

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
