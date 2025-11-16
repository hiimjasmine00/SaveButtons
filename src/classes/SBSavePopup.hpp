#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>

class SBSavePopup : public geode::Popup<> {
protected:
    std::vector<geode::Mod*> m_mods;
    geode::ScrollLayer* m_scrollLayer;

    bool setup() override;
    void updateMods(std::string_view query);
public:
    static SBSavePopup* create();
};
