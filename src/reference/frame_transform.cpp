#include "solarium/reference/frame_transform.hpp"

#include <stdexcept>

namespace solarium::reference {

math::Vec3 FrameTransform::position(
    const math::Vec3& position,
    const math::Vec3& originPosition,
    ReferenceFrame from,
    ReferenceFrame to
) {
    if (from == to) {
        return position;
    }

    if (from == ReferenceFrame::Barycentric &&
        to == ReferenceFrame::Heliocentric) {
        return position - originPosition;
    }

    if (from == ReferenceFrame::Heliocentric &&
        to == ReferenceFrame::Barycentric) {
        return position + originPosition;
    }

    throw std::invalid_argument("unsupported reference-frame position transformation");
}

math::Vec3 FrameTransform::velocity(
    const math::Vec3& velocity,
    const math::Vec3& originVelocity,
    ReferenceFrame from,
    ReferenceFrame to
) {
    if (from == to) {
        return velocity;
    }

    if (from == ReferenceFrame::Barycentric &&
        to == ReferenceFrame::Heliocentric) {
        return velocity - originVelocity;
    }

    if (from == ReferenceFrame::Heliocentric &&
        to == ReferenceFrame::Barycentric) {
        return velocity + originVelocity;
    }

    throw std::invalid_argument("unsupported reference-frame velocity transformation");
}

}