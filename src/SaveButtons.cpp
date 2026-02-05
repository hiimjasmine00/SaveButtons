#include "SaveButtons.hpp"
//#include "classes/SBSavePopup.hpp"
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/ModSettingsManager.hpp>
//#include <geode.custom-keybinds/include/OptionalAPI.hpp>

using namespace geode::prelude;
//using namespace keybinds;

void SaveButtons::registerKeybind() {
    /*if (auto cat = CategoryV2::create("Save Buttons")) {
        std::vector<Ref<Bind>> defs;
        if (auto keybind = KeybindV2::create(KEY_S, ModifierV2::Control | ModifierV2::Shift)) {
            defs.push_back(keybind.unwrap());
        }
        else return;
        #ifdef GEODE_IS_MACOS
        if (auto keybind = KeybindV2::create(KEY_S, ModifierV2::Command | ModifierV2::Shift)) {
            defs.push_back(keybind.unwrap());
        }
        else return;
        #endif

        if (auto action = BindableActionV2::create(
            "save-popup"_spr,
            "Open Save Popup",
            "Open the Save Buttons popup",
            defs,
            std::move(cat).unwrap()
        )) {
            if (BindManagerV2::registerBindable(std::move(action).unwrap())) {
                new EventListener(+[](InvokeBindEventV2* event) {
                    if (event->isDown()) SBSavePopup::create()->show();
                    return ListenerResult::Propagate;
                }, InvokeBindFilterV2(nullptr, "save-popup"_spr));
            }
        }
    }*/
}

std::string SaveButtons::format(const asp::Duration& duration) {
    return fmt::format("{}.{:03}s", duration.seconds(), duration.subsecMillis());
}

std::pair<bool, bool> SaveButtons::save(Mod* mod) {
    auto json = ModSettingsManager::from(mod)->save();

    ModStateEvent(ModEventType::DataSaved, mod).send();

    auto name = mod->getName();
    auto saveDir = mod->getSaveDir();

    auto settingsRes = file::writeStringSafe(saveDir / "settings.json", json.dump());
    if (settingsRes.isErr()) log::error("Failed to save settings for {}: {}", name, settingsRes.unwrapErr());

    auto savedRes = file::writeStringSafe(saveDir / "saved.json", mod->getSavedSettingsData().dump());
    if (savedRes.isErr()) log::error("Failed to save saved data for {}: {}", name, savedRes.unwrapErr());

    return { settingsRes.isOk(), savedRes.isOk() };
}
