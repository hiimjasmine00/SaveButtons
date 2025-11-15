#include <cocos2d.h>

class SBModCell : public cocos2d::CCLayer {
protected:
    bool init(geode::Mod* mod, int index);
public:
    static SBModCell* create(geode::Mod* mod, int index);

    void draw() override;
};
