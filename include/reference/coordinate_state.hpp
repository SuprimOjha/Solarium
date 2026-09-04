#pragma once

#include "solarium/math/vec3.hpp"
#include "solarium/reference/reference_frame.hpp"

namespace solarium::reference {

struct CoordinateState {
    math::Vec3 position{};
    math::Vec3 velocity{};

    ReferenceFrame frame =
        ReferenceFrame::Barycentric;
};

}