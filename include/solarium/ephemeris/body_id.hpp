#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace solarium::ephemeris {

enum class BodyId : std::uint16_t {
    Sun,
    Mercury,
    Venus,
    Earth,
    Moon,
    Mars,
    Phobos,
    Deimos,
    Jupiter,
    Io,
    Europa,
    Ganymede,
    Callisto,
    Saturn,
    Mimas,
    Enceladus,
    Tethys,
    Dione,
    Rhea,
    Titan,
    Iapetus,
    Uranus,
    Miranda,
    Ariel,
    Umbriel,
    Titania,
    Oberon,
    Neptune,
    Triton
};

[[nodiscard]] constexpr bool isKnownBody(BodyId body) noexcept {
    return static_cast<std::size_t>(body) < 29U;
}

[[nodiscard]] std::string_view bodyName(BodyId) noexcept;

} // namespace solarium::ephemeris
