#include <asp/time/Duration.hpp>
#include <Geode/loader/Types.hpp>

class SaveButtons {
public:
    static std::string format(const asp::Duration& duration);
    static std::pair<bool, bool> save(geode::Mod* mod);
};
