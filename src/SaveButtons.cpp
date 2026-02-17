#include "SaveButtons.hpp"
#include "classes/SBSavePopup.hpp"
#include <Geode/loader/GameEvent.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/ModSettingsManager.hpp>

using namespace geode::prelude;

$on_game(Loaded) {
    listenForKeybindSettingPresses("save-popup", [](const Keybind& keybind, bool down, bool repeat, double timestamp) {
        if (down && !repeat) SBSavePopup::create()->show();
    });
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
