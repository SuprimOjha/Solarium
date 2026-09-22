#pragma once

#include "solarium/reference/coordinate_state.hpp"

#include <stdexcept>
#include <string>

namespace solarium::reference {

enum class FrameTransformErrorCode {
    InvalidSourceFrame,
    InvalidTargetFrame,
    EpochMismatch,
    TimeScaleMismatch,
    MissingOriginState,
    MissingOrientationModel,
    RotatingFrameUnsupported
};

class FrameTransformError : public std::runtime_error {
public:
    FrameTransformError(FrameTransformErrorCode code, std::string message)
        : std::runtime_error(std::move(message)), code_(code) {}

    [[nodiscard]] FrameTransformErrorCode code() const noexcept {
        return code_;
    }

private:
    FrameTransformErrorCode code_;
};

struct FrameTransformContext {
    const CoordinateState* originState = nullptr;
};

class FrameTransformer {
public:
    [[nodiscard]] static CoordinateState transform(
        const CoordinateState& state,
        const FrameDescriptor& target,
        const FrameTransformContext& context
    );
};

} // namespace solarium::reference
