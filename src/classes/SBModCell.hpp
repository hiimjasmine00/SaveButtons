#include <cocos2d.h>
#include <Geode/loader/ModMetadata.hpp>

class SBModCell : public cocos2d::CCLayer {
protected:
    const geode::ModMetadata* m_metadata;
    geode::Mod* m_mod;

    bool init(geode::Mod* mod, int index);
    void onDevelopers(cocos2d::CCObject*);
    void onSave(cocos2d::CCObject*);
public:
    static SBModCell* create(geode::Mod* mod, int index);

    void draw() override;
};
