#include "solarium/ephemeris/body_id.hpp"

namespace solarium::ephemeris {

std::string_view bodyName(BodyId body) noexcept {
    constexpr std::string_view names[] = {
        "Sun", "Mercury", "Venus", "Earth", "Moon", "Mars",
        "Phobos", "Deimos", "Jupiter", "Io", "Europa", "Ganymede",
        "Callisto", "Saturn", "Mimas", "Enceladus", "Tethys", "Dione",
        "Rhea", "Titan", "Iapetus", "Uranus", "Miranda", "Ariel",
        "Umbriel", "Titania", "Oberon", "Neptune", "Triton"
    };

    const auto index = static_cast<std::size_t>(body);
    return index < (sizeof(names) / sizeof(names[0])) ? names[index] : std::string_view{};
}

} // namespace solarium::ephemeris
