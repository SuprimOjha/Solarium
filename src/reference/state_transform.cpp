#include "solarium/reference/state_transform.hpp"
#include "solarium/reference/frame_transform.hpp"

namespace solarium::reference {

CoordinateState StateTransform::transform(
    const CoordinateState& state,
    const CoordinateState& origin,
    ReferenceFrame targetFrame
) {
    if (state.frame == targetFrame) {
        return state;
    }

    CoordinateState result;

    result.frame = targetFrame;

    result.position =
        FrameTransform::position(
            state.position,
            origin.position,
            state.frame,
            targetFrame
        );

    result.velocity =
        FrameTransform::velocity(
            state.velocity,
            origin.velocity,
            state.frame,
            targetFrame
        );

    return result;
}

}