#include <Geode/binding/TextInputDelegate.hpp>
#include <Geode/loader/Types.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>

class SBSavePopup : public geode::Popup, public TextInputDelegate {
protected:
    std::vector<geode::Mod*> m_mods;
    geode::ScrollLayer* m_scrollLayer;

    bool init() override;
    void onGameData(cocos2d::CCObject*);
    void onLocalLevels(cocos2d::CCObject*);
    void onModData(cocos2d::CCObject*);
    void textChanged(CCTextInputNode*) override;
    void updateMods();
public:
    static SBSavePopup* create();
};
