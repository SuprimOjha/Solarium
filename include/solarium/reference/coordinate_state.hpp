#pragma once

#include "solarium/math/vec3.hpp"
#include "solarium/reference/reference_frame.hpp"
#include "solarium/time/julian_date.hpp"
#include "solarium/time/time_scale.hpp"

namespace solarium::reference {

struct CoordinateState {
    math::Vec3 position{};
    math::Vec3 velocity{};

    ReferenceFrame frame =
        ReferenceFrame::Barycentric;

    time::JulianDate epoch{};
    time::TimeScale timeScale = time::TimeScale::TDB;
    std::optional<ephemeris::BodyId> center;
    FrameDescriptor descriptor = FrameDescriptor::barycentric();
};

}