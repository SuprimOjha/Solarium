#include "solarium/reference/frame_transformer.hpp"

namespace solarium::reference {

CoordinateState FrameTransformer::transform(
    const CoordinateState& state,
    const FrameDescriptor& target,
    const FrameTransformContext& context
) {
    if (state.descriptor.frame != state.frame || state.descriptor.orientation.empty()) {
        throw FrameTransformError(
            FrameTransformErrorCode::InvalidSourceFrame,
            "source state has inconsistent frame metadata"
        );
    }
    if (target.orientation.empty()) {
        throw FrameTransformError(
            FrameTransformErrorCode::InvalidTargetFrame,
            "target frame has no orientation"
        );
    }
    if (state.frame == target.frame && state.descriptor.origin == target.origin &&
        state.descriptor.orientation == target.orientation) {
        return state;
    }
    if (state.descriptor.isRotating() || target.isRotating()) {
        throw FrameTransformError(
            FrameTransformErrorCode::MissingOrientationModel,
            "body-fixed transformations require an orientation model"
        );
    }

    const bool sourceIsBarycentric = state.frame == ReferenceFrame::Barycentric;
    const bool targetIsBarycentric = target.frame == ReferenceFrame::Barycentric;
    if (sourceIsBarycentric == targetIsBarycentric) {
        throw FrameTransformError(
            FrameTransformErrorCode::InvalidTargetFrame,
            "inertial transformations between two centered origins require an explicit two-origin model"
        );
    }
    if (context.originState == nullptr) {
        throw FrameTransformError(
            FrameTransformErrorCode::MissingOriginState,
            "origin state is required for inertial translation"
        );
    }

    const CoordinateState& origin = *context.originState;
    const ReferenceFrame requiredOriginFrame =
        sourceIsBarycentric ? state.frame : target.frame;
    if (origin.frame != requiredOriginFrame ||
        origin.epoch.value() != state.epoch.value()) {
        throw FrameTransformError(
            FrameTransformErrorCode::EpochMismatch,
            "origin state does not match required frame, epoch, or time scale"
        );
    }
    if (origin.timeScale != state.timeScale) {
        throw FrameTransformError(
            FrameTransformErrorCode::TimeScaleMismatch,
            "origin state does not match source time scale"
        );
    }

    CoordinateState result = state;
    result.frame = target.frame;
    result.descriptor = target;
    result.center = target.origin;
    if (sourceIsBarycentric) {
        result.position = state.position - origin.position;
        result.velocity = state.velocity - origin.velocity;
    } else {
        result.position = state.position + origin.position;
        result.velocity = state.velocity + origin.velocity;
    }
    return result;
}

} // namespace solarium::reference
