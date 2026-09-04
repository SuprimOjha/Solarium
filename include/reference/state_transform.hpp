#pragma once

#include "solarium/reference/coordinate_state.hpp"

namespace solarium::reference {

class StateTransform {
public:
    [[nodiscard]]
    static CoordinateState transform(
        const CoordinateState& state,
        const CoordinateState& origin,
        ReferenceFrame targetFrame
    );

private:
    StateTransform() = delete;
};

}