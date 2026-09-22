#pragma once

#include "solarium/ephemeris/body_id.hpp"

#include <optional>
#include <string>

namespace solarium::reference {

enum class ReferenceFrame {
    Barycentric,
    Heliocentric,
    Geocentric,
    PlanetCentered,
    MoonCentered
};

enum class FrameKind {
    Inertial,
    BodyFixed
};

struct FrameDescriptor {
    ReferenceFrame frame = ReferenceFrame::Barycentric;
    FrameKind kind = FrameKind::Inertial;
    std::optional<ephemeris::BodyId> origin;
    std::string orientation = "J2000";

    [[nodiscard]] static FrameDescriptor barycentric(
        std::string orientation = "J2000"
    );

    [[nodiscard]] static FrameDescriptor heliocentric(
        std::string orientation = "J2000"
    );

    [[nodiscard]] static FrameDescriptor geocentric(
        std::string orientation = "J2000"
    );

    [[nodiscard]] static FrameDescriptor planetCentered(
        ephemeris::BodyId body,
        std::string orientation = "J2000"
    );

    [[nodiscard]] static FrameDescriptor moonCentered(
        ephemeris::BodyId body,
        std::string orientation = "J2000"
    );

    [[nodiscard]] bool isRotating() const noexcept {
        return kind == FrameKind::BodyFixed;
    }
};

}