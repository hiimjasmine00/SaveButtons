#include <cocos2d.h>
#include <Geode/loader/ModMetadata.hpp>

class SBModCell : public cocos2d::CCLayer {
protected:
    bool init(const geode::ModMetadata& metadata, int index);
public:
    static SBModCell* create(const geode::ModMetadata& metadata, int index);

    void draw() override;
};
