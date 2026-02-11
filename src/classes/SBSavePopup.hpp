#include <Geode/loader/Types.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>

class SBSavePopup : public geode::Popup {
protected:
    std::vector<geode::Mod*> m_mods;
    geode::ScrollLayer* m_scrollLayer;

    bool init() override;
    void onGameData(cocos2d::CCObject*);
    void onLocalLevels(cocos2d::CCObject*);
    void onModData(cocos2d::CCObject*);
    void updateMods(std::string_view query);
public:
    static SBSavePopup* create();
};
