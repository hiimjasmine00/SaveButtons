#include <Geode/ui/Popup.hpp>

class SBSavePopup : public geode::Popup<> {
protected:
    std::vector<geode::Mod*> m_mods;

    bool setup() override;
public:
    static SBSavePopup* create();
};
