#include <chrono>
#include <Geode/loader/Types.hpp>

class SaveButtons {
public:
    static std::string format(const std::chrono::steady_clock::time_point& start, const std::chrono::steady_clock::time_point& end);
    static std::pair<bool, bool> save(geode::Mod* mod);
};
