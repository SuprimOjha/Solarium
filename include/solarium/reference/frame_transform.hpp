#pragma once

#include "solarium/math/vec3.hpp"
#include "solarium/reference/reference_frame.hpp"

namespace solarium::reference {

class FrameTransform {
public:
    [[nodiscard]]
    static math::Vec3 position(
        const math::Vec3& position,
        const math::Vec3& originPosition,
        ReferenceFrame from,
        ReferenceFrame to
    );

    [[nodiscard]]
    static math::Vec3 velocity(
        const math::Vec3& velocity,
        const math::Vec3& originVelocity,
        ReferenceFrame from,
        ReferenceFrame to
    );
};

}