#include "SaveButtons.hpp"
#include "classes/SBSavePopup.hpp"
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/ModSettingsManager.hpp>
#include <geode.custom-keybinds/include/OptionalAPI.hpp>

using namespace geode::prelude;
using namespace keybinds;

void registerKeybind() {
    if (auto cat = CategoryV2::create("Save Buttons")) {
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
    }
}

$on_mod(Loaded) {
    if (auto customKeybinds = Loader::get()->getInstalledMod("geode.custom-keybinds")) {
        if (customKeybinds->isEnabled()) {
            registerKeybind();
        }
        else {
            new EventListener(+[](ModStateEvent*) {
                registerKeybind();
            }, ModStateFilter(customKeybinds, ModEventType::Loaded));
        }
    }
}

std::string SaveButtons::format(const std::chrono::steady_clock::time_point& start, const std::chrono::steady_clock::time_point& end) {
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    return fmt::format("{}.{:03}s", dur / 1000, dur % 1000);
}

std::pair<bool, bool> SaveButtons::save(Mod* mod) {
    auto json = ModSettingsManager::from(mod)->save();

    ModStateEvent(mod, ModEventType::DataSaved).post();

    auto name = mod->getName();
    auto saveDir = mod->getSaveDir();

    auto settingsRes = file::writeStringSafe(saveDir / "settings.json", json.dump());
    if (settingsRes.isErr()) log::error("Failed to save settings for {}: {}", name, settingsRes.unwrapErr());

    auto savedRes = file::writeStringSafe(saveDir / "saved.json", mod->getSavedSettingsData().dump());
    if (savedRes.isErr()) log::error("Failed to save saved data for {}: {}", name, savedRes.unwrapErr());

    return std::make_pair(settingsRes.isOk(), savedRes.isOk());
}
