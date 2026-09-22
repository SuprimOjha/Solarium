#include "solarium/reference/reference_frame.hpp"

namespace solarium::reference {

FrameDescriptor FrameDescriptor::barycentric(std::string orientation) {
    return {ReferenceFrame::Barycentric, FrameKind::Inertial, std::nullopt, std::move(orientation)};
}

FrameDescriptor FrameDescriptor::heliocentric(std::string orientation) {
    return {ReferenceFrame::Heliocentric, FrameKind::Inertial, ephemeris::BodyId::Sun, std::move(orientation)};
}

FrameDescriptor FrameDescriptor::geocentric(std::string orientation) {
    return {ReferenceFrame::Geocentric, FrameKind::Inertial, ephemeris::BodyId::Earth, std::move(orientation)};
}

FrameDescriptor FrameDescriptor::planetCentered(
    ephemeris::BodyId body,
    std::string orientation
) {
    return {ReferenceFrame::PlanetCentered, FrameKind::Inertial, body, std::move(orientation)};
}

FrameDescriptor FrameDescriptor::moonCentered(
    ephemeris::BodyId body,
    std::string orientation
) {
    return {ReferenceFrame::MoonCentered, FrameKind::Inertial, body, std::move(orientation)};
}

} // namespace solarium::reference
